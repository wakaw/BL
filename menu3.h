#ifndef __MENU1_6axis_ForceTorque__
#define __MENU1_6axis_ForceTorque__

#include <stdio.h>
#include <stdlib.h>

#include "infinite_timer.h"	//Infinite_Timer()
#include "utility.h"		//Utility_Pause() etc.


#include "T104_AD16S.h"		//T104_AD16S_AD_Read() etc.
#include "T104_ADA.h"		//T104_ADA_AD_Read(), T104_ADA_Set_DA() etc.
#include "T104_C160.h"		//T104_C160_Count_Read() etc.

#include "BL_Force.h"		//struct Force_Torque, Calc_BL_Force()

#define Menu3_NofADch		12
#define Menu3_Sampling_Time	1
#define Menu3_OffSet_Cnt	5000	// OffSet_Cnt = (Measeurement_Time/Samplimg_Time)


#define Menu3_Kp	0.002

void Menu3_Measure_BL_OffSet(struct Force_Torque *DataStruct, struct Force_Torque *OffSet, int loop_cnt){
	if(loop_cnt == 0){
		printf("BL Force Torque Sensor Initializing...\n");
		OffSet->Force[0] = DataStruct->Force[0];
		OffSet->Force[1] = DataStruct->Force[1];
		OffSet->Force[2] = DataStruct->Force[2];
		OffSet->Torque[0] = DataStruct->Torque[0];
		OffSet->Torque[1] = DataStruct->Torque[1];
		OffSet->Torque[2] = DataStruct->Torque[2];
	}else{
		OffSet->Force[0] += DataStruct->Force[0];
		OffSet->Force[1] += DataStruct->Force[1];
		OffSet->Force[2] += DataStruct->Force[2];
		OffSet->Torque[0] += DataStruct->Torque[0];
		OffSet->Torque[1] += DataStruct->Torque[1];
		OffSet->Torque[2] += DataStruct->Torque[2];
	}

	if(loop_cnt == Menu3_OffSet_Cnt - 1){
		OffSet->Force[0] /= Menu3_OffSet_Cnt;
		OffSet->Force[1] /= Menu3_OffSet_Cnt;
		OffSet->Force[2] /= Menu3_OffSet_Cnt;
		OffSet->Torque[0] /= Menu3_OffSet_Cnt;
		OffSet->Torque[1] /= Menu3_OffSet_Cnt;
		OffSet->Torque[2] /= Menu3_OffSet_Cnt;
	}

}

void Menu3_Data_Out(FILE *fp, struct Force_Torque *DataStruct, int loop_cnt){

	fprintf(fp, "%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
		(loop_cnt - Menu3_OffSet_Cnt) * Menu3_Sampling_Time * 0.001,
		DataStruct->Force[0],
		DataStruct->Force[1],
		DataStruct->Force[2],
		DataStruct->Torque[0],
		DataStruct->Torque[1],
		DataStruct->Torque[2]);

	Utility_CLS();

	printf("%2.3lf\t%2.3lf\t%2.3lf\n%2.3lf\t%2.3lf\t%2.3lf\n",
		DataStruct->Force[0],
		DataStruct->Force[1],
		DataStruct->Force[2],
		DataStruct->Torque[0],
		DataStruct->Torque[1],
		DataStruct->Torque[2]);

}


void Menu3_Motor_Ctrl(struct Force_Torque *DataStruct){

	int DA_V[4] = {0,0,0,0};	//{Enable/Disable, CW/CCW, Open/Brake, Speed}

	double target;
	double order_volt;

	target = 0.0;

	if(target < DataStruct->Torque[2]){
		order_volt = (DataStruct->Torque[2] - target) * Menu3_Kp;
		DA_V[0] = 4095;
		DA_V[1] = 4095;
		DA_V[2] = 4095;
		DA_V[3] = T104_ADA_VoltToDA(order_volt);	
	}else{
		order_volt = (target - DataStruct->Torque[2]) * Menu3_Kp;
		DA_V[0] = 4095;
		DA_V[1] = 0;
		DA_V[2] = 4095;
		DA_V[3] = T104_ADA_VoltToDA(order_volt);
	}

	Set_DA(DA_V, 4);


}

void Menu3_Motor_Stop(void){

	int DA_V[4] = {0,0,0,0};	//{Enable/Disable, CW/CCW, Open/Brake, Speed}

	T104_ADA_Set_DA(DA_V, 4);

}


/* Menu3 Loop Function */
void Menu3_Timer_Function(FILE *fp){

	static int loop_cnt = 0;

	int ad_data[Menu3_NofADch];
	int angle;

	struct Force_Torque DataStruct;
	static struct Force_Torque OffSet;


	T104_AD16S_AD_Read(ad_data, Menu3_NofADch);

	Calc_BL_Force(ad_data, &DataStruct);
	angle = Count_Read(1)+Count_Read(2)+Count_Read(3);

	if(loop_cnt < Menu3_OffSet_Cnt){

		Menu3_Measure_BL_OffSet(&DataStruct, &OffSet, loop_cnt);

	}else{

		DataStruct.Force[0] -= OffSet.Force[0];
		DataStruct.Force[1] -= OffSet.Force[1];
		DataStruct.Force[2] -= OffSet.Force[2];
		DataStruct.Torque[0] -= OffSet.Torque[0];
		DataStruct.Torque[1] -= OffSet.Torque[1];
		DataStruct.Torque[2] -= OffSet.Torque[2];

		Menu3_Motor_Ctrl(&DataStruct);

		Menu3_Data_Out(fp, &DataStruct, loop_cnt);

	}

	loop_cnt++;

}


/* Menu3 Main Function */
void Menu3_Torque_Ctrl(void){

	FILE *fp;
	char filename[32];


	// Boad Initialize
	T104_AD16S_Initialize();
	T104_ADA_Initialize();
	T104_C160_Initialize();

	// Motor Stop (for Safety)
	Menu3_Motor_Stop();

	// File Open
	Utility_Get_FileName(filename);
	if ((fp = fopen(filename, "w")) == NULL) {
		printf("in Utility_File_Open()\n");
		printf("An error occurred while opneing %s file!!\n", filename);
		return;
	}

	// Timer, untill one of any keyboad was hit.
 	Infinite_Timer(1, Menu3_Timer_Function, fp);

	// Motor Stop (for Safety)
	Menu3_Motor_Stop();

	// File Close
	fclose(fp);

	// Boad Finishalize
	T104_AD16S_Finishalize();
	T104_ADA_Finishalize();
	T104_C160_Finishalize();

}


#endif
