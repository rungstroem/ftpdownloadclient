#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

const int size = 1024;

int fileWrite(char *fb, char fileName[], int bytesReceived){ 
	char filePlace[] = "/home/runge/projects/ftpdownloadclient/test2/";
	if(mkdir(filePlace, 0777) < 0){
		printf("%s", "Directory exists\n");
	} else{
		printf("%s", "Directory created\n");
	}

	strcat(filePlace, fileName);
	FILE *fp = fopen(filePlace, "wb");
	if((fwrite(fb, 1, bytesReceived, fp)) <= 0){
		fclose(fp);
		return 1;
	} else{
		fclose(fp);
		return 0;
	}
}

int main(){
	
	char receiveBuff[] = "123hej1234";
	if(fileWrite(receiveBuff, "abcd.cxml", 10) == 1){
		printf("%s", "Failed to write file\n");
	} else{
		printf("%s", "Operation succesfull\n");
	}
	
	return 0;
}
