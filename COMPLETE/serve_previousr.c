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
#include <ctype.h>
#include <time.h>
#include <sys/select.h>
#include <sys/utsname.h>

#define SIZE 20
#define NUMBER_OF_MINE (SIZE*SIZE/3)
#define MAX_BUF_SIZE SIZE*SIZE*3

void initGameArray();
void initPublicArray();
void makeGameArray(int xi,int yi);
void setPublic(int x, int y);
void initIntGameArray();
void getPointBuffer();
void Click(int x, int y);
void initGame();
void pack(int intgamearray[][SIZE], char pkg[]);
void parser(int *x, int *y, char XY[]);
bool check_invalid_xy(const char buf[]);
bool check_for_mine(const int x, const int y);
bool check_for_win(void);
bool check_xy_invalid_range(const int x, const int y);
void creat_sad(void);
void creat_happy(void);
int sendall(int client, char *buf, int *len);
void *get_in_addr(struct sockaddr *sa);
void Select(fd_set *read_fds);
void Run_server(void);
int Init_Server(void);
void execution(int argc, char *argv[]);

int fdmax;
char PORT[20];
int gamearray[SIZE][SIZE];
bool publicarray[SIZE][SIZE];
int pointbuffer[MAX_BUF_SIZE][2];     // pointbuffer[][0] : x      pointbuffer[][1] : y
int top_buffer = 0;
int intgamearray[SIZE][SIZE]; // game data
char pkg[MAX_BUF_SIZE];

/*********************SM Algorithm****************************/
void initGameArray(){
	int i,j;
	for(i = 0 ; i < SIZE ;i++){
		for(j = 0 ; j < SIZE ; j++){
			gamearray[i][j]=0;
		}
	}
}

void initPublicArray(){
	int i,j;
	for(i = 0 ; i < SIZE ;i++){
		for(j = 0 ; j < SIZE ; j++){
			publicarray[i][j]=false;
		}
	}
}

void initPointBuffer(){
	int i;
	for(i=0;i<MAX_BUF_SIZE;i++){
		pointbuffer[i][0]=0;
		pointbuffer[i][1]=0;
	}
}

void initIntGameArray(){
	int i, j;
	for(i = 0 ; i < SIZE ; i++){
		for(j = 0 ; j < SIZE ; j++){
			intgamearray[i][j] = 10;
		}
	}
}
void setPointBuffer(int x, int y){
	pointbuffer[top_buffer][0]=x;
	pointbuffer[top_buffer][1]=y;
	top_buffer++;
}

void setPublic(int x, int y){
	if(publicarray[x][y] == true) return;
        publicarray[x][y] = true;
	setPointBuffer(x,y);
	if(gamearray[x][y] == 0){
                if(x-1>=0 && y-1>=0)
                        setPublic(x-1,y-1);
                if(x+1<SIZE && y-1>=0)
			setPublic(x+1,y-1);
		if(x+1<SIZE && y+1<SIZE)
			setPublic(x+1,y+1);
		if(x-1>=0 && y+1<SIZE)
			setPublic(x-1,y+1);
                if(x-1>=0)
                        setPublic(x-1,y);
                if(y-1>=0)
                        setPublic(x,y-1);
                if(x+1<SIZE)
                        setPublic(x+1,y);
                if(y+1<SIZE)
                        setPublic(x,y+1);
        }
        
}
void getPointBuffer(){
	int i;
	for(i = 0 ; i < top_buffer ; i++){
		int x = pointbuffer[i][0];
		int y = pointbuffer[i][1];
		intgamearray[x][y] = gamearray[x][y];
	}
}
void makeGameArray(int xi, int yi){
	int i=0;
	int j;
	int m,n;
	int minecount;
	int x,y;
	while(i<NUMBER_OF_MINE){
		x = rand()%SIZE;
		y = rand()%SIZE;
		if(xi != x && yi != y && gamearray[x][y]==0){
			gamearray[x][y] = 9;
			i++;
		}
	}

	for(i = 0 ; i < SIZE;i++){
		for(j = 0 ; j < SIZE ; j++){
			if(gamearray[i][j] != 9){
				minecount = 0;
				for(m = i-1 ; m <= i+1 ; m++){
					for(n = j-1 ; n <= j+1 ; n++){
						if(m>=0 && m<SIZE && n>=0 && n<SIZE){
							if(gamearray[m][n] == 9){
								minecount++;
							}
						}
					}
				}
				gamearray[i][j] = minecount;
			}
		}
	}
}
void initGame(){
	srand((unsigned)time(0));
	top_buffer = 0;
	initPublicArray();
	initIntGameArray();
	initGameArray();
	pack(intgamearray, pkg);
}

