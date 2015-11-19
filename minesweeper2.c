#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

#define SIZE_X 20
#define SIZE_Y 20
#define NUMBER_OF_MINE (SIZE_X*SIZE_Y/4)
#define MAX_BUF_SIZE SIZE_X*SIZE_Y

void initGameArray();
void initPublicArray();
void makeGameArray(int xi,int yi);
void setPublic(int x, int y);
void printArray();
void setPointBuffer(int x, int y);
void printPointBuffer();

int gamearray[SIZE_X][SIZE_Y];
bool publicarray[SIZE_X][SIZE_Y];
int pointbuffer[MAX_BUF_SIZE][2];     // pointbuffer[][0] : x      pointbuffer[][1] : y
int top_buffer = 0;

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

void printGameArray(){
	int i,j;
	for(i = 0 ; i<SIZE_X;i++){
		for(j = 0 ; j < SIZE_Y ; j++){
			if(publicarray[i][j]){
				printf("%d ",gamearray[i][j]);
			}
			else{
				printf("* ");
			}
		}
		printf("\n");
	}
	printPointBuffer();
	printf("\n");
}

void printPointBuffer(){
	int i;
	for(i=0;i<top_buffer;i++){
		printf("(%d,%d), ",pointbuffer[i][0],pointbuffer[i][1]);
		if(i%5==1) printf("\n");
	}
	printf("\n");
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
                if(x+1<SIZE_X && y-1>=0)
			setPublic(x+1,y-1);
		if(x+1<SIZE_X && y+1<SIZE_Y)
			setPublic(x+1,y+1);
		if(x-1>=0 && y+1<SIZE_Y)
			setPublic(x-1,y+1);
                if(x-1>=0)
                        setPublic(x-1,y);
                if(y-1>=0)
                        setPublic(x,y-1);
                if(x+1<SIZE_X)
                        setPublic(x+1,y);
                if(y+1<SIZE_Y)
                        setPublic(x,y+1);
        }
        
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
}

void startGame(){
	int x,y;
	srand((unsigned)time(0));
	initPublicArray();
	scanf("%d %d",&x,&y);
	initGameArray();
	makeGameArray(x,y);
	setPublic(x,y);
	printGameArray();
	while(1){
		scanf("%d %d",&x,&y);
		setPublic(x,y);
		printGameArray();
	}
	
}


int main(){                 // 0~8 : number of mine     9 : mine
	startGame();
	return 0;
}
