#ifndef __MENU1_6axis_ForceTorque__
#define __MENU1_6axis_ForceTorque__

#include <stdio.h>
#include <stdlib.h>

#include "infinite_timer.h"	//Infinite_Timer()
#include "utility.h"		//Utility_Pause() etc.


#include "T104_AD16S.h"		//T104_AD16S_AD_Read() etc.
#include "T104_AD16S2.h"		//T104_AD16S_AD2_Read() etc.

#include "BL_Force.h"		//struct Force_Torque, Calc_BL_Force()
#include "BL_Force2.h"	//struct Force_Torque2, Calc_BL2_Force()

#define Menu4_NofADch		16
#define Menu4_Sampling_Time	1
#define Menu4_Sampling_Time_Output	10
#define Menu4_Sampling_Time_Display	100

#define Menu4_OffSet_Cnt	5000	// OffSet_Cnt = (Measeurement_Time/Samplimg_Time)

void Menu4_Measure_BL_OffSet(struct Force_Torque *DataStruct, struct Force_Torque *OffSet, int loop_cnt){

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

	if(loop_cnt == Menu4_OffSet_Cnt - 1){
		OffSet->Force[0] /= Menu4_OffSet_Cnt;
		OffSet->Force[1] /= Menu4_OffSet_Cnt;
		OffSet->Force[2] /= Menu4_OffSet_Cnt;
		OffSet->Torque[0] /= Menu4_OffSet_Cnt;
		OffSet->Torque[1] /= Menu4_OffSet_Cnt;
		OffSet->Torque[2] /= Menu4_OffSet_Cnt;
	}

}