void Click(int x, int y){
	if(top_buffer == 0){
		makeGameArray(x,y);
	}
	setPublic(x,y);
	getPointBuffer();
	pack(intgamearray, pkg);
}

/*********************Tools****************************/
void parser(int *x, int *y, char XY[]) // coordinate paser
{
    char *token;
    *x = atoi(strtok(XY, ","));
    *y = atoi(strtok(NULL, ","));
}

void pack(int intgamearray[][SIZE], char pkg[])
{
    bzero(pkg, MAX_BUF_SIZE);
    char temp[10];
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(i == SIZE-1 && j == SIZE-1){
                sprintf(temp, "%d", intgamearray[i][j]);
            }else{
                sprintf(temp, "%d,", intgamearray[i][j]);

            }
            strcat(pkg, temp);
        }
    }
}
bool check_invalid_xy(const char buf[])
{
    int length;
    int comma_num = 0;
    if((length = strlen(buf)) > 5)
        return true;
    for(int i = 0; i < length; i++){
        if(buf[i] == ',')
            comma_num++;
    }
    if(comma_num != 1)
        return true;
    if(buf[0] == ',' || buf[length-1] == ',')
        return true;
    
    return false;
}
bool check_xy_invalid_range(const int x, const int y)
{
    if(x < 1 || x > 20 || y < 1 || y > 20)
        return true;
    else
        return false;
}
bool check_for_mine(int x, int y)
{
    if(gamearray[x-1][y-1] == 9)
        return true;
    else
        return false;
}
bool check_for_win(void)
{
    int num = 0;
    for(int i = 0; i < SIZE; i ++){
        for(int j = 0; j < SIZE; j++){
            if(publicarray[i][j] == 0)
                num++;
        }
    }
    if(num == NUMBER_OF_MINE){
        return true;
    }else{
        return false;
    }

}
int sendall(int client, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(client, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
	send(client,"\n",2,0);
	
    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 
void creat_sad(void)
{
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            intgamearray[i][j] = 12;
        }
    }

    for(int i = 3; i <= 7; i++)
    {
        intgamearray[3][i] = 11;
        intgamearray[4][i] = 11;
        intgamearray[5][i] = 11;
        intgamearray[6][i] = 11;
    }
    for(int i = 12; i <= 16; i++)
    {
        intgamearray[3][i] = 11;
        intgamearray[4][i] = 11;
        intgamearray[5][i] = 11;
        intgamearray[6][i] = 11;
    }

    intgamearray[14][7] = 11, intgamearray[14][8] = 11, intgamearray[14][9] = 11, intgamearray[14][10] = 11, intgamearray[14][11] = 11, intgamearray[14][12] = 11;
    intgamearray[15][13] = 11, intgamearray[15][6] = 11, intgamearray[16][5] = 11, intgamearray[16][14] = 11;
    intgamearray[17][4] = 11, intgamearray[17][15] = 11;


    pack(intgamearray, pkg);
}
void creat_happy(void)
{
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            intgamearray[i][j] = 12;
        }
    }
    for(int i = 3; i <= 7; i++)
    {
        intgamearray[3][i] = 11;
        intgamearray[4][i] = 11;
        intgamearray[5][i] = 11;
        intgamearray[6][i] = 11;
    }
    for(int i = 12; i <= 16; i++)
    {
        intgamearray[3][i] = 11;
        intgamearray[4][i] = 11;
        intgamearray[5][i] = 11;
        intgamearray[6][i] = 11;
    }

    intgamearray[18][7] = 11, intgamearray[18][8] = 11, intgamearray[18][9] = 11, intgamearray[18][10] = 11, intgamearray[18][11] = 11, intgamearray[18][12] = 11;
    intgamearray[15][4] = 11, intgamearray[15][15] = 11, intgamearray[16][5] = 11, intgamearray[16][14] = 11;
    intgamearray[17][6] = 11, intgamearray[17][15] = 11;
    
    pack(intgamearray, pkg);
}

