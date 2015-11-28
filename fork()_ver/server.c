#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#define BUFF_SIZE 1024
#define MAX_PLAYER 100

int new_server_socket();
void server();

int new_server_socket(){
        int server_socket = socket(PF_INET,SOCK_STREAM,0);
        if(server_socket == -1){
                perror("server socket error");
                exit(1);
        }
        else printf("서버소켓 생성 성공\n");
        return server_socket;
}

void server(){
        int server_socket;
        struct sockaddr_in server_addr;
        char buff_rcv[BUFF_SIZE+5];
        char buff_snd[BUFF_SIZE+5];

        server_socket = new_server_socket();

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(server_socket,(struct sockaddr*)&server_addr, sizeof(server_addr))
                == -1){
                perror("bind() error");
                exit(1);
		}
        else printf("바인드성공\n");

        if(listen(server_socket, 5)==-1){
                perror("listen() error");
                exit(1);
        }
        else printf("listen() 성공\n");

        pid_t pid[MAX_PLAYER];
        int client_socket[MAX_PLAYER];
        int pidtop = 0;
        int client_socket_top = 0;
        int client_addr_size;
        struct sockaddr_in client_addr[MAX_PLAYER];
        int client_addr_top = 0;
        char *str="OK";
        while(1){
                printf("main server : 연결 기다리는 중 \n");
                client_addr_size = sizeof(client_addr[0]);
                client_socket[client_socket_top] = accept(server_socket,
                                (struct sockaddr*)&client_addr[client_addr_top], &client_addr_size);
                if(client_socket[client_socket_top] == -1){
                        perror("연결 실패");
                        exit(1);
                }
                else    printf("연결 성공\n");

                switch(pid[pidtop] = fork()){
                case -1:
                        perror("fork");
                        exit(1);
                        break;
                case 0 :
                        printf("(chi)fork 성공  pidtop:%d\n",pidtop);
                        while(1){
                                read(client_socket[client_socket_top], buff_rcv, BUFF_SIZE);
                                printf("(%d)receive:%s\n",pidtop,buff_rcv);
                                if(!strcmp(buff_rcv,"exit")) break;
                                strcpy(buff_snd,str);
                                write(client_socket[client_socket_top], buff_snd, strlen(buff_snd)+1);
                                printf("(%d)send:%s\n",pidtop,buff_snd);
						}
                        printf("(chi)종료 pidtop:%d\n",pidtop);
                        exit(1);
                        break;
                default :
                        client_socket_top++;
                        client_addr_top++;
                        pidtop++;
                        break;
                }
        }
        close(server_socket);
}

int main(void){
        server();
        return 0;
}