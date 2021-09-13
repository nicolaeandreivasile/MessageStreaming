#include <netinet/in.h>
#include "list.h"

#ifndef _STRUCT_H
#define _STRUCT_H 1

#define BUFLEN 1600
#define CLIENTS 25

#define TOPICLEN 50
#define CONTENTLEN 1500
#define NAMELEN 10

//~~ UDP message structure ~~//
typedef struct udp_msg {
    
    char topic[TOPICLEN];
    char content[CONTENTLEN];
} udp_message;

#define TOPIC(u)	((udp_message*) u)->topic
#define CONTENT(u)	((udp_message*) u)->content

//~~ Package structure ~~//
typedef struct pkg {

    struct sockaddr_in cli_info;
    udp_message msg;
} package;

#define CLI_INFO(p)	((package*) p)->cli_info
#define MSG(p)		((package*) p)->msg

//~~ Topic structure ~~//
typedef struct tpc {

    char topic[TOPICLEN];
    int sf;
} topic;

#define TPC(t)		((topic *) t)->topic
#define SF(t)		((topic *) t)->sf

//~~ Topic functions ~~//
int cmp_tpcs(void *x1, void *x2);
int cmp_tpcwiths(void *x1, void *x2);

//~~ Subscriber structure ~~//
typedef struct subs {

    char name[NAMELEN];
    list* topics;
    list* messages;
} subscriber;

#define NAME(s)		((subscriber*) s)->name
#define TOPICS(s)	((subscriber*) s)->topics
#define MSGS(s)		((subscriber*) s)->messages

//~~ Subscriber functions ~~//
int cmp_subs(void *x1, void*x2);
int cmp_swiths(void *x1, void *x2);

//~~ Socket-Subscriber structure ~~//
typedef struct ss {

    int sockfd;
    char name_subs[NAMELEN];
} sock_subs;

#define NAME_SS(ss)	((sock_subs*) ss)->name_subs
#define SOCKFD(ss)	((sock_subs*) ss)->sockfd

//~~ Socket-Subscriber functions ~~//
int cmp_nss(void *x1, void *x2);
int cmp_ss(void *x1, void *x2);
int cmp_nsswiths(void *x1, void *x2);
int cmp_sswiths(void *x1, void *x2);


//~~ Server-Client connection initialization functions ~~//
void init_serv_sockaddr(struct sockaddr_in *addr, char *port);
void init_cli_sockaddr (struct sockaddr_in *addr, char *ip, char *port);
void init_tcp_socket (int *sockfd, int *opt, socklen_t optlen);
void init_udp_socket (int *sockfd);
void connect_socket (int *sockfd, struct sockaddr_in *addr, socklen_t addrlen,
					char* name);
void bind_socket (int *sockfd, struct sockaddr *addr, socklen_t addrlen);
void listen_socket(int *sockfd, int nsubs);

#endif
