# Client-Server messaging service

## Description
A server-client application that uses TCP and UDP sockets for computing messages. The subscribers can subscribe to different topics and receive messages from that topic. A S&F option is implemented to receive all the messages from a topic, even if the subscriber is offline for a period of time.


## Components

### Server
Realises the connection between TCP and UDP clients and initializes TCP and UDP passive sockets and the server address. It binds both sockets to the server address. The socket sets are being initilized. For each iteration it selects the open read and write sockets and computes the given 
operations.

#### Operations
- **send_command()** - the "exit" command is read from stdin and it closes all the TCP clients and the server.
- **new_tcp_connection()** - a new connection is received from the passive TCP socket. A new active socket is being initialized for the new client. The subscriber structures are being initialized. If the subscriber has been connected to the server in the past then the old connection is being reestablished and the saved messages from the topics with store&forward are being sent to the current client. If not, it is added to the subscribers' lists. A message is being printed.
- **new_udp_message()** - a new message is being received from an UDP client. From the subscriber's list, each client's topics list is checked. If the current is in the list: If the client is online, send the message ot it; if not, save the message if the client selected the store&forward option for this topic.
- **new_tcp_message()** - a new TCP message is being received from a TCP client. If it is a subsribe command, it adds the given topic to the subscriber's topic list accordingly. If it is an unsubscribe command, it removes the topic from the subscriber's topic list.

#### Usage
./server [SERVER_PORT]


### Subscriber: 
TCP client. Initializes TCP passive socket and the server address. It connects the socket to the server address. The socket sets are being initialized. For each iteration it selects the proper socket to assure the correct functionality of the client.
    
#### Operations
- **send_command()** - sends the correct commands to the server(subscribe / unsubscribe), or it closes the client if the "exit" command is read.
- **recv_package()** - receive the package from an UDP client (via server). It computes the message and it prints the outcome accordingly(INT, SHORT_REAL, REAL, STRING).
   
#### Usage
./subscriber NAME [SERVER_IP] [SERVER_PORT]


### Stream:
UDP client. Initializes UDP passive socket and the server address. A true loop makes sure that the stream is on as long as it is needed.

#### Operations
- **make_udp_message()** - makes the UDP message with the specified UDP format;
    
#### Usage
./stream [SERVER_IP] [SERVER_PORT]


## Other
  
### Additional files:
- **list.h** - the library where the list structure is defined.
- **list.c** - the C file where the necessary functions for the list structure are defined.
- **struct.h** - the library where the following structures are being defined: *udp_message* (the UDP message structure), *package* (the message structure to be sent to TCP clints), *topic* (the topic structure), *subscriber* (the subscriber structure), *sock_subs* (the subscriber structure that connects the socket to the subscriber's name).
- **struct.c** - the C file where the server-client connection initialization functions are being defined, and also the compare functions for each structure.
- **error.h** - the library where the error MACROs are being defined.
