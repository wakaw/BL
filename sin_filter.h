#ifndef __SIN_FILTER__
#define __SIN_FILTER__


#define		SAMPLE_Hz	1000.0	//サンプリング周波数
//#define		TREMOR_Hz	6	//基準周波数[Hz]
//#define		VECTOR_CNT	166	//SAMPLE_Hz / TREMOR_Hz
#define		EMG_CNT		1	//EMG ch数

#define 	PI		3.141592653589793238462643383

//構造体定義
struct SinSQStruct{
	double phase[EMG_CNT];
	double correlation[EMG_CNT];
	double attenuation[EMG_CNT];
};

#include <math.h>
#include "filter_data_IO.h"



double base_norm;
int VECTOR_CNT;

double Base_Wave_Func(double w, double power){

//	return sin(w)*sin(w)*sin(w)*sin(w);
//	return sin(w)*sin(w);
	return pow(sin(w), power);

}

/* ノルムの計算 */
double Calc_Norm(double x[]){

	int i;
	double sum = 0.0;

	for(i = 0 ; i < VECTOR_CNT ; i++){
		sum += x[i] * x[i];
	}

	return sqrt(sum);

}

void Set_Base_Norm(double base_wave[]){

	base_norm = Calc_Norm(base_wave);

}

/* 相互相関関数の計算 */
double Calc_Correlation(double x[], double y[], int x_start, int y_start, double norm_x,  double norm_y){

	int i;
	double inner_product = 0.0;

	for(i = 0 ; i < VECTOR_CNT ; i++){
		if(i < VECTOR_CNT - x_start && i < VECTOR_CNT - y_start){
			inner_product += x[i + x_start] * y[i + y_start];
		}
		if(i >= VECTOR_CNT - x_start && i < VECTOR_CNT - y_start){
			inner_product += x[i - VECTOR_CNT + x_start] * y[i + y_start];
		}
		if(i < VECTOR_CNT - x_start && i >= VECTOR_CNT - y_start){
			inner_product += x[i + x_start] * y[i - VECTOR_CNT + y_start];
		}
		if(i >= VECTOR_CNT - x_start && i >= VECTOR_CNT - y_start){
			inner_product += x[i - VECTOR_CNT + x_start] * y[i - VECTOR_CNT + y_start];
		}

	}
	return inner_product / (norm_x * norm_y);

}

void Get_Phase(double object_wave[], double base_wave[], int object_start, double *phase_return, double *correlation_return){

	int i;

//	double correlation[VECTOR_CNT];
	double *correlation;
	double object_norm;

	double max_correlation = 0.0;
	int max_correlation_n = 0;

	correlation = (double *)malloc(sizeof(double) * VECTOR_CNT);

	if(correlation == NULL){
		fprintf(stderr,"メモリが確保できません. \n");
		system("PAUSE");
		exit(1);
	}

	object_norm = Calc_Norm(object_wave);

	for(i = 0 ; i < VECTOR_CNT ; i++){
		correlation[i] = Calc_Correlation(object_wave, base_wave, object_start, i, object_norm, base_norm);
	}

	for(i = 0 ; i < VECTOR_CNT ; i++){
		if(max_correlation < correlation[i]){
			max_correlation_n = i;
			max_correlation = correlation[i];
		}
	}

	*phase_return = ((double)(max_correlation_n + VECTOR_CNT - 1) * PI / VECTOR_CNT) - PI;
	*correlation_return = max_correlation;

	free(correlation);
}

