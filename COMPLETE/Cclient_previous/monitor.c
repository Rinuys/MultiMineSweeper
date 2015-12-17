#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>

#define MAXDATASIZE (BOARD_SIZE*BOARD_SIZE*3) // max number of bytes we can get at once 
#define BOARD_SIZE 20
#define ADDRESS "vpn.jeonghan.wo.tc"

int board[BOARD_SIZE][BOARD_SIZE];

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void unpack(int board[BOARD_SIZE][BOARD_SIZE], char pkg[])
{
    char *token;
    token = strtok(pkg, ",");
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            board[i][j] = atoi(token);
            token = strtok(NULL, ",");
        }
    }
}

void print_board()
{   
       for(int row = 0; row < BOARD_SIZE; row++){
        
         for(int col = 0; col < BOARD_SIZE; col++){
                if(board[row][col] == 10){
                    printf("* ");
                }else if(board[row][col] == 0){
                    printf("o ");
                }else if(board[row][col] == 12){
                    printf("  ");
                }else if(board[row][col] == 11){
                    printf("\033[31mo \033[0m");
                }else
                    printf("%d ", board[row][col]);            
            }
                printf("|%d\n", row+1);
    }
    printf("\n\n");
}
int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char xy[10];
    struct timespec ts;
    char port[10];
    int port_temp;
    ts.tv_sec = 0;
    ts.tv_nsec = 400000000;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if(argc < 2){
        fprintf(stderr, "Usage: %s <room number> (e.g: %s 1)\n", argv[0], argv[0]);
        exit(1);
    }
    port_temp = atoi(argv[1])+8999;
    sprintf(port, "%d", port_temp);
    if ((rv = getaddrinfo(ADDRESS, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

   
    while(1){
		char ref_buf[5];
		memset(buf,0,sizeof(buf));
        send(sockfd, "refresh", 10, 0);
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv");
            exit(1);
        }
		if ((numbytes = recv(sockfd, ref_buf, 5, 0)) == -1) {
            perror("recv");
            exit(1);
        }
		unpack(board, buf);
        print_board();
        nanosleep(&ts,NULL);
    }
    
    return 0;    

}
    

