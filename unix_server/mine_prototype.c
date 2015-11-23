#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

#include "server.h"
#include "mine.h"

struct minesweeper games[MAX_ROOM];

void initMineArray(int room_n){ // Merge initGameArray() and initPublicArray(). 
	int i,j;
	for(i = 0 ; i < SIZE_X ;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			games[room_n].publicarray[i][j]=false;
			games[room_n].gamearray[i][j]=0;
		}
	}
}
void initPointBuffer(int room_n){
	int i;
	for(i=0;i<MAX_BUF_SIZE;i++){
		games[room_n].pointbuffer[i][0]=0;
		games[room_n].pointbuffer[i][1]=0;
	}
	games[room_n].point_top_buffer=0;
	games[room_n].point_sended_buffer=-1;
}
void initGames(int room_n){
	initMineArray(room_n);
	initPointBuffer(room_n);

}

void printGameArray(int room_n){
	int i,j;
	for(i = 0 ; i<SIZE_X;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			if(games[room_n].publicarray[i][j]){
				printf("%d ",games[room_n].gamearray[i][j]);
			}
			else{
				printf("* ");
			}
		}
		printf("\n");
	}
	printPointBuffer(room_n);
	printf("\n");
}

void printPointBuffer(int room_n){
	int i;
	for(i=0;i<games[room_n].point_top_buffer;i++){
		printf("(%d,%d), ",games[room_n].pointbuffer[i][0],games[room_n].pointbuffer[i][1]);
		if(i%5==1) printf("\n");
	}
	printf("\n");
}

void setPointBuffer(int room_n,int x, int y){
	games[room_n].pointbuffer[games[room_n].point_top_buffer][0]=x;
	games[room_n].pointbuffer[games[room_n].point_top_buffer][1]=y;
	games[room_n].point_top_buffer++;
}

void setPublic(int room_n,int x, int y){
	char buf[6];
	
	if(games[room_n].publicarray[x][y] == true) return;
	games[room_n].publicarray[x][y] = true;
	setPointBuffer(room_n,x,y);	
	
	if(games[room_n].gamearray[x][y] == 0){
		//check exist 0 near the point
		if(x-1>=0 && y-1>=0)
			setPublic(room_n,x-1,y-1);
		if(x+1<SIZE_X && y-1>=0)
			setPublic(room_n,x+1,y-1);
		if(x+1<SIZE_X && y+1<SIZE_Y)
			setPublic(room_n,x+1,y+1);
		if(x-1>=0 && y+1<SIZE_Y)
			setPublic(room_n,x-1,y+1);
		if(x-1>=0)
			setPublic(room_n,x-1,y);
		if(y-1>=0)
			setPublic(room_n,x,y-1);
		if(x+1<SIZE_X)
			setPublic(room_n,x+1,y);
		if(y+1<SIZE_Y)
			setPublic(room_n,x,y+1);
	}
        
}


void makeGameArray(int room_n,int xi, int yi){
	int i=0;
	int j;
	int m,n;
	int minecount;
	int x,y;
	while(i<NUMBER_OF_MINE){
		//Set to mine. randomly!
		x = rand()%SIZE_X;
		y = rand()%SIZE_Y;
		if(xi != x && yi != y && games[room_n].gamearray[x][y]==0){
			games[room_n].gamearray[x][y] = 9;
			i++;
		}
	}

	for(i = 0 ; i < SIZE_X;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			if(games[room_n].gamearray[i][j] != 9){
				minecount = 0;
				for(m = i-1 ; m <= i+1 ; m++){
					for(n = j-1 ; n <= j+1 ; n++){
						if(m>=0 && m<SIZE_X && n>=0 && n<SIZE_Y){
							if(games[room_n].gamearray[m][n] == 9){
								minecount++;
							}
						}
					}
				}
				games[room_n].gamearray[i][j] = minecount;
			}
		}
	}
}

void startGame(int room_n,int x, int y){
	srand((unsigned)time(0));
	makeGameArray(room_n,x,y);
}

/*
int main(){     // only use test. main() is in server
	startGame();
	
	while(1){
		scanf("%d %d",&x,&y);
		setPublic(x,y);
		printGameArray();
	}
	
	return 0;
}
*/