/*********************Server****************************/
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
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
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
    printf("==Server is ready to go==\n");
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
void Run_server(void)
{
    int user_num = 0;
    char user_num_string[5];
    char mines_num_string[5];
    int listener; // listener
    int newfd;    // for new connection
    int nbytes;   // received number of bytes
    char buf[20]; // receive buffer
    int x, y;
    fd_set read_fds; // read_fds
    fd_set master;
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];
    int sendsize;
    int user_num_string_size;
    int mines_num_string_size;
    time_t tm;
    char time_string[30];
    char serverinfo_string[30];
    struct utsname uts;

    FD_ZERO(&read_fds);
    FD_ZERO(&master);

    listener = Init_Server();
    FD_SET(listener, &master), fdmax = listener;  // add listener to read_fds set

    for(;;){
            if(user_num == 0){
                initGame();
            }
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
                    user_num++;
                    printf("New connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
                }
            }else{ // handle data here

                for(int i = 0; i <= fdmax; i++){
                    if(FD_ISSET(i, &read_fds)){
                        if(i == listener){
                            continue;
                        }else{
							for(int init=0;init<sizeof(buf);init++)
								buf[init]='\0';
                        if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                            // got error or connection closed by client
                            if (nbytes == 0) {
                                // connection closed
                                printf("Server: socket %d hung up\n", i);
                            } else {
                                perror("recv");
                            }
                            close(i); // connection closed or error occured
                            user_num--;
                            FD_CLR(i, &master); // remove from read set
                        }else{
							char *tmp;
                            if(strlen(buf) > 7 || strlen(buf) < 3){
                                    continue;
                            }
                            //printf("Received : %lu, %s\n",strlen(buf),buf); //CHECK
							tmp = strtok(buf,"#");
							//printf("Tokened : %lu, %s\n",strlen(tmp),tmp);
							

							if(!strcmp(tmp, "refresh")){
                                sendsize = sizeof(pkg);
                                sendall(i, pkg, &sendsize);
                            }else if(!strcmp(tmp, "query")){
                                    sprintf(user_num_string, "%d", user_num);
                                    user_num_string_size = sizeof(user_num_string);
                                    send(i, user_num_string, user_num_string_size, 0);
									send(i,"\n",2,0);
                            }else if(!strcmp(tmp, "mines")){
                                    sprintf(mines_num_string, "%d", NUMBER_OF_MINE);
                                    mines_num_string_size = sizeof(mines_num_string);
                                    send(i, mines_num_string, mines_num_string_size, 0);
									send(i,"\n",2,0);
                            }else if(!strcmp(tmp, "time")){
                                    time(&tm);
                                    strcpy(time_string,ctime(&tm));
                                    send(i, time_string, sizeof(time_string), 0);
									send(i,"\n",2,0);
                            }else if(!strcmp(tmp, "server")){
                                    uname(&uts);
                                    sprintf(serverinfo_string, "%s:[%s]", uts.sysname, uts.nodename);
                                    send(i, serverinfo_string, sizeof(serverinfo_string), 0);
									send(i,"\n",2,0);
                            }else{
                                if(check_invalid_xy(tmp)){
                                    continue;
                                }
								printf("Received : %lu, %s\n",strlen(buf),buf); //CHECK
                                parser(&x, &y, tmp);
                                if(check_xy_invalid_range(x,y)){
                                    continue;
                                }
                                if(check_for_mine(x,y)){
                                    creat_sad();
                                    // close(i); 
                                    // user_num--;
                                    // FD_CLR(i, &master); 		
									
                                    for(int j = 0; j <= fdmax; j++) {
                                    if (FD_ISSET(j, &master)) {
                                    if (j != listener) { // && i!=j
                                        sendall(j, pkg, &sendsize);
                                        }
                                    }
                                }
                                    sleep(3);
                                    initGame();
                                }else if(check_for_win()){
                                    creat_happy();
                                    for(int j = 0; j <= fdmax; j++) {
                                    if (FD_ISSET(j, &master)) {
                                    if (j != listener && j != i) {
                                        sendall(j, pkg, &sendsize);
                                        }
                                    }
                                }

                                }else{
                                    Click(x-1, y-1);
                                }
                                
                            }
                        }
                    } 
                }
            }
        }
    }
}

void execution(int argc, char *argv[])
{
    char buf1[20];
    pid_t pid;
    int left,ports;
    
    if (argc==1){
        left = 4; // we make 4 severs
        ports = 9000; // first port.
        while(left>=2){
            pid=fork();
            if(pid==-1){
                perror("fork()");
                exit(1);
            }
            else if(pid==0){
                sprintf(buf1,"%d",ports);
                execlp(argv[0],argv[0],buf1,(char *) NULL);
                exit(1);
            }
            else{
                ports++;
            }
            left--;
        }
    }
    else if(argc==2){
        ports = atoi(argv[1]);    
    }
    printf("Opened Server Port: %d\n",ports);
    sprintf(PORT,"%d",ports);
}

int main(int argc, char *argv[])
{
    execution(argc,argv);
    Run_server();
	
    return 0;
}
