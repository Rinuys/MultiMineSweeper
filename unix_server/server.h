#include <sys/socket.h>

#define PORT "9034"   // port we're listening on
#define MAX_ROOM 20
#define MAX_MEMBER 4
#define ROOM_MEMBER(x) (room[x][0] > 4) ? 4 : room[x][0]

int find_room(int fd); //return room index has fd
void broadcast(char* str,int strsize,int room_n,int except);

void init_room();

// find empty slot and add client. and add to read_fds. broadcast. if failed close fd
void add_client(int fd);
void bye_client(int fd,int member);
void kick_client(int fd);
void ending(int room_n);

void start_room(int fd);
void pushing(int fd, char *message);
void next_tok(int room_n); //set next token

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);
void init_server_connection();

void flow(); //Running forever
