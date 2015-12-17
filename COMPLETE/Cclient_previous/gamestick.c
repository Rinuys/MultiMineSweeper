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
#include <fcntl.h>

#define SERVER1 "9000" // the port client will be connecting to 
#define SERVER2 "9001"
#define SERVER3 "9002"
#define SERVER4 "9003"
#define ADDRESS "lochuan.iptime.org"

#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>	/* POSIX flags */
#include <time.h>	/* clock_gettime(), time() */
#include <sys/time.h>	/* gethrtime(), gettimeofday() */

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#else
#error "Unable to define getRealTime( ) for an unknown OS."
#endif

double getRealTime( )
{
#if defined(_WIN32)
	FILETIME tm;
	ULONGLONG t;
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
	/* Windows 8, Windows Server 2012 and later. ---------------- */
	GetSystemTimePreciseAsFileTime( &tm );
#else
	/* Windows 2000 and later. ---------------------------------- */
	GetSystemTimeAsFileTime( &tm );
#endif
	t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
	return (double)t / 10000000.0;

#elif (defined(__hpux) || defined(hpux)) || ((defined(__sun__) || defined(__sun) || defined(sun)) && (defined(__SVR4) || defined(__svr4__)))
	/* HP-UX, Solaris. ------------------------------------------ */
	return (double)gethrtime( ) / 1000000000.0;

#elif defined(__MACH__) && defined(__APPLE__)
	/* OSX. ----------------------------------------------------- */
	static double timeConvert = 0.0;
	if ( timeConvert == 0.0 )
	{
		mach_timebase_info_data_t timeBase;
		(void)mach_timebase_info( &timeBase );
		timeConvert = (double)timeBase.numer /
			(double)timeBase.denom /
			1000000000.0;
	}
	return (double)mach_absolute_time( ) * timeConvert;

#elif defined(_POSIX_VERSION)
	/* POSIX. --------------------------------------------------- */
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
	{
		struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
		/* BSD. --------------------------------------------- */
		const clockid_t id = CLOCK_MONOTONIC_PRECISE;
#elif defined(CLOCK_MONOTONIC_RAW)
		/* Linux. ------------------------------------------- */
		const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_HIGHRES)
		/* Solaris. ----------------------------------------- */
		const clockid_t id = CLOCK_HIGHRES;
#elif defined(CLOCK_MONOTONIC)
		/* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
		const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
		/* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
		const clockid_t id = CLOCK_REALTIME;
#else
		const clockid_t id = (clockid_t)-1;	/* Unknown. */
#endif /* CLOCK_* */
		if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
			return (double)ts.tv_sec +
				(double)ts.tv_nsec / 1000000000.0;
		/* Fall thru. */
	}
