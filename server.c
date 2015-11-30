#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9999"   // port we're listening on

int fdmax;
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int Init_Server(void)
{
    int listener;     // listening socket descriptor
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    struct addrinfo hints, *ai, *p;
    int rv;

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai))!= 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }
        printf("Step1: socket has been created successfully!\n");
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        printf("Step2: bind successfully!\n");
        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }
    printf("Step 3: listen successfully!\n");
    printf("===============Server is ready to go===============\n");
    return listener;
}
void Select(fd_set *read_fds)
{
    if (select(fdmax+1, read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
}
bool is_connection(fd_set read_fds, int listener) // 0: new connection
{
    for(int i = 0; i <= fdmax; i++){
        if(FD_ISSET(i, &read_fds)){
            if(i == listener)
                return true;
        }
    }
    return false;
}

int main(int argc, char const *argv[])
{
    int listener; // listener
    int newfd;    // for new connection
    int nbytes;   // received number of bytes
    char buf[20]; // receive buffer
    fd_set read_fds; // read_fds
    fd_set master;
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];

    FD_ZERO(&read_fds);
    FD_ZERO(&master);

    listener = Init_Server();
    FD_SET(listener, &master), fdmax = listener;  // add listener to read_fds set

    for(;;){
            printf("while running!\n");
            read_fds = master;
            Select(&read_fds);
            if(is_connection(read_fds, listener)){ // handle new connection here
                addrlen = sizeof remoteaddr;
                newfd = accept(listener,(struct sockaddr *)&remoteaddr,&addrlen);
                if(newfd == -1){
                    perror("accept");
                }else{
                    FD_SET(newfd, &master); // add to master set 
                    if(newfd > fdmax){
                        fdmax = newfd;
                    }
                    printf("New connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                }
            }else{ // handle data here

                for(int i = 0; i <= fdmax; i++){
                    if(FD_ISSET(i, &read_fds)){
                        if(i == listener){
                            continue;
                        }else{
                        if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                            // got error or connection closed by client
                            if (nbytes == 0) {
                                // connection closed
                                printf("Server: socket %d hung up\n", i);
                            } else {
                                perror("recv");
                            }
                            close(i); // connection closed or error occured
                            FD_CLR(i, &master); // remove from read set
                        }else{

                            if(!strcmp(buf, "start")){
                                send(i, "I'm ready", 10, 0); // send mine array, whether the game running or not.
                            }else if(!strcmp(buf, "exit")){
                                close(i);
                                FD_CLR(i, &read_fds);
                                printf("Client(%d) closed!\n", i);
                            }else{
                                // receive coordinate, re-processing data and brodcast to all client.
                            }
                        }
                    } 
                }
            }
        }
    }

    return 0;
}
