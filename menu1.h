#ifndef __MENU1_Type_A_Pos_Ctrl__
#define __MENU1_Type_A_Pos_Ctrl__

#include <stdio.h>
#include <stdlib.h>

#include "infinite_timer.h"	//Infinite_Timer()
#include "utility.h"		//Utility_Pause() etc.


#include "T104_AD16S.h"		//T104_AD16S_AD_Read() etc.
#include "T104_ADA.h"		//T104_ADA_AD_Read(), T104_ADA_Set_DA() etc.
#include "T104_C160.h"		//T104_C160_Count_Read() etc.



#define Menu1_NofADch		4	//for T104_AD16S
#define Menu1_Sampling_Time	1	//[ms]
#define Menu1_OffSet_Cnt	1000	//OffSet_Cnt = (OffSet_Time/Samplimg_Time)

#define Menu1_CUT_OFF	1.0

#define Menu1_GAIN 0.5
#define Menu1_THLESHOLD 23.0

#define Menu1_Kp 0.002

struct Menu1_Data{
	int AD[Menu1_NofADch];
	int angle;
	int target;
	double lpf_emg;
};



void Menu1_Data_Out(FILE *fp, struct Menu1_Data *DataStruct, int loop_cnt){

	int i;

	fprintf(fp, "%lf,\n"(loop_cnt - Menu1_OffSet_Cnt) * Menu1_Sampling_Time * 0.001);
	for(i=0,i<Menu1_NofADch,i++){
		fprintf(fp, "%lf,\n",DataStruct->AD[i]);
	}

	fprintf(fp, "%lf,%lf,%lf\n",
		DataStruct->angle,
		DataStruct->target,
		DataStruct->lpf_emg);

	Utility_CLS();

	printf("%lf,%lf,%lf\n",
		DataStruct->angle,
		DataStruct->target,
		DataStruct->lpf_emg);

}

void Menu1_Timer_Function(FILE *fp){

	static int loop_cnt = 0;

	int DA_V[4];	//{Enable/Disable, CW/CCW, Open/Brake, Speed}

	static struct Menu1_Data DataStruct;

	int abs_emg;

	static const double CR_TimeConst = 1.0 / (2.0 * 3.14159265359 * CUT_OFF);

	if(loop_cnt == 0){
		DataStruct.lpf_emg = 0;
	}


	T104_AD16S_AD_Read(&DataStruct->AD, Menu1_NofADch);

	angle = T104_C160_Count_Read(1)
			+T104_C160_Count_Read(2)
			+T104_C160_Count_Read(3);


	abs_emg = 2048 - DataStruct.AD[0];
	if(abs_emg < 0){
		abs_emg *= -1;
	}

	DataStruct.lpf_emg += ((double)abs_emg - DataStruct.lpf_emg)
				 * Menu1_Sampling_Time * 0.001 / CR_TimeConst;

	DataStruct.target = (int)((DataStruct.lpf_emg - Menu1_THLESHOLDTHLESHOLD)
				 * Menu1_THLESHOLDGAIN) + 300;

	if(1200 < DataStruct.target){
		DataStruct.target = 1200;
	}
	if(300 > DataStruct.target){
		DataStruct.target = 300;
	}





	if(loop_cnt < Menu1_OffSet_Cnt){

		DA_V[0] = 4095;
		DA_V[1] = 0;
		DA_V[2] = 0;
		DA_V[3] = 0;

		T104_ADA_Set_DA(DA_V, 4);
		Menu1_Data_Out(fp, &DataStruct, loop_cnt);

	}else{

		if(DataStruct.target < DataStruct.angle){
			order_volt = (double)(DataStruct.angle - DataStruct.target) * Menu1_Kp;
			DA_V[0] = 4095;
			DA_V[1] = 4095;
			DA_V[2] = 4095;
			DA_V[3] = T104_ADA_VoltToDA(order_volt);	
		}else{
			order_volt = (double)(DataStruct.target - DataStruct.angle) * Menu1_Kp;
			DA_V[0] = 4095;
			DA_V[1] = 0;
			DA_V[2] = 4095;
			DA_V[3] = T104_ADA_VoltToDA(order_volt);
		}


		T104_ADA_Set_DA(DA_V, 4);
		Menu1_Data_Out(fp, &DataStruct, loop_cnt);

	}

	loop_cnt++;

}


void Menu1_Motor_Stop(void){

	int DA_V[4] = {0,0,0,0};	//{Enable/Disable, CW/CCW, Open/Brake, Speed}

	T104_ADA_Set_DA(DA_V, 4);

}


void Menu1_Type_A_Pos_Ctrl(void){

	FILE *fp;
	char filename[32];

	T104_AD16S_Initialize();
	T104_ADA_Initialize();
	T104_C160_Initialize();

	Menu1_Motor_Stop();

	Utility_Get_FileName(filename);

	if ((fp = fopen(filename, "w")) == NULL) {
		printf("in Utility_File_Open()\n");
		printf("An error occurred while opneing %s file!!\n", filename);
		return;
	}

 	Infinite_Timer(1, Menu1_Timer_Function, fp);

	printf("timer STOP\n");

	Menu1_Motor_Stop();

	fclose(fp);

	T104_AD16S_Finishalize();
	T104_ADA_Finishalize();
	T104_C160_Finishalize();

}


#endif
