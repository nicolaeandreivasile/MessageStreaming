#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "struct.h"
#include "list.h"
#include "error.h"

/* send exit command to subscribers */
void send_command(int *tcp_sockfd, int *udp_sockfd, fd_set *write_fds, int fdmax) {

    int index, nbytes;
    char buffer[BUFLEN];

    memset(buffer, 0, BUFLEN);
    fgets(buffer, BUFLEN - 1, stdin);

    if (strncmp(buffer, "exit", 4) == 0) {
        for (index = 0; index <= fdmax; index++) {
            if (FD_ISSET(index, write_fds)) {
                nbytes = send(index, buffer, BUFLEN, 0);
		DIE(nbytes < 0, "Failed send");
            }
        }

	close(*tcp_sockfd);
	close(*udp_sockfd);
        exit(1);
    }
}

/* stabilize new tcp connection 
 * if the subscriber has been connected in the past, the connection is 
 * reestablished and offline messages are being sent
 */
void new_tcp_connection(int *sockfd, struct sockaddr_in *addr, socklen_t *addrlen, 
			fd_set *read_fds, int *fdmax, 
			list** subs, list** online_subs) {
    
    int nbytes, newsockfd;
    char buffer[BUFLEN];
    subscriber s;
    sock_subs ss;

    newsockfd = accept((*sockfd), (struct sockaddr *) addr, addrlen);
    DIE(newsockfd < 0, "accept");

    FD_SET(newsockfd, read_fds);
    if (newsockfd > *fdmax) { 
        *fdmax = newsockfd;
    }

    memset(buffer, 0, BUFLEN);
    nbytes = recv(newsockfd, buffer, BUFLEN, 0);
    DIE(nbytes < 0, "Failed receiving subscriber name");   

    memset(&ss, 0, sizeof(sock_subs));
    SOCKFD(&ss) = newsockfd;
    memcpy(NAME_SS(&ss), buffer, NAMELEN);
    add_cell(online_subs, &ss, sizeof(sock_subs), cmp_ss);

    if (contain_cell(*subs, NAME_SS(&ss), cmp_swiths) > 0) {
        list *sub = get_cell(*subs, NAME_SS(&ss), cmp_swiths);
        list *message;

        while ((message = MSGS(INFO(sub)))) {
            memcpy(buffer, INFO(message), BUFLEN);
            nbytes = send(newsockfd, buffer, BUFLEN, 0);
            DIE(nbytes < 0, "Failed sending offline messages");
	    remove_first(&MSGS(INFO(sub))); //to do
        }
    } else {
        memset(&s, 0, sizeof(subscriber));
        memcpy(NAME(&s), buffer, NAMELEN);
        TOPICS(&s) = NULL;
	MSGS(&s) = NULL;
        add_cell(subs, &s, sizeof(subscriber), cmp_subs);
    }
    printf("New client %s connected from %s:%d\n", buffer, 
		    inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
}

/* recieve new UDP message 
 * send messages for online subscribers and save messages for sore&forward
 * topics
 */
void new_udp_message(int *sockfd, struct sockaddr_in *addr, socklen_t *addrlen, 
			fd_set *write_fds, int *fdmax, 
			list** subs, list** online_subs) {

    int nbytes;
    char buffer[BUFLEN];
    udp_message um;
    package p;
    list *sub_list;

    memset(buffer, 0, BUFLEN);
    nbytes = recvfrom((*sockfd), buffer, BUFLEN, 0, 
		    (struct sockaddr *) addr, addrlen);
    DIE(nbytes < 0, "Failed receiving UDP message ");

    memcpy(&um, buffer, sizeof(udp_message));
    
    memcpy(&CLI_INFO(&p), addr, sizeof(struct sockaddr_in));
    memcpy(&MSG(&p), &um, sizeof(udp_message));
    memcpy(buffer, (char *) &p, sizeof(package));

    for (sub_list = *subs; sub_list != NULL; sub_list = NEXT(sub_list)) {
	if (contain_cell(TOPICS(INFO(sub_list)), TOPIC(&um), cmp_tpcwiths) > 0) {
            if (contain_cell(*online_subs, NAME(INFO(sub_list)), cmp_nsswiths) > 0) {
	        list *sock_cell = get_cell(*online_subs, NAME(INFO(sub_list)), 
						cmp_nsswiths);
                if (FD_ISSET(SOCKFD(INFO(sock_cell)), write_fds)) {
		    nbytes = send(SOCKFD(INFO(sock_cell)), buffer, BUFLEN, 0);
                    DIE(nbytes < 0, "send");
                }
	    } else {
	        list *topic_cell = get_cell(TOPICS(INFO(sub_list)), 
						TOPIC(&um), cmp_tpcwiths);
                if (SF(INFO(topic_cell)) == 1) {
                    add_last(&MSGS(INFO(sub_list)), &buffer, BUFLEN);
	        }
	    }
	}	
    }
}

/* receive new TCP message 
 * send subscribe/unsubscribe commands to server
 */
void new_tcp_message(int *sockfd, fd_set *read_fds, int *fdmax, 
			list** subs, list** online_subs) {

    int nbytes;
    char buffer[BUFLEN];

    memset(buffer, 0, BUFLEN);
    nbytes = recv((*sockfd), buffer, BUFLEN, 0);
    DIE(nbytes < 0, "recv");
		    
    if (nbytes == 0) {
        list* cell = get_cell(*online_subs, sockfd, cmp_sswiths);

	printf("Client %s disconnected\n", NAME_SS(INFO(cell)));
			
        close((*sockfd));
        FD_CLR((*sockfd), read_fds);
	remove_cell(online_subs, sockfd, cmp_sswiths);
    } else {
        char *delim = " ";
        char *command = strtok(buffer, delim);
        char *topic_name = strtok(NULL, delim);
        
        if (strncmp(command, "subscribe", 9) == 0) {
            int sf = atoi(strtok(NULL, delim));
	    topic t;

      	    memset(&t, 0, sizeof(topic));
            memcpy(TPC(&t), topic_name, strnlen(topic_name, TOPICLEN));
            SF(&t) = sf;

            list *cell1 = get_cell(*online_subs, sockfd, cmp_sswiths);
	    list *cell2 = get_cell(*subs, NAME_SS(INFO(cell1)), cmp_swiths);
            add_cell(&TOPICS(INFO(cell2)), &t, sizeof(topic), cmp_tpcs);
	} else if (strncmp(command, "unsubscribe", 11) == 0) {
            list *cell1 = get_cell(*online_subs, sockfd, cmp_sswiths);
	    list *cell2 = get_cell(*subs, NAME_SS(INFO(cell1)), cmp_swiths);
            remove_cell(&TOPICS(INFO(cell2)), topic_name, cmp_tpcwiths);
	}
    }
}

/* main */
int main(int argc, char *argv[]) {

    int i, ret, opt = 1;
    int tcp_sockfd, udp_sockfd;
    struct sockaddr_in serv_addr, cli_addr; 
    socklen_t clilen;
    list *subs = NULL;
    list *online_subs = NULL;

    USAGE(argc < 2, "USAGE: ./server <SERVER_PORT>");

    char *port = argv[1];

    init_serv_sockaddr(&serv_addr, port);

    init_tcp_socket(&tcp_sockfd, &opt, sizeof(opt));
    bind_socket(&tcp_sockfd, (struct sockaddr *) &serv_addr, 
		    sizeof(struct sockaddr));
    listen_socket(&tcp_sockfd, CLIENTS);

    init_udp_socket(&udp_sockfd);
    bind_socket(&udp_sockfd, (struct sockaddr *) &serv_addr, 
		    sizeof(struct sockaddr));

    fd_set write_fds;
    fd_set read_fds;
    fd_set tmp_fds;
    int fdmax;

    FD_ZERO(&write_fds);
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    FD_SET(0, &read_fds);
    FD_SET(tcp_sockfd, &read_fds);
    FD_SET(udp_sockfd, &read_fds);
    fdmax = udp_sockfd;

    // connection loop
    while (1) {
        tmp_fds = read_fds; 
		
        write_fds = read_fds;
        FD_CLR(0, &write_fds);
	FD_CLR(tcp_sockfd, &write_fds);
        FD_CLR(udp_sockfd, &write_fds);

        ret = select(fdmax + 1, &tmp_fds, &write_fds, NULL, NULL);
        DIE(ret < 0, "select");

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {
		if (i == 0) {
                    send_command(&tcp_sockfd, &udp_sockfd, &write_fds, fdmax);
	        } else if (i == tcp_sockfd) {
                    clilen = sizeof(cli_addr);
		    new_tcp_connection(&i, &cli_addr, &clilen, 
					&read_fds, &fdmax, &subs, &online_subs);
                } else if (i == udp_sockfd) {
                    new_udp_message(&i, &cli_addr, &clilen, 
				    &write_fds, &fdmax, &subs, &online_subs);
	        } else {
                    new_tcp_message(&i, &read_fds, &fdmax, &subs, &online_subs);
                }
	    }
        }
    }

    close(tcp_sockfd);
    close(udp_sockfd);

    return 0;
}
