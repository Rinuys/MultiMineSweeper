#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"stdbool.h"

#define SIZE_X 10
#define SIZE_Y 10
#define NUMBER_OF_MINE (SIZE_X*SIZE_Y/4)

void initGameArray();
void initPublicArray();
void makeGameArray(int xi,int yi);
void setPublic(int x, int y);

int gamearray[SIZE_X][SIZE_Y];
int publicarray[SIZE_X][SIZE_Y];

void initGameArray(){
	int i,j;
	for(i = 0 ; i < SIZE_X ;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			gamearray[i][j]=0;
		}
	}
}

void initPublicArray(){
	int i,j;
	for(i = 0 ; i < SIZE_X ;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			publicarray[i][j]=0;
		}
	}
}

void printGameArray(){
	int i,j;
	for(i = 0 ; i<SIZE_X;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			if(publicarray[i][j]){
				printf("%d ",gamearray[i][j]);
			}
		}
		printf("\n");
	}
	printf("\n");
}

void setPublic(int x, int y){
	publicarray[x][y] = true;
}

void makeGameArray(int xi, int yi){
	int i=0;
	int j;
	int m,n;
	int minecount;
	int x,y;
	while(i<NUMBER_OF_MINE){
		x = rand()%SIZE_X;
		y = rand()%SIZE_Y;
		if(xi != x && yi != y && gamearray[x][y]==0){
			gamearray[x][y] = 9;
			i++;
		}
	}

	for(i = 0 ; i < SIZE_X;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			if(gamearray[i][j] != 9){
				minecount = 0;
				for(m = i-1 ; m <= i+1 ; m++){
					for(n = j-1 ; n <= j+1 ; n++){
						if(m>=0 && m<SIZE_X && n>=0 && n<SIZE_Y){
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
	printGameArray();
}

void startGame(){
	int x,y;
	initPublicArray();
	scanf("%d %d",&x,&y);
	initGameArray();
	makeGameArray(x,y);
	setPublic(x,y);
	while(1){
		scanf("%d %d",&x,&y);
		printArray();
	}
	
}


int main(){                 // 0~8 : number of mine     9 : mine
	srand((unsigned)time(0));
	startGame();
	
	
	return 0;
}