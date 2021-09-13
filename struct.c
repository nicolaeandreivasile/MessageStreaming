#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "struct.h"
#include "error.h"

//~~ Server-Client connection initialization functions ~~//

/* initialize socket address structure */
void init_serv_sockaddr(struct sockaddr_in *addr, char* port) {

    memset((char *) addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(atoi(port));
    addr->sin_addr.s_addr = INADDR_ANY;
}

/*  initialize socket address structure */
void init_cli_sockaddr (struct sockaddr_in *addr, char *ip, char *port) {

    int ret;

    memset((char *) addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(atoi(port));
    ret = inet_aton(ip, &(addr->sin_addr));
    DIE(ret == 0, "inet_aton");
}

/* initialize TCP socket
 * set socket opetions
 */
void init_tcp_socket (int *sockfd, int *opt, socklen_t optlen) {

    int ret;

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE((*sockfd) < 0, "Failed TCP socket initialization");

    ret = setsockopt((*sockfd), IPPROTO_TCP, TCP_NODELAY, opt, optlen);
    DIE(ret < 0, "Faield setting TCP socket options");
}

/* initialize UDP socket */
void init_udp_socket (int *sockfd) {
    
    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    DIE((*socket) < 0, "Failed UDP socket initialization");
}

/* conect to server via TCP socket
 * send name of current subscriber
 */
void connect_socket (int *sockfd, struct sockaddr_in *addr, socklen_t addrlen,
			char* name) {

    int ret, nbytes;
    char buffer[BUFLEN];

    ret = connect((*sockfd), (struct sockaddr*) addr, addrlen);
    DIE(ret < 0, "Failed connection to server");

    memset(buffer, 0, BUFLEN);
    memcpy(buffer, name, strnlen(name, BUFLEN));

    nbytes = send((*sockfd), buffer, BUFLEN, 0);
    DIE(nbytes < 0, "Failed sending subscriber name");
}

/* bind TCP/UDP socket */
void bind_socket (int *sockfd, struct sockaddr *addr, socklen_t addrlen) {

    int ret;

    ret = bind((*sockfd), addr, addrlen);
    DIE(ret < 0, "Failed binding");
}

/* listen TCP socket */
void listen_socket(int *sockfd, int nsubs) {

    int ret;

    ret = listen((*sockfd), nsubs);
    DIE(ret < 0, "Failed listening");
}

//~~ Topic functions ~~//

/* compare 2 topics by name */
int cmp_tpcs(void *x1, void *x2) {

    return strncmp(TPC(x1), TPC(x2), TOPICLEN);
}

/* compare a topic's name with a string */
int cmp_tpcwiths(void *x1, void *x2) {

    return strncmp(TPC(x1), (char *) x2, TOPICLEN);
}

//~~ Subscriber functions ~~//

/* compare 2 subscribers by name */
int cmp_subs (void *x1, void *x2) {

    return strncmp(NAME(x1), NAME(x2), NAMELEN);
}

/* compare a subscriber's name with a string */
int cmp_swiths(void *x1, void *x2) {

    return strncmp(NAME(x1), (char *) x2, NAMELEN);
}

//~~ Socker-Subscriber functions ~~//

/* compare 2 socket-subs by name */
int cmp_nss (void *x1, void *x2) {

    return strncmp(NAME_SS(x1), NAME_SS(x2), NAMELEN);
}

/* compare 2 socket-subs by sockets */
int cmp_ss (void *x1, void *x2) {
    return SOCKFD(x1) - SOCKFD(x2);
}

/* compare a soclet-subs's name with a string */
int cmp_nsswiths (void *x1, void *x2) {

    return strncmp(NAME_SS(x1), (char *) x2, NAMELEN);
}

/* compare a socket-subs's socket with a number */
int cmp_sswiths (void *x1, void *x2) {
    return SOCKFD(x1) - *(int *) x2;
}

