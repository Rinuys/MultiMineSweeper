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
#include <ncurses.h>

#define MAXDATASIZE (BOARD_SIZE*BOARD_SIZE*3) // max number of bytes we can get at once 
#define BOARD_SIZE 20
#define ADDRESS "lochuan.iptime.org"

int board[BOARD_SIZE][BOARD_SIZE];
WINDOW *my_win;
WINDOW *title_win;
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

void print_board(int room_num)
{
    wattron(title_win, A_BOLD|A_BLINK); 
    mvwprintw(title_win, 0, 12, "Monitoring: Room %d", room_num);
    wrefresh(title_win);
       for(int row = 0; row < BOARD_SIZE; row++){
        
         for(int col = 0; col < BOARD_SIZE*2; col = col+2){
                if(board[row][col/2] == 10){
                    wattroff(my_win, COLOR_PAIR(3)|A_BLINK|A_BOLD);
                    mvwprintw(my_win, row+1, col+2, "*");
                }else if(board[row][col/2] == 0){
                    mvwprintw(my_win, row+1, col+2, " ");
                }else if(board[row][col/2] == 12){
                    mvwprintw(my_win, row+1, col+2, " ");
                }else if(board[row][col/2] == 11){
                    wattron(my_win, COLOR_PAIR(3)|A_BLINK|A_BOLD);
                    mvwprintw(my_win, row+1, col+2, "*");
                }else
                    mvwprintw(my_win, row+1, col+2, "%d", board[row][col/2]);            
            }
    }
    wrefresh(my_win);
}
int main(int argc, char *argv[])
{
    int window_row, window_col;
    int height = 22, width = 43;
    int title_height = 1, title_width = 43;
    int starty, startx;
    int title_starty, title_startx;
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

    initscr();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_RED, COLOR_WHITE);
    getmaxyx(stdscr, window_row, window_col);
    if(window_row < 23 || window_col < 43){
        fprintf(stderr, "Make sure your window height > 23, and width > 43!\n");
        exit(1);
    }
    starty = (window_row - height)/2;
    startx = (window_col - width)/2;
    title_starty = starty - 1;
    title_startx = startx;
    my_win = newwin(height, width, starty, startx);
    title_win = newwin(title_height, title_width, title_starty, title_startx);
    wbkgd(my_win, COLOR_PAIR(1));
    wbkgd(title_win, COLOR_PAIR(2));
    box(my_win, 0, 0);
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
		memset(buf,0,sizeof(buf));
        send(sockfd, "refresh", 10, 0);
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
            perror("recv");
            exit(1);
        }
		unpack(board, buf);
        print_board(atoi(argv[1]));
        nanosleep(&ts,NULL);
    }
    endwin();
    return 0;    

}
    

