#include "stdio.h"
#include "stdlib.h"
#include "string.h"


void parser(int *x, int *y, char *XYZ){
    int len;
    int flag = 0;
    int count1 = 0, count2 = 0;
    char temp1[10];
    char temp2[10];

    len = strlen(XYZ);
    for(int i = 0; i < len; i++)
    {
        if(flag == 0){
            temp1[count1] = XYZ[i];
            count1++;
        }else{
            temp2[count2] = XYZ[i];
            count2++;
        }
        if(XYZ[i] == ','){
            flag = 1;
        }
    }
    temp1[count1] = '\0';
    temp2[count2] = '\0';
    *x = atoi(temp1);
    *y = atoi(temp2);
}

int main(void)
{
    int x, y;
    char *XYZ = "12,32";
    char *AAA = "130,60";
    char *BBB = "110,999";

    parser(&x, &y, XYZ);
    printf("%d %d\n", x, y);
    parser(&x, &y, AAA);
    printf("%d %d\n", x, y);
    parser(&x, &y, BBB);
    printf("%d %d\n", x, y);
}
