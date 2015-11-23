#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "server.h"
#include "mine.h"

fd_set read_fds;  // temp file descriptor list for select()
fd_set write_fds;
int fdmax;        // maximum file descriptor number
int listener;     // listening socket descriptor
int newfd;        // newly accept()ed socket descriptor

int room[MAX_ROOM][MAX_MEMBER+1]; // 0 is member number, 1~4 is socket
int room_token[MAX_ROOM]; // index of member has token in a room

int main(void){
	init_room();
	init_server_connection();
	flow();
	
	return 0;
}

int find_room(int fd){
	int i,j,member;
	static int cache_fd=-1;
	static int cache_n=-1;
	static int cache_member=-1;
	
	if(cache_fd==fd && room[cache_n][cache_member]==cache_fd)
		return cache_n;
	
	for(i=0;i<MAX_ROOM;i++){
		member = ROOM_MEMBER(i);
		
		for(j=1;j<=member;j++)
			if(room[i][j]==fd){
				cache_fd = fd;
				cache_n=i;
				cache_member=i;
				return fd;
			}
	}
	fprintf(stderr,"find_room failed\n");
	return -1;
}

void broadcast(char* str,int strsize,int room_n,int except_fd){
	int i,j,member;
	member=ROOM_MEMBER(room_n);
	for(i=1;i<=member;i++){
		
		if(except_fd!=room[room_n][i] && room[room_n][i]!=-1){
			if(send(room[room_n][i],str,strsize,0)<0){
				perror("broadcast error!");
			}
		}
	}
}

void init_room(){
	int i,j;
	for(i=0;i<MAX_ROOM;i++){
		initGames(i);
		room[i][0]=0;
		room_token[i]=-1;
		for(j=1;j<=MAX_MEMBER;j++)
			room[i][j]=-1;
	}
	
}

void check_init_empty_room(int room_n){
	int i;
	for(i=1;i<=4;i++){
		if(room[room_n][i]!=-1)
			return;
	}
	room[room_n][0]=0;
	initGames(room_n);
}
void add_client(int fd){ 
	char buf[6];
	int j;
	for(j=0;j<MAX_ROOM && room[j][0]>=4;j++); // find empty slot
	
	if(j==MAX_ROOM){
		send(fd,"No___",6,0);
		close(fd);
		
	}
	else {
		/*printf("selectserver: new connection from %s on "
						"socket %d\n",
						inet_ntop(remoteaddr.ss_family,
						get_in_addr((struct sockaddr*)&remoteaddr),
						remoteIP, INET6_ADDRSTRLEN),
						newfd); // print new client */
		
		//Send room data to New client
		sprintf(buf,"%2d_%2d",j,room[j][0]);
		if(send(fd,buf,6,0) == -1){
			perror("send to new client");
			close(fd);
		}
		else{
			// binding new client to room
			room[j][ room[j][0] ] = fd; 
			room[j][0]++;
			FD_SET(fd, &read_fds); // add to read_fds set
			if (fd > fdmax) {    // keep track of the max
				fdmax = fd;
			}
			sprintf(buf,"New%2d",room[j][0]-1);
			broadcast(buf,6,j,fd);
		}
	}
}
void bye_client(int fd,int member){
	char buf[6];
	int room_n = find_room(fd);
	int i,j;
	
	if(room_n < 0)
		return;
	
	if( room[room_n][member] != fd ){
		fprintf(stderr,"wrong receive bye");
		return;
	}

	sprintf(buf,"Bye%2d",member);
	broadcast(buf,6,room_n,fd);
	
	if(room[room_n][0]!=5)
		room[room_n][0]--;
	room[room_n][member] = -1;
	
	close(fd);
	FD_CLR(fd, &read_fds);
		
	//refresh fdmax
	if(fd==fdmax){
		int max=listener;
		for(i=0;i<MAX_ROOM;i++)
			for(j=1;j<=4;j++)
				if(max<room[i][j])
					max=room[i][j];
		fdmax=max;
	}
	check_init_empty_room(room_n);	
}
void kick_client(int fd){
	char buf[6];
	int room_n = find_room(fd);
	int member;
	int i,j;
	
	if(room_n < 0)
		return;
	
	for(member=1;member<5 && room[room_n][member]!=fd;member++);
	if( member==5 ){
		fprintf(stderr,"can't find member in kick");
		return;
	}

	sprintf(buf,"Kic%2d",member);
	broadcast(buf,6,room_n,fd);
	
	room[room_n][member] = -1;
	if(room[room_n][0]!=5)
		room[room_n][0]--;
	close(fd);
	FD_CLR(fd, &read_fds);
	
	//refresh fdmax
	if(fd==fdmax){
		int max=listener;
		for(i=0;i<MAX_ROOM;i++)
			for(j=1;j<=4;j++)
				if(max<room[i][j])
					max=room[i][j];
		fdmax=max;
	}
	check_init_empty_room(room_n);
}

