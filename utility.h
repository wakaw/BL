#ifndef __Utilities_for_QNX__
#define __Utilities_for_QNX__


#include <stdio.h>		//printf() etc.
#include <stdlib.h>		//fopen(),fgets() etc.
#include <termios.h>		//tcischars()

void Utility_Pause(void){

	//The tcischars() function checks to see
	// how many characters are waiting to be read
	// from the given File Descriptor.
	//File Descriptor == 0 means stdin.
	while(!tcischars(0));
}

void Utility_CLS(void)
{
	//clear screen
	printf("\x1b[2J");
}

int Utility_Get_FileName(char filename[]){

	int i;
	char get_string[32];

	printf("Enter the file name to write AD data.\n");
	printf("(the program automatically assigns the extension .csv)\n");

	//read 1 line from the console
	//filename[] = {*,*,,,*,\n,\0}
	rewind(stdin);
	if(fgets(filename, sizeof(get_string)-4, stdin)==NULL){
		printf("in Utility_File_Open()\n");
		printf("NULL is returned from the fgets() function!!\n");
		return 1;
	}
	i = strlen(filename) - 1; // 4  for 'log'
	filename[i] = '.';	//overwrite the '\n'
	filename[i+1] = 'c';	//overwrite the '\0'
	filename[i+2] = 's';
	filename[i+3] = 'v';
	filename[i+4] = '\0';

/*
	filename[i+4] = '\0';
	filename[i+3] = 'v';
	filename[i+2] = 's';
	filename[i+1] = 'c';	//overwrite the '\0'
	filename[i] = '.';	//overwrite the '\n'
	filename[i - 1] = filename[i - 1];	//overwrite the '\n'
*/
	return 0;

}

void Utility_File_Close(FILE *fp){

	fclose(fp);

}

#endif
