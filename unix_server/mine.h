#include <stdbool.h>
#define SIZE_X 20
#define SIZE_Y 20
#define NUMBER_OF_MINE (SIZE_X*SIZE_Y/4)
#define MAX_BUF_SIZE SIZE_X*SIZE_Y


struct minesweeper{
	int gamearray[SIZE_X][SIZE_Y];    // 0~8 : number of mine     9 : mine
	bool publicarray[SIZE_X][SIZE_Y];
	int pointbuffer[MAX_BUF_SIZE][2];     // pointbuffer[][0] : x      pointbuffer[][1] : y
	int point_top_buffer;
	int point_sended_buffer;	
};

extern struct minesweeper games[MAX_ROOM];

void initGames(int room_n);
//void initMineArray(int room_n); // call initGames.
//void initPointBuffer(int room_n);

void printGameArray(int room_n);
void printPointBuffer(int room_n);

void setPointBuffer(int room_n,int x, int y);
void setPublic(int room_n,int x, int y);

void makeGameArray(int room_n,int xi,int yi);
void startGame(int room_n,int x, int y);