void Menu4_Measure_BL2_OffSet(struct Force_Torque2 *DataStruct, struct Force_Torque2 *OffSet, int loop_cnt){

	if(loop_cnt == 0){
		printf("BL2 Force Torque Sensor Initializing...\n");
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

	if(loop_cnt == Menu4_OffSet_Cnt - 1){
		OffSet->Force[0] /= Menu4_OffSet_Cnt;
		OffSet->Force[1] /= Menu4_OffSet_Cnt;
		OffSet->Force[2] /= Menu4_OffSet_Cnt;
		OffSet->Torque[0] /= Menu4_OffSet_Cnt;
		OffSet->Torque[1] /= Menu4_OffSet_Cnt;
		OffSet->Torque[2] /= Menu4_OffSet_Cnt;
	}

}

void Menu4_Data_Out(FILE *fp, struct Force_Torque *DataStruct, struct Force_Torque2 *DataStruct2, struct Force_Torque *OffSet, struct Force_Torque2 *OffSet2,  struct Force_Torque *MaxValue, struct Force_Torque2 *MaxValue2, int loop_cnt){

	struct Force_Torque CurrentValue;
	struct Force_Torque2 CurrentValue2;
	static int status;
	
    if( ((loop_cnt - Menu4_OffSet_Cnt) * Menu4_Sampling_Time)%Menu4_Sampling_Time_Output == 0){ // 10[msec	]

		CurrentValue.Force[0] = DataStruct->Force[0] - OffSet->Force[0];
		CurrentValue.Force[1] = DataStruct->Force[1] - OffSet->Force[1];
		CurrentValue.Force[2] = DataStruct->Force[2] - OffSet->Force[2];
		CurrentValue.Torque[0] = DataStruct->Torque[0] - OffSet->Torque[0];
		CurrentValue.Torque[1] = DataStruct->Torque[1] - OffSet->Torque[1];
		CurrentValue.Torque[2] = DataStruct->Torque[2] - OffSet->Torque[2];
		CurrentValue.EMG[0]     = DataStruct->EMG[0];
	
		CurrentValue2.Force[0] = DataStruct2->Force[0] - OffSet2->Force[0];
		CurrentValue2.Force[1] = DataStruct2->Force[1] - OffSet2->Force[1];
		CurrentValue2.Force[2] = DataStruct2->Force[2] - OffSet2->Force[2];
		CurrentValue2.Torque[0] = DataStruct2->Torque[0] - OffSet2->Torque[0];
		CurrentValue2.Torque[1] = DataStruct2->Torque[1] - OffSet2->Torque[1];
		CurrentValue2.Torque[2] = DataStruct2->Torque[2] - OffSet2->Torque[2];
				
		fprintf(fp, "%2.3lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%8d\n",
			(loop_cnt - Menu4_OffSet_Cnt) * Menu4_Sampling_Time * 0.001,
			CurrentValue.Force[0],
			CurrentValue.Force[1],
			CurrentValue.Force[2],
			CurrentValue.Torque[0],
			CurrentValue.Torque[1],
			CurrentValue.Torque[2],
			
			CurrentValue2.Force[0],
			CurrentValue2.Force[1],
			CurrentValue2.Force[2],
			CurrentValue2.Torque[0],
			CurrentValue2.Torque[1],
			CurrentValue2.Torque[2],		

			DataStruct->EMG[0]
		);

		if( ((loop_cnt - Menu4_OffSet_Cnt) * Menu4_Sampling_Time)%Menu4_Sampling_Time_Display == 0){ // 100[msec]
			Utility_CLS();
			
			if(abs(CurrentValue.Force[2]) > abs(MaxValue->Force[2])) MaxValue->Force[2] = CurrentValue.Force[2];
			if(abs(CurrentValue2.Force[2]) > abs(MaxValue2->Force[2])) MaxValue2->Force[2] = CurrentValue2.Force[2];
			status = (DataStruct->EMG[0] > 2047) ? 0 : 1;

			printf("F1z[N]: %2.3lf\n Max[N]: %2.3lf\n\nF2z[N]: %2.3lf\n Max[N]: %2.3lf\n\nTRG: %8d\n status: %8d\n",
				CurrentValue.Force[2],
				MaxValue->Force[2],
				
				CurrentValue2.Force[2],			
				MaxValue2->Force[2],
				
				DataStruct->EMG[0],
				status							
			);
/*
			printf("F1x[N]: %2.3lf\nF1y[N]: %2.3lf\nF1z[N]: %2.3lf\nT1x[N]: %2.3lf\nT1y[N]: %2.3lf\t%2.3lf\n\n%2.3lf\t%2.3lf\t%2.3lf\n%2.3lf\t%2.3lf\t%2.3lf\n\n%8d\n",
				CurrentValue.Force[0],
				CurrentValue.Force[1],
				CurrentValue.Force[2],
				CurrentValue.Torque[0],
				CurrentValue.Torque[1],
				CurrentValue.Torque[2],
			
				CurrentValue2.Force[0],
				CurrentValue2.Force[1],
				CurrentValue2.Force[2],
				CurrentValue2.Torque[0],
				CurrentValue2.Torque[1],
				CurrentValue2.Torque[2],		

				DataStruct->EMG[0]				
			);
*/
		}		
	}
}

void Menu4_Timer_Function(FILE *fp){

	static int loop_cnt = 0;

	int ad_data[Menu4_NofADch];
	int ad_data2[Menu4_NofADch];

	struct Force_Torque DataStruct;
	struct Force_Torque2 DataStruct2;

	static struct Force_Torque OffSet;
	static struct Force_Torque2 OffSet2;
	
	static struct Force_Torque MaxValue;
	static struct Force_Torque2 MaxValue2;

	T104_AD16S_AD_Read(ad_data, Menu4_NofADch);
	T104_AD16S2_AD_Read(ad_data2, Menu4_NofADch);

	Calc_BL_Force(ad_data, &DataStruct);
	Calc_BL2_Force(ad_data2, &DataStruct2);
	
	if(loop_cnt < Menu4_OffSet_Cnt){

		Menu4_Measure_BL_OffSet(&DataStruct, &OffSet, loop_cnt);
		Menu4_Measure_BL2_OffSet(&DataStruct2, &OffSet2, loop_cnt);
		MaxValue.Force[2] = 0.0;
		MaxValue2.Force[2] = 0.0;

	}else{

		Menu4_Data_Out(fp, &DataStruct, &DataStruct2, &OffSet, &OffSet2, &MaxValue, &MaxValue2, loop_cnt);

	}

	loop_cnt++;

}


void Menu4_6axis_ForceTorque(void){

	FILE *fp;
	char filename[32];

	T104_AD16S_Initialize();
	T104_AD16S2_Initialize();

	Utility_Get_FileName(filename);

	if ((fp = fopen(filename, "w")) == NULL) {
		printf("in Utility_File_Open()\n");
		printf("An error occurred while opneing %s file!!\n", filename);
		return;
	}

	fprintf(fp, "Time[sec],F1x[N],F1y[N],F1z[N],T1x[Nm],T1y[Nm],T1z[Nm],F2x[N],F2y[N],F2z[N],T2x[Nm],T2y[Nm],T2z[Nm],TRG\n");
 	Infinite_Timer(Menu4_Sampling_Time, Menu4_Timer_Function, fp);

	printf("timer STOP\n");

	fclose(fp);

	T104_AD16S_Finishalize();
	T104_AD16S2_Finishalize();

}


#endif