void ending(int room_n){
	int i,j;
	int max=listener; // used find fdmax
	broadcast("Ended",6,room_n,-1);
	
	
	room[room_n][0]=0;
	for(i=1;i<=4;i++){
		if(i==-1)
			continue;
		close(room[room_n][i]);
		FD_CLR(room[room_n][i], &read_fds);
		room[room_n][i]=-1;
	}
	initGames(room_n);
	
	//refresh fdmax
	for(i=0;i<MAX_ROOM;i++)
		for(j=1;j<=4;j++)
			if(max<room[i][j])
				max=room[i][j];
	fdmax=max;
}

void start_room(int fd){
	int room_n = find_room(fd);
	
	if(fd != room[room_n][1]){
		fprintf(stderr,"the client is not superuser but send start");
		return;
	}
	
	broadcast("Start",6,room_n,-1);
	room[room_n][0]=5; // flag this room is gaming.
	room_token[room_n]=1;
	if (send(fd,"Istok",6,0) == -1)
		perror("send");
	
	//startGame will be called at first pushing
}

void pushing(int fd, char *message){
	int room_n=find_room(fd);
	char buf[6];
	//int (*pointbuffer)[2];
	//int buffer_size;
	
	//message interpret. find position.
	int x = atoi(message);
	int y = atoi(message+3);
	int i,j;
	int *top_buffer=&games[room_n].point_top_buffer;
	int *sended_buffer=&games[room_n].point_sended_buffer;
	
	if(room[room_n][room_token[room_n]] != fd){
		fprintf(stderr,"he don't have token");
		return;
	}
	
	//if the first pushing. we need to make map.
	if(games[room_n].point_top_buffer == 0)
		startGame(room_n,x,y);
	
	broadcast( message,6,room_n,fd);
	setPublic(room_n,x,y); //Check poision
	
	sprintf(buf,"%1d",games[room_n].gamearray[x][y]);
	broadcast(buf,2,room_n,fd);
	

	for(*sended_buffer+=2;*sended_buffer<*top_buffer;*sended_buffer++){
		
		sprintf(buf,"%2d-%2d",x,y);
		broadcast(buf,6,room_n,-1);
	
		sprintf(buf,"%1d",games[room_n].gamearray[x][y]);
		broadcast(buf,2,room_n,-1);
	}
	
	
	//And if it is mine. kick him!
	if(games[room_n].gamearray[x][y]==9)
		kick_client(fd);
	
	//Check game is ended
	if(games[room_n].point_top_buffer == (SIZE_X*SIZE_Y) - NUMBER_OF_MINE){
		ending(room_n);
	}
	else
		next_tok(room_n);
}

void next_tok(int room_n){
	int i_member=room_token[room_n];
	int member=i_member;
	do{
		member++;
		if(member==5)
			member=1;
	}while(member!=i_member && room[room_n][member]!=-1 );
	
	room_token[room_n]=member;
	if(send(room[room_n][member],"Istok",6,0)!=-1)
		perror("token send error!");
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void init_server_connection(){
	struct addrinfo hints, *ai, *p;
	int yes=1;// for setsockopt() SO_REUSEADDR, below
	int rv;
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
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
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

	// listen
	if (listen(listener, 10) == -1) {
		perror("listen");
		exit(3);
	}

	// add the listener to the read_fds set
	FD_SET(listener, &read_fds);

	// keep track of the biggest file descriptor
	fdmax = listener; // so far, it's this one

}

void flow(){
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buf[100];    // buffer for client data
	int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

	int i,j;
	for(;;){
		if (select(fdmax+1, &read_fds, &write_fds, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == listener) {
					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener,
					(struct sockaddr *)&remoteaddr,
					&addrlen);
					
					if (newfd == -1) {
						perror("accept");
						continue;
					} 
					
					add_client(newfd); // bind room and broadcast
						
				}// END got new incoming connection
				else{
					//handle data from client
					if (nbytes = recv(i, buf, 6,0) <= 0){
						//got error or connection closed by client
						
						kick_client(i);
					}
					else{
						// we got some data!
						if(strcmp("Start",buf)==0)
							start_room(i);
						if(buf[0] >= '0' && buf[0] <= '9')
							pushing(i,buf);
						if(buf[0]='B') // may be Bye
							bye_client(i,atoi(buf+3));
						
					}				
				} // END handle data from client
			} // END FD_ISSET (check exist receving data) true. 
		}// END looping through file descriptors
	} //END for(;;)--and you thought it would never end!
} // END flow()