#endif /* _POSIX_TIMERS */

	/* AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. ----- */
	struct timeval tm;
	gettimeofday( &tm, NULL );
	return (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#else
	return -1.0;		/* Failed. */
#endif
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

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 
int check_server(char *port, double *response)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int room_player = 0;
    char buf[5];
    double start, end;

    start = getRealTime();
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ADDRESS, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if(send(sockfd, "query", 10, 0) < 0){
        perror("send");
    }
    if(recv(sockfd, buf, sizeof buf, 0) < 0){
        perror("recv");
    }
    end = getRealTime();
    *response = end - start;
    room_player = atoi(buf);
    close(sockfd);
    return (int)room_player/2;
}
void connect_server(char *port)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char xy[10];
    int xy_size;
    char recvbuf[50];
    struct timeval tv;
    fd_set read_fds;
    fd_set master;
    tv.tv_sec = 2;
    tv.tv_usec = 1000000;

    FD_ZERO(&read_fds);
    FD_ZERO(&master);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ADDRESS, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        exit(1);
    }
    FD_SET(sockfd, &master);
    freeaddrinfo(servinfo); // all done with this structure

    for( ; ; ){  
        read_fds = master;
        if(select(sockfd+1, &read_fds, NULL, NULL, &tv)){
            recv(sockfd, recvbuf, sizeof(recvbuf), 0);
            printf("Message From Server: %s\n\n", recvbuf);
            tv.tv_sec = 0;
            tv.tv_usec = 200000;
        }else{
            printf("\n");
            tv.tv_sec = 0;
            tv.tv_usec = 200000;
        }  
        printf("Input the coordinate(e.g: 2,3)\n");
        scanf("%s", xy);
        xy_size = sizeof(xy);
        sendall(sockfd, xy, &xy_size);      
    }

}
void print_room_status(int room1, int room2, int room3, int room4, double ping1, double ping2, double ping3, double ping4)
{
    char *good = "Status: Good";
    char *normal = "Status: Normal";
    char *congestion = "Status: Congestion";
    char *closed = "Server closed";

    if(room1 < 0)
        printf("Room1: "), puts(closed);
    if(room1 >= 0 && room1 <= 4)
        printf("Room1: %3d players - Ping: %fs ", room1, ping1),puts(good);
    if(room1 > 4 && room1 <=10)
        printf("Room1: %3d players - Ping: %fs ", room1, ping1),puts(normal);
    if(room1 > 10)
        printf("Room1: %3d players - Ping: %fs ", room1, ping1),puts(congestion);

    if(room2 < 0)
        printf("Room2: "),puts(closed);
    if(room2 >= 0 && room2 <= 4)
        printf("Room2: %3d players - Ping: %fs ", room2, ping2),puts(good);
    if(room2 > 4 && room2 <=10)
        printf("Room2: %3d players - Ping: %fs ", room2, ping2),puts(normal);
    if(room2 > 10)
        printf("Room2: %3d players - Ping: %fs ", room2, ping2),puts(congestion);


    if(room3 < 0)
        printf("Room3: "),puts(closed);
    if(room3 >= 0 && room3 <= 4)
        printf("Room3: %3d players - Ping: %fs ", room3, ping3),puts(good);
    if(room3 > 4 && room3 <=10)
        printf("Room3: %3d players - Ping: %fs ", room3, ping3),puts(normal);
    if(room4 > 10)
        printf("Room3: %3d players - Ping: %fs ", room3, ping3),puts(congestion);


    if(room4 < 0)
        printf("Room4: "),puts(closed);
    if(room4 >= 0 && room4 <= 4)
        printf("Room4: %3d players - Ping: %fs ", room4, ping4),puts(good);
    if(room4 > 4 && room4 <=10)
        printf("Room4: %3d players - Ping: %fs ", room4, ping4),puts(normal);
    if(room4 > 10)
        printf("Room4: %3d players - Ping: %fs ", room4, ping4),puts(congestion);
    printf("----------------------------------------------------\n");


    if(room1 < 0 && room2 < 0 && room3 < 0 && room4 < 0){
        sleep(2);
        exit(1);
    }
}
void print_icon(void)
{
    puts("MMMMMMMM               MMMMMMMM        SSSSSSSSSSSSSSS ");
    puts("M:::::::M             M:::::::M      SS:::::::::::::::S");
    puts("M::::::::M           M::::::::M     S:::::SSSSSS::::::S");
    puts("M:::::::::M         M:::::::::M     S:::::S     SSSSSSS");
    puts("M::::::::::M       M::::::::::M     S:::::S            ");
    puts("M:::::::::::M     M:::::::::::M     S:::::S            ");
    puts("M:::::::M::::M   M::::M:::::::M      S::::SSSS         ");
    puts("M::::::M M::::M M::::M M::::::M       SS::::::SSSSS    ");
    puts("M::::::M  M::::M::::M  M::::::M         SSS::::::::SS  ");
    puts("M::::::M   M:::::::M   M::::::M            SSSSSS::::S ");
    puts("M::::::M    M:::::M    M::::::M                 S:::::S");
    puts("M::::::M     MMMMM     M::::::M                 S:::::S");
    puts("M::::::M               M::::::M     SSSSSSS     S:::::S");
    puts("M::::::M               M::::::M     S::::::SSSSSS:::::S");
    puts("M::::::M               M::::::Mine   S:::::::::::::::SSeeper Online");
    puts("                                                                   ");
    puts("###################################################################");
    puts("##########Unix Term Project############        Team 8       #######");
    puts("#######################################     Hongseok Lee    #######");
    puts("#######################################     Jeonghan Lee    #######");
    puts("#######################################     Junghan Choi    #######");
    puts("#######################################     Zhengwen Zhang  #######");
    puts("###################################################################");


}
int main(void)
{
    int room1 = 0, room2 = 0, room3 = 0, room4 = 0;
    char op;
    double room1_ping, room2_ping, room3_ping, room4_ping;

    room1 = check_server(SERVER1, &room1_ping);
    room2 = check_server(SERVER2, &room2_ping);
    room3 = check_server(SERVER3, &room3_ping);
    room4 = check_server(SERVER4, &room4_ping);

    print_icon();
    printf("Enter to continue\n");
    getchar();
    printf("Checking Server...\n");
    sleep(1);
    system("clear");
    printf("----------------------Room List---------------------\n");
    print_room_status(room1, room2, room3, room4, room1_ping, room2_ping, room3_ping, room4_ping);
    printf("\n\n");
    puts("+------------------------------+");
    puts("| Select a room for great fun  |");
    puts("+--------------+---------------+");
    puts("|   1.Room1    |    2.Room2    |");
    puts("+------------------------------+");
    puts("|   3.Room3    |    4.Room4    |");
    puts("+--------------+---------------+");
    puts("|           q.Exit             |");
    puts("+------------------------------+");
    printf("\n\n");
    printf("Input the room number:");
    scanf("%c", &op);
    if((int)op < 49 || (int)op > 52){
        printf("Input error\n");
        exit(1);
    }
    system("clear");
    printf("\n\n");
    if(op == '1'){
        printf("===Open another terminal window, run ./monitor %c===\n\n", op);
        printf("===Surported Command===\n");
        printf("===query ===\n");
        printf("===server===\n");
        printf("===mines ===\n");
        printf("===time  ===\n");
        printf("query: check # of players\nmines: check # of mines\ntime: check for current time\nserver: check serverinfo");
        connect_server(SERVER1);
    }else if(op == '2'){
        printf("===Open another terminal window, run ./monitor %c===\n\n", op);
        printf("===Surported Command===\n");
        printf("===query ===\n");
        printf("===server===\n");
        printf("===mines ===\n");
        printf("===time  ===\n");
        printf("query: check # of players\nmines: check # of mines\ntime: check for current time\nserver: check serverinfo");
        connect_server(SERVER2);
    }else if(op == '3'){
        printf("===Open another terminal window, run ./monitor %c===\n\n", op);
        printf("===Surported Command===\n");
        printf("===query ===\n");
        printf("===server===\n");
        printf("===mines ===\n");
        printf("===time  ===\n");
        printf("query: check # of players\nmines: check # of mines\ntime: check for current time\nserver: check serverinfo");
        connect_server(SERVER3);
    }else if(op == '4'){
        printf("===Open another terminal window, run ./monitor %c===\n\n", op);
        printf("===Surported Command===\n");
        printf("===query ===\n");
        printf("===server===\n");
        printf("===mines ===\n");
        printf("===time  ===\n");
        printf("query: check # of players\nmines: check # of mines\ntime: check for current time\nserver: check serverinfo");
        connect_server(SERVER4);
    }else{
        exit(1);
    }

    return 0;
}
