#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "struct.h"
#include "list.h"
#include "error.h"

/* UDP messages */
udp_message *make_udp_message(char *buf, int *sockfd) {

    if (strncmp(buf, "exit", 4) == 0) {
	    close(*sockfd);
	    exit(1);
    }	   

    udp_message *udp_msg = (udp_message *) calloc(1, sizeof(udp_message));
    INITFAIL(!udp_msg, "udp message");

    char *topic = strtok(buf, ":");
    char *content = strtok(NULL, "\0");
   
    memcpy(TOPIC(udp_msg), topic, strnlen(topic, TOPICLEN));
    memcpy(CONTENT(udp_msg), content, strnlen(content, CONTENTLEN));

    return udp_msg;
}

/* main function */
int main (int argc, char *argv[]) {

    int sockfd;
    int count, nbytes;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr;

    USAGE(argc < 3, "USAGE: ./client <SERVER_IP> <SERVER_PORT>");
    
    char *ip = argv[1];
    char *port = argv[2];

    init_cli_sockaddr(&serv_addr, ip, port);
    init_udp_socket(&sockfd);

    while (1) {
	if ((count = read(0, buffer, CONTENTLEN))) {

	    udp_message *udp_msg = make_udp_message(buffer, &sockfd);
	    memcpy(buffer, udp_msg, sizeof(udp_message));
	
	    nbytes = sendto(sockfd, buffer, BUFLEN, 0, 
			(struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in));
	    DIE(nbytes < 0, "Failed sending message");
	    
	    free(udp_msg);
	}
    }

    close(sockfd);

    return 0;
}
