#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *contents : package context (message string)
*/
typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y) {
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	char init[PASSWD_LEN+1] = {0,};					// �������� �ʱ�ȭ�� �� ���� 
	
	deliverySystem[x][y].building 	= 0;			// ������ �״�� 0 ���� 
	deliverySystem[x][y].room 		= 0;			// ���⵵ 0 ���� 
	strcpy(deliverySystem[x][y].passwd, init);		// �迭���� 0�� �������༭ �迭���� 0���� �ʱ�ȭ 
	deliverySystem[x][y].context 	= 0;			// ���뿡�� 0���� �ʱ�ȭ 
	deliverySystem[x][y].cnt 		= 0;			// �繰���� ������ִ� �ǹ�  
	
	free(deliverySystem[x][y].context) ;			// �޸� leakage�� ���� ���� �ش� �繰�� ������ �޸��Ҵ��� �������� 
	
	x='\0';				//x�� y�� �ʱ�ȭ���� 
	y='\0';
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
	char input_passwd[PASSWD_LEN+1]; 	//passwd�� �Է¹��� ���� 
	int correct ;						//passwd�� ������ ���θ� ��ȯ���� ���� 
	int master ;						//passwd�� masterkey�� ������ ���θ� ��ȯ���� ���� 
	
	printf(" - input password for (%d, %d) storage : ", x, y);
	scanf("%s", input_passwd);			//passwd �Է¹���  

	
	correct = strcmp( input_passwd, deliverySystem[x][y].passwd );  // ����� passwd�� �Է¹��� passwd�� ������ 1, �ٸ��� 0�� ��ȯ 
	master  = strcmp( input_passwd, masterPassword );				// ������ passwd�� �Է¹��� passwd�� ������ 1, �ٸ��� 0�� ��ȯ 
	
	if (correct == 0 || master == 0)	
	{
		printf(" -----------> extracting the storage (%d, %d)...", x, y);		//��й�ȣ�� ������ ������ �����  
		return 0;
	}
	
	if (correct == 1 || master == 1)	
	{
		printf(" -----------> password is wrong!!");							//��й�ȣ Ʋ���� ���� �� ������  
		return -1 ;
	}	
	
}






// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	
	int i;
	int j;
	
	FILE *fp = NULL;							//������ �б� ���� �ҷ��ͼ� ó������ �� ���ƾ���
	fp = fopen(filepath,"w");
		if( fp == NULL )
		{
			printf("There's no file");			//������ ��ã���� -1���� ��ȯ�� 
			return -1 ;
		}	
		
	fprintf(fp,"%d %d\n", systemSize[0], systemSize[1]);		//������ ù��° �ٿ��� �繰���� ��� �� ����� �Է�  
	fprintf(fp,"%s\n",masterPassword);							//������ �ι�° �ٿ� ������ ��й�ȣ ���� �־��� 
	
	
	//cnt�� 1�� ������, �� �繰�Կ� ������ ����ִ� �ֵ鸸 �����Ͽ� for���� �����༭ �� �پ� �� �繰���� ��ǥ�� ����ü ������ �־���  
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0 ) 	
			{	
				fprintf(fp,"%d %d %d %d %s %s\n", i, j, deliverySystem[i][j].building, deliverySystem[i][j].room, deliverySystem[i][j].passwd, deliverySystem[i][j].context );
			}	
		}		
	}	
	
	fclose(fp);
	
	return 0;
}



