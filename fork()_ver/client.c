#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>

#define BUFF_SIZE 1024

int main(int argc, char **argv){
        int client_socket;

        struct sockaddr_in server_addr;

        char buff[BUFF_SIZE+5];

        char str[BUFF_SIZE+5];

        client_socket = socket(PF_INET, SOCK_STREAM, 0);
        if(client_socket == -1){
                perror("socket 생성 실패");
                exit(1);
        }
        else printf("socket 생성 성공\n");

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4000);
        server_addr.sin_addr.s_addr=inet_addr("128.199.81.97");
        if(connect(client_socket,(struct sockaddr*)&server_addr,
                sizeof(server_addr))){
                perror("접속 실패");
                exit(1);
        }
        else    printf("접속 성공\n");
        while(1){
                printf("write string : ");
                scanf("%s",str);
                write(client_socket, str, strlen(str)+1);
                if(!strcmp(str,"exit")) break;
                read(client_socket,buff,BUFF_SIZE);
                printf("%s\n",buff);
		}
        close(client_socket);
        return 0;
}