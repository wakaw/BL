#ifndef __MENU2_Type_C_Pos_Ctrl__
#define __MENU2_Type_C_Pos_Ctrl__

#include <stdio.h>
#include <stdlib.h>

#include "infinite_timer.h"	//Infinite_Timer()
#include "utility.h"		//Utility_Pause() etc.


#include "T104_AD16S.h"		//T104_AD16S_AD_Read() etc.
#include "T104_ADA.h"		//T104_ADA_AD_Read(), T104_ADA_Set_DA() etc.
#include "T104_C160.h"		//T104_C160_Count_Read() etc.



#define Menu2_NofADch		4	//for T104_AD16S
#define Menu2_Sampling_Time	1	//[ms]
#define Menu2_OffSet_Cnt	1000	//OffSet_Cnt = (OffSet_Time/Samplimg_Time)

#define Menu2_CUT_OFF	1.0

#define Menu2_GAIN 20.0
#define Menu2_THLESHOLD 10.0

#define Menu2_Kp 0.01

struct Menu2_Data{
	int AD[Menu2_NofADch];
	int angle;
	int target;
	double lpf_emg;
};



void Menu2_Data_Out(FILE *fp, struct Menu2_Data *DataStruct, int loop_cnt){

	int i;

	fprintf(fp, "%lf,", (loop_cnt - Menu2_OffSet_Cnt) * Menu2_Sampling_Time * 0.001);

	for(i=0; i<Menu2_NofADch; i++){
		fprintf(fp, "%d,",DataStruct->AD[i]);
	}

	fprintf(fp, "%d,%d,%lf\n",
		DataStruct->angle,
		DataStruct->target,
		DataStruct->lpf_emg);

	Utility_CLS();

	printf("%d,%d,%lf\n",
		DataStruct->angle,
		DataStruct->target,
		DataStruct->lpf_emg);

}

void Menu2_Timer_Function(FILE *fp){

	static int loop_cnt = 0;

	int DA[4];	//{Speed, NC, NC, NC}

	static struct Menu2_Data DataStruct;

	int abs_emg;
	double order_volt;

	static const double CR_TimeConst = 1.0 / (2.0 * 3.14159265359 * Menu2_CUT_OFF);


	if(loop_cnt == 0){
		DataStruct.lpf_emg = 0;
	}

	//EMG
	T104_AD16S_AD_Read(&DataStruct.AD[0], Menu2_NofADch);

	//Position
	DataStruct.angle = T104_ADA_Single_AD_Read(0,0,0);

	abs_emg = 2048 - DataStruct.AD[0];
	if(abs_emg < 0){
		abs_emg *= -1;
	}

	DataStruct.lpf_emg += ((double)abs_emg - DataStruct.lpf_emg)
				 * Menu2_Sampling_Time * 0.001 / CR_TimeConst;

	DataStruct.target = (int)((DataStruct.lpf_emg - Menu2_THLESHOLD)
				 * Menu2_GAIN) + 1000;

	if(2000 < DataStruct.target){
		DataStruct.target = 2000;
	}
	if(1000 > DataStruct.target){
		DataStruct.target = 1000;
	}


	if(loop_cnt < Menu2_OffSet_Cnt){

		DA[0] = 2048;
		DA[1] = 0;
		DA[2] = 0;
		DA[3] = 0;

		T104_ADA_Set_DA(DA, 4);

		Menu2_Data_Out(fp, &DataStruct, loop_cnt);
		printf("%d\n", DA[0]);

	}else{

		//2.5V = order volt : Stop
		//2.5V > order volt : CW
		//2.5V < order volt : CCW
		order_volt = 2.5 + (double)(DataStruct.target - DataStruct.angle) * Menu2_Kp;

		DA[0] = T104_ADA_VoltToDA(order_volt);;
		DA[1] = 0;
		DA[2] = 0;
		DA[3] = 0;

		T104_ADA_Set_DA(DA, 4);

		Menu2_Data_Out(fp, &DataStruct, loop_cnt);
		printf("%d\n", DA[0]);

	}

	loop_cnt++;

}


void Menu2_Motor_Stop(void){

	int DA[4] = {2048,0,0,0};	//{Enable/Disable, CW/CCW, Open/Brake, Speed}

	T104_ADA_Set_DA(DA, 4);

}


void Menu2_Type_C_Pos_Ctrl(void){

	FILE *fp;
	char filename[32];

	T104_AD16S_Initialize();
	T104_ADA_Initialize();
	T104_C160_Initialize();

	Menu2_Motor_Stop();

	Utility_Get_FileName(filename);

	if ((fp = fopen(filename, "w")) == NULL) {
		printf("in Utility_File_Open()\n");
		printf("An error occurred while opneing %s file!!\n", filename);
		return;
	}

 	Infinite_Timer(1, Menu2_Timer_Function, fp);

	printf("timer STOP\n");

	Menu2_Motor_Stop();

	fclose(fp);

	T104_AD16S_Finishalize();
	T104_ADA_Finishalize();
	T104_C160_Finishalize();

}


#endif
