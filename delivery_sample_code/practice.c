#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
	
	
int i;
	char input[100];
	FILE* fp;
	
	fp=fopen("sample.txt","w");
	for(i=0;i<3;i++)
	{
		printf("input a word :");
		scanf("%s", input);
		fprintf(fp,"%s\n",input);
	}

	fclose(fp);
	
	
	}
