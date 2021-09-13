#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "struct.h"
#include "list.h"
#include "error.h"

/* print string type message */
void string_type(struct sockaddr_in cli_addr, udp_message msg) {
    fprintf(stdout, "%s:%d - %s: %s\n", inet_ntoa(cli_addr.sin_addr),
		    ntohs(cli_addr.sin_port), TOPIC(&msg), CONTENT(&msg));
}

/* recieve package from server
 * printf inside message accordingly
 */
void recv_package (int *sockfd) {

    int nbytes;
    char buffer[BUFLEN];
    struct sockaddr_in cli_addr;
    udp_message msg;

    memset(buffer, 0, BUFLEN);
            
    nbytes = recv((*sockfd), buffer, BUFLEN, 0);
    DIE(nbytes < 0, "Failed recievind message");

    if (strncmp(buffer, "exit", 4) == 0) {
	close(*sockfd);
        exit(1);
    }

    package *p = (package *) buffer;
    cli_addr = CLI_INFO(p);
    msg = MSG(p);

    string_type(cli_addr, msg);
}

/* send command to server */
void send_command (int *sockfd) {

    int nbytes;
    char buffer[BUFLEN], aux_buffer[BUFLEN];
    char *command, *topic, *sf, *delim;

    memset(buffer, 0, BUFLEN);
    fgets(buffer, BUFLEN, stdin);
    memcpy(aux_buffer, buffer, BUFLEN);

    if (strncmp(buffer, "exit", 4) == 0) {
        close(*sockfd);
        exit(1);
    }
   
    delim = " ";
    command = strtok(aux_buffer, delim);
    topic = strtok(NULL, delim);
    sf = strtok(NULL, delim);

    if (strncmp(command, "subscribe", 9) == 0 && topic && sf) {
	buffer[strnlen(buffer, BUFLEN) - 1] = '\0';
        nbytes = send((*sockfd), buffer, BUFLEN, 0);
        DIE(nbytes < 0, "Failed sending message");

        fprintf(stdout, "Subscribed %s\n", topic);
    } else if (strncmp(buffer, "unsubscribe", 11) == 0 && topic) {
        buffer[strnlen(buffer, BUFLEN) - 1] = '\0';
        nbytes = send((*sockfd), buffer, BUFLEN, 0);
        DIE(nbytes < 0, "Failed sending message");

        fprintf(stdout, "Unsubscribed %s\n", topic);
    } else {
        fprintf(stderr, "COMMANDS: subscribe <TOPIC> <SF>, unsubscribe <TOPIC>\n");
    }
}

/* main */
int main (int argc, char *argv[]) {

    int sockfd;
    int ret, opt = 1;
    struct sockaddr_in serv_addr;

    USAGE(argc < 4, 
	"USAGE: ./subscriber <SUBSCRIBER_NAME> <SERVER_IP> <SERVER_PORT>");

    char* name = argv[1];
    char* ip = argv[2];
    char* port = argv[3];

    init_cli_sockaddr(&serv_addr, ip, port);
    init_tcp_socket(&sockfd, &opt, sizeof(opt));
    connect_socket(&sockfd, &serv_addr, sizeof(serv_addr), name);

    fd_set read_fds;
    fd_set tmp_fds;
    int fdmax;

    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    FD_SET(0, &read_fds);
    FD_SET(sockfd, &read_fds);
    fdmax = sockfd;
	
    while (1) {
        tmp_fds = read_fds;

        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");
 
        if (FD_ISSET(sockfd, &tmp_fds))	{
            recv_package(&sockfd);
	} else if (FD_ISSET(0, &tmp_fds)) {
            send_command(&sockfd);
    	}
    }

    close(sockfd);

    return 0;
}
