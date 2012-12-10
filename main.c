#include <stdio.h>
#include "menu4.h"

//Functuion Prototype
void Main_Menu(void);

//Grobal Variable


int main(void){

	Main_Menu();



	return 0;
}


void Main_Menu(void){

	char menu[10];

	do{
		Utility_CLS();
		
		printf("\nMain Menu\n");
		printf("4:Force Measurement\n");
		printf("0:quit\n");

		fgets(menu, sizeof(menu), stdin);

		switch(menu[0]){
			case '0':	printf("menu 0 selected\n");
					break;
//			case '1':	printf("menu 1 selected\n");
//					Menu1_Type_A_Pos_Ctrl();
//					break;
			case '4':	printf("menu 4 selected\n");
					Menu4_6axis_ForceTorque();
					break;
			default:	printf("%c is illegal number!\n", menu);
					printf("please retry\n");
		}

	}while(menu[0] != '0');

	printf("quit menu OK\n");

}