//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {

	int i;
	int j;
	
	FILE *fp = NULL;										//�����ͷ� ������ ���� 
	fp = fopen(filepath,"r");								//�б���� ���� 
	
	if( fp == NULL )
	{
		printf("There's no file");														//������ �������� -1���� ��ȯ��  
		return -1 ;
	}
			
	fscanf(fp, "%d %d %s", &systemSize[0], &systemSize[1], masterPassword);				//filepath�� �ش��ϴ� ���Ͽ��� ���ʴ�� �繰�� ������ ������ ��й�ȣ�� �޾ƿ� 
		
	deliverySystem = (storage_t**)malloc(sizeof(storage_t*)*systemSize[0]);  			//�ù躸������ ����Ű�� ������������ deliverySystem�� �޸𸮸� �Ҵ����� 
		for(i=0;i<systemSize[0];i++)													//2���� �迭�� ���������Ϳ� �޸𸮸� �Ҵ�
			deliverySystem[i] = (storage_t*)malloc(sizeof(storage_t)*systemSize[1]);
	
	int x;
	int y;
	char c;			// �о�� ������ ������ �κ��� ã�Ƴ� ���� 
	
	for(i=0;i<systemSize[0];i++)					//�� �繰���� cnt���� 0���� �ʱ�ȭ�� 
	{
		for(j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].cnt = 0;			//�̷��� ���ߴ��� �����Ⱚ ������ 0���� �ʱ�ȭ�� 
		}		
	}	
	
	while((c=fgetc(fp))!=EOF)
	{
		fscanf(fp, "%d %d", &x, &y);	 																// ù �� ���ڸ� ���� ��, �ķ� �޾ƿ�  
		deliverySystem[x][y].context = (char*)malloc(100*sizeof(char));									//*cotext�� �޸� �Ҵ�����  

		fscanf(fp, "%d %d", &deliverySystem[x][y].building, &deliverySystem[x][y].room);  				//�� �� �� �� �ش��ϴ� ����ü�� ������ �� ������ ����  
		fscanf(fp, "%4s %s", deliverySystem[x][y].passwd, deliverySystem[x][y].context); 				// �н������ ���빰�� ���� 
		
		deliverySystem[x][y].cnt = 1;																	//���� �� ĭ���� ������ ä������ �ǹ� 
		
		storedCnt++;																					//c�� EOF�� �޾ƿ� ������ �ݺ�, �� ���� ������ ����� ������ ������Ŵ 
	}
	fclose(fp);

	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	int i;
	int j;
	
	for(i=0;i<systemSize[0];i++)					//��� �繰���� context �޸� �Ҵ��� �����Ͽ� leakage�� ����   
	{
		for(j=0;j<systemSize[1];j++)
		{
			free(deliverySystem[i][j].context);		//�޸� ���� 
		}		
	}	
	
	free(deliverySystem);							//���� �������� �����Ϳ� �Ҵ�� �޸𸮸� �����Ͽ� �޸� leakage�� ���´� 
	return;
	
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}



//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
	if ( str_checkStorage(x,y) == -1 ) 
	{
		printf(" -----------> Storage (%d,%d) is already occupied or invalid!",x,y);	

		return -1;
	}
	
	deliverySystem[x][y].building = nBuilding;						//���� ���� ���÷� ����� ���� 
	
	deliverySystem[x][y].room = nRoom;								//�� ��ȣ�� ���÷� ����� ���� 
	
	deliverySystem[x][y].context = (char*)malloc(sizeof(msg));		//*cotext�� �޸� �Ҵ�����  �ٵ� 100�ڰ� �� ������ �ʾ��� �� 100�� �Ҵ��ϴ� �� �޸� ������ ���� �����Ƿ�  �޽����� ���� ũ���� �޸𸮸� �Ҵ����� 
	
	strcpy(deliverySystem[x][y].context, msg);						//�޽����� 100�̻��̸� 100������ �߶� �����

	strcpy(deliverySystem[x][y].passwd, passwd);					//passwd�� 4������ ���� ������ �տ� 4�ڸ��� �߶� passw�� ����� 

	deliverySystem[x][y].cnt = 1;									//�� ĭ�� �ù谡 ���ִٴ� ǥ��  
	
					

	return 0;				
	
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	if ( inputPasswd(x,y) == 0 )					//inputpasswd �Լ��� passwd�� �Է¹��� �� ��й�ȣ�� ������ 
	{			
		printStorageInside(x,y);					//�� ������ ������ ��  
		initStorage(x,y);							//�繰�� ���� ��� 
		return 0; 									//���������� 0���� ��ȯ 
	}	
	else if( inputPasswd(x,y) == -1 )				//��� ��ȣ Ʋ���� ���������Ƿ� -1�� ��ȯ 
	{
		printf("\n -----------> password is wrong!! \n -----------> Failed to extract my package!") ;
		return -1;
	}	
		
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int cnt = 0 ;
	int i;
	int j;
	for(i=0;i<systemSize[0];i++)
	{
		for(j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].building == nBuilding && deliverySystem[i][j].room == nRoom) 	// for���� ���� �ش��ϴ� �ǹ��� ȣ���� �´��� ��� �繰���� Ȯ�� 
			{
				printf(" -----------> Found a package in (%d, %d)\n",i,j);							// ��ġ�ϸ� �� �繰���� ã����� ��ǥ ������  
				cnt++;																				// �� ���� �� �� �ù� �Դ��� Ȯ�� 
			}	
		}	
	}	
	return cnt;
}
