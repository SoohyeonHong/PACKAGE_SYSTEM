#include <stdio.h>
#include <stdlib.h>
#include <string.h>
	#define MAX_MSG_SIZE			100
#define PASSWD_LEN				4
#define N_BUILDING				10
#define N_FLOOR					10
#define N_ROOM					10

#define STORAGE_FILEPATH 		"storage.txt"
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;

void main(void){
		//row/column of the delivery system
	char masterPassword[5];
	int systemSize[2]= {0.0};
	
static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//row/column of the delivery system


	FILE *fp = NULL;
	fp = fopen(STORAGE_FILEPATH,"r");
		if( fp == NULL )
		{
			printf("There's no file");
		}
	fscanf(fp, "%d %d %4s", &systemSize[0], &systemSize[1], masterPassword);
	
	printf("%d %d %s\n" ,systemSize[0],systemSize[1], masterPassword);
	
	int i;
	int j;
	deliverySystem = (storage_t**)malloc(sizeof(storage_t*)*systemSize[0]);  
		for(i=0;i<systemSize[0];i++)
			deliverySystem[i] = (storage_t*)malloc(sizeof(storage_t)*systemSize[1]);
	
	int x;
	int y;
	char c;
	
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].cnt = 0;	
		}
		
	}	
	
	while((c=fgetc(fp))!=EOF)
	{
		
	
		fscanf(fp, "%d %d", &x, &y);
		
		fscanf(fp, "%d %d", &deliverySystem[x][y].building, &deliverySystem[x][y].room);
		
	printf("delivery[%d][%d].building : %d , delivery[%d][%d].room : %d \n\n",x,y, deliverySystem[x][y].building,x, y , deliverySystem[x][y].room);

	
	deliverySystem[x][y].context = (char*)malloc(100*sizeof(char));
		fscanf(fp, "%s %s", deliverySystem[x][y].passwd, deliverySystem[x][y].context);
		printf("delivery[%d][%d].passwd : %s , delivery[%d][%d].context : %s \n\n",x,y, deliverySystem[x][y].passwd,x, y , deliverySystem[x][y].context);
	free(deliverySystem[x][y].context);
		storedCnt++;
		
		deliverySystem[x][y].cnt = 1;	
	printf("stored Cnt4 : %d\n",storedCnt);
		
	}
	
	
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			printf("cnt : %d \n",deliverySystem[i][j].cnt);
		}
		
	}	
	
	
	
	
	fclose(fp);
	free(deliverySystem);
}
