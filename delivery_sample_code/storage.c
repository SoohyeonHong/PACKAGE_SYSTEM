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


static storage_t** deliverySystem; 		//deliverySystem
static int storedCnt = 0;			//number of cells occupied
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
	char init[PASSWD_LEN+1] = {0,};				// 복사해줄 초기화할 값 구성 
	
	deliverySystem[x][y].building 	= 0;			// 값에는 그대로 0 대입 
	deliverySystem[x][y].room 		= 0;		// 여기도 0 대입 
	strcpy(deliverySystem[x][y].passwd, init);		// 배열에는 0을 복사해줘서 배열에다 0으로 초기화 
	deliverySystem[x][y].context 	= 0;			// 내용에도 0으로 초기화 
	deliverySystem[x][y].cnt 		= 0;		// 사물함이 비워져있단 의미  
	
	free(deliverySystem[x][y].context) ;			// 메모리 leakage를 막기 위해 해당 사물함 내용의 메모리할당을 해제해줌 
	
	x='\0';							//x와 y도 초기화해줌 
	y='\0';
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	
	char input_passwd[PASSWD_LEN+1]; 			//passwd를 입력받을 변수 
	int correct ;						//passwd가 같은지 여부를 반환받을 변수 
	int master ;						//passwd가 masterkey와 같은지 여부를 반환받을 변수 
	
	printf(" - input password for (%d, %d) storage : ", x, y);
	scanf("%s", input_passwd);				//passwd 입력받음  

	
	correct = strcmp( input_passwd, deliverySystem[x][y].passwd );  		// 저장된 passwd가 입력받은 passwd와 같으면 1, 다르면 0을 반환 
	master  = strcmp( input_passwd, masterPassword );				// 마스터 passwd가 입력받은 passwd와 같으면 1, 다르면 0을 반환 
	
	if (correct == 0 || master == 0)	
	{
		printf(" -----------> extracting the storage (%d, %d)...", x, y);	//비밀번호가 맞으니 물건을 비워줌  
		return 0;
	}
	
	if (correct == 1 || master == 1)	
	{
		printf(" -----------> password is wrong!!");				//비밀번호 틀리면 물건 안 돌려줌  
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
	
	FILE *fp = NULL;						//파일을 읽기 모드로 불러와서 처음부터 싹 갈아엎음
	fp = fopen(filepath,"w");
		if( fp == NULL )
		{
			printf("There's no file");			//파일을 못찾으면 -1값을 반환함 
			return -1 ;
		}	
		
	fprintf(fp,"%d %d\n", systemSize[0], systemSize[1]);		//파일의 첫번째 줄에는 사물함의 행과 열 사이즈를 입력  
	fprintf(fp,"%s\n",masterPassword);				//파일의 두번째 줄에 마스터 비밀번호 값을 넣어줌 
	
	
	//cnt가 1로 설정된, 즉 사물함에 물건이 들어있는 애들만 선택하여 for문을 돌려줘서 한 줄씩 그 사물함의 좌표와 구조체 정보를 넣어줌  
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
	
	FILE *fp = NULL;									//포인터로 파일을 받음 
	fp = fopen(filepath,"r");								//읽기모드로 열음 
	
	if( fp == NULL )
	{
		printf("There's no file");														//파일이 안잡히면 -1값을 반환함  
		return -1 ;
	}
			
	fscanf(fp, "%d %d %s", &systemSize[0], &systemSize[1], masterPassword);				//filepath에 해당하는 파일에서 차례대로 사물함 개수와 마스터 비밀번호를 받아옴 
		
	deliverySystem = (storage_t**)malloc(sizeof(storage_t*)*systemSize[0]);  			//택배보관함을 가리키는 이중포인터인 deliverySystem에 메모리를 할당해줌 
		for(i=0;i<systemSize[0];i++)													//2차원 배열인 이중포인터에 메모리를 할당
			deliverySystem[i] = (storage_t*)malloc(sizeof(storage_t)*systemSize[1]);
	
	int x;
	int y;
	char c;			// 읽어올 파일이 끝나는 부분을 찾아낼 변수 
	
	for(i=0;i<systemSize[0];i++)					//각 사물함의 cnt값을 0으로 초기화함 
	{
		for(j=0;j<systemSize[1];j++)
		{
			deliverySystem[i][j].cnt = 0;			//이렇게 안했더니 쓰레기값 잡혀서 0으로 초기화함 
		}		
	}	
	
	while((c=fgetc(fp))!=EOF)
	{
		fscanf(fp, "%d %d", &x, &y);	 																// 첫 두 숫자를 각자 행, 렬로 받아옴  
		deliverySystem[x][y].context = (char*)malloc(100*sizeof(char));									//*cotext에 메모리 할당해줌  

		fscanf(fp, "%d %d", &deliverySystem[x][y].building, &deliverySystem[x][y].room);  				// 그 행 렬 에 해당하는 구조체에 빌딩과 방 정보를 저장  
		fscanf(fp, "%4s %s", deliverySystem[x][y].passwd, deliverySystem[x][y].context); 				// 패스워드와 내용물도 저장 
		
		deliverySystem[x][y].cnt = 1;																	//이제 이 칸에는 물건이 채워졌단 의미 
		
		storedCnt++;																					//c가 EOF를 받아올 때까지 반복, 한 줄이 지나면 저장된 개수를 증가시킴 
	}
	fclose(fp);

	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	
	int i;
	int j;
	
	for(i=0;i<systemSize[0];i++)					//모든 사물함의 context 메모리 할당을 해제하여 leakage를 막음   
	{
		for(j=0;j<systemSize[1];j++)
		{
			free(deliverySystem[i][j].context);		//메모리 해제 
		}		
	}	
	
	free(deliverySystem);						//가장 마지막에 포인터에 할당왼 메모리를 해제하여 메모리 leakage를 막는다 
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
	
	deliverySystem[x][y].building = nBuilding;				//빌딩 값을 와플로 만들어 저장 
	
	deliverySystem[x][y].room = nRoom;					//방 번호도 와플로 만들어 저장 
	
	deliverySystem[x][y].context = (char*)malloc(sizeof(msg));		//*cotext에 메모리 할당해줌  근데 100자가 다 들어오지 않았을 때 100을 할당하는 건 메모리 관리에 좋지 않으므로  메시지로 받은 크기대로 메모리를 할당해줌 
	
	strcpy(deliverySystem[x][y].context, msg);				//메시지가 100이상이면 100까지만 잘라서 저장됨

	strcpy(deliverySystem[x][y].passwd, passwd);				//passwd가 4개보다 많이 들어오면 앞에 4자리만 잘라서 passw로 저장됨 

	deliverySystem[x][y].cnt = 1;						//이 칸엔 택배가 차있다는 표시  
	
					

	return 0;				
	
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	
	if ( inputPasswd(x,y) == 0 )					//inputpasswd 함수로 passwd를 입력받은 후 비밀번호가 맞으면 
	{			
		printStorageInside(x,y);				//그 내용을 보여준 후  
		initStorage(x,y);					//사물함 내용 비움 
		return 0; 						//성공했으니 0값을 반환 
	}	
	else if( inputPasswd(x,y) == -1 )				//비밀 번호 틀리면 실패했으므로 -1를 반환 
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
			if (deliverySystem[i][j].building == nBuilding && deliverySystem[i][j].room == nRoom) 	// for문을 돌려 해당하는 건물과 호수가 맞는지 모든 사물함을 확인 
			{
				printf(" -----------> Found a package in (%d, %d)\n",i,j);							// 일치하면 그 사물함을 찾으라고 좌표 보여줌  
				cnt++;										// 택배 개수를 count함										// 그 집에 몇 개 택배 왔는지 확인 
			}	
		}	
	}	
	return cnt;
}