void Phase_Analysis(struct DataStruct Input[], struct SinSQStruct SinSQ[], struct DataStruct Output[], int data_cnt, double toremor_Hz, double pow_sin){

	int i,j;
//	double wave[VECTOR_CNT];
//	double base_wave[VECTOR_CNT];
	double *wave;
	double *base_wave;

	int wave_start = 0;

//	double sqrt_2_3;

//	sqrt_2_3 = sqrt(2.0/3.0);

	/* ベース波形の規定 */
	VECTOR_CNT = (int)(SAMPLE_Hz / toremor_Hz);

	base_wave = (double *)malloc(sizeof(double) * VECTOR_CNT);
	wave = (double *)malloc(sizeof(double) * VECTOR_CNT);

	if(base_wave == NULL || wave == NULL){
		fprintf(stderr,"メモリが確保できません. \n");
		system("PAUSE");
		exit(1);
	}

	for(i = 0 ; i < VECTOR_CNT ; i++){
		base_wave[i] = Base_Wave_Func(i * PI / VECTOR_CNT, pow_sin);
	}

	Set_Base_Norm(base_wave);


	for(i = 0 ; i < data_cnt ; i++){
		Output[i].Time = Input[i].Time;
		Output[i].Gonio[0] = Input[i].Gonio[0];
		Output[i].Gonio[1] = Input[i].Gonio[1];
		Output[i].Velocity[0] = Input[i].Velocity[0];
		Output[i].Velocity[1] = Input[i].Velocity[1];
	}


	for(j = 0 ; j < EMG_CNT ; j++){


		for(i = 0 ; i < VECTOR_CNT ; i++){
			wave[i] = Input[i].Data[j];
			SinSQ[i].phase[j] = 0.0;
			Output[i].Data[j] = Input[i].Data[j];
		}


		for(i = VECTOR_CNT ; i < data_cnt ; i++){

			Get_Phase(wave, base_wave, wave_start, &SinSQ[i].phase[j], &SinSQ[i].correlation[j]);

			SinSQ[i].attenuation[j] = Base_Wave_Func(SinSQ[i].phase[j], pow_sin);
			Output[i].Data[j] = Input[i].Data[j] * (1.0 - SinSQ[i].attenuation[j] * (1.0 / (1.0 + exp(-100.0 * (SinSQ[i].correlation[j] - 0.8)))));
//			Output[i].Data[j] = Input[i].Data[j] * (1.0 - SinSQ[i].attenuation[j] * (1.0 / (1.0 + exp(-100.0 * (SinSQ[i].correlation[j] - sqrt_2_3)))));
//			Output[i].Data[j] = Input[i].Data[j] * (1.0 - SinSQ[i].attenuation[j] * SinSQ[i].correlation[j]);
/*
			if(SinSQ[i].correlation[j] < sqrt_2_3){
				Output[i].Data[j] = Input[i].Data[j];
			}else{
				Output[i].Data[j] = Input[i].Data[j] * (1.0 - SinSQ[i].attenuation[j] * ((SinSQ[i].correlation[j] - sqrt_2_3) / (1.0 - sqrt_2_3)));
			}
*/
			wave[wave_start] = Input[i].Data[j];

			wave_start++;
			if(wave_start >= VECTOR_CNT){
				wave_start = 0;
			}

		}
	printf("Data[%d] Get_Phase completed.\n", j);

	}

	free(base_wave);
	free(wave);
}


int SinSQ_Data_Output(char write_file[], struct DataStruct Input[], struct DataStruct Output[], struct SinSQStruct SinSQ[], int data_cnt){

	FILE *fp;
	errno_t err;

	int i,j;

	// ファイルオープン　VC2005以降はfopen_s()で行うとベター
	err=fopen_s(&fp,write_file,"w");
	if(err!=0){
		fprintf(stderr,"readファイルを開けません. \n");
		system("PAUSE");
		exit(1);
	}

	

	fprintf(fp, "Time,Flexion,Pronation,Flexion_Vel,Pronation_Vel");

	for(j = 0 ; j < EMG_CNT ; j++){
		fprintf(fp, ",,EMG_%d,Phase_%d,SinPow4_%d,Correlation_%d,attenuated_EMG_%d", j, j, j, j, j);
	}

	fprintf(fp, "\n");

	for(i = 0 ; i < data_cnt ; i++){

		fprintf(fp, "%lf,%lf,%lf,%lf,%lf",
			Output[i].Time,
			Output[i].Gonio[0]*180.0/4000.0,
			Output[i].Gonio[1]*180.0/4000.0,
			Output[i].Velocity[0]*180.0/4000.0,
			Output[i].Velocity[1]*180.0/4000.0);

		for(j = 0 ; j < EMG_CNT ; j++){


			fprintf(fp, ",,%lf,%lf,%lf,%lf,%lf",
				Input[i].Data[j],
				SinSQ[i].phase[j],
				SinSQ[i].attenuation[j],
				SinSQ[i].correlation[j],
				Output[i].Data[j]);

		}
		fprintf(fp, "\n");

	}
	// ファイルクローズ
	fclose(fp);


	return data_cnt;
}

void Rectification(struct DataStruct Input[], int data_cnt){

	int i, j;

	for(i = 0 ; i < data_cnt ; i++){

		for(j = 0 ; j < EMG_CNT ; j++){

			if(Input[i].Data[j] < 0.0)	Input[i].Data[j] *= -1.0;

		}
	}

}

#endif
