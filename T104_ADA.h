#ifndef __T104_ADA__
#define __T104_ADA__


#include <hw/inout.h>			// out8(), in8(), etc.
#include <sys/neutrino.h>		// TreadCtl():, mmap_device_io(), etc.

#define	BIO_T104_ADA	0x0340	
	//Base I/O Address
#define	SIZE_T104_ADA		1
	//Size of I/O Address

#define	T104_ADA_DA0L	BIO_T104_ADA+0x0000
	// low order byte of D/A ch0
#define	T104_ADA_DA0U	BIO_T104_ADA+0x0001
	// upper order byte of D/A ch0
#define	T104_ADA_DA1L	BIO_T104_ADA+0x0002
	// low order byte of D/A ch1
#define	T104_ADA_DA1U	BIO_T104_ADA+0x0003
	// upper order byte of D/A ch1
#define	T104_ADA_DA2L	BIO_T104_ADA+0x0004
	// low order byte of D/A ch2
#define	T104_ADA_DA2U	BIO_T104_ADA+0x0005
	// upper order byte of D/A ch2
#define	T104_ADA_DA3L	BIO_T104_ADA+0x0006
	// low order byte of D/A ch3
#define	T104_ADA_DA3U	BIO_T104_ADA+0x0007
	// upper order byte of D/A ch3
#define	T104_ADA_LDAC	BIO_T104_ADA+0x0000
	// load data into D/A converter

#define	T104_ADA_ADL	BIO_T104_ADA+0x0008
	// low order byte of A/D converter
#define	T104_ADA_ADU	BIO_T104_ADA+0x0009
	// upper order byte of A/D converter
#define	T104_ADA_ADSTS	BIO_T104_ADA+0x000a
	// satus of A/D converter

void T104_ADA_Initialize(void)
{

	mmap_device_io(BIO_T104_ADA , SIZE_T104_ADA);
}

void T104_ADA_Finishalize(void)
{

	munmap_device_io(BIO_T104_ADA , SIZE_T104_ADA);
}


// SingleEnded_Differential (config bit 4) <0:Single Ended 1:Differential>
// Range (config bit 3) <0 : 0/5[V] 1 : -2.5/+2.5[V]>
// CH (config bit 0,1,2) 
// Single Ended
// <0 : Pin1 from Pin2(GND), 1 : Pin3 from Pin4(GND), 2 : Pin5 from Pin6(GND), 3 : Pin7 from Pin8(GND)>
// <4 : Pin9 from Pin10(GND), 5 : Pin11 from Pin12(GND), 6 : Pin13 from Pin14(GND), 7 : Pin15 from Pin16(GND)>
// Differential
// <0 : Pin1 from Pin3, 1 : Pin3 from Pin1(GND), 2 : Pin5 from Pin7, 3 : Pin7 from Pin5)>
// <4 : Pin9 from Pin11, 5 : Pin11 from Pin9, 6 : Pin13 from Pin15, 7 : Pin15 from Pin13>
int T104_ADA_Single_AD_Read(int CH, int Range, int SingleEnded_Differential)
{
	int config;

	config = CH | (Range << 3) | (SingleEnded_Differential << 4);

	//start A/D converting
	out8(T104_ADA_ADL, config);

	//wait for the ADSTS Bit7 to set "1"
	//It means A/D converting is finished
	while((in8(T104_ADA_ADSTS) & 0x80) != 0x80);
	
	return((in8(T104_ADA_ADU) << 8) + in8(T104_ADA_ADL));
}


void T104_ADA_AD_Read(int AD_data[], int N, int Range, int SingleEnded_Differential)
{

	int i;

	for(i = 0; i < N; i++){

		AD_data[i] = T104_ADA_Single_AD_Read(i, Range, SingleEnded_Differential);
	}

}


void T104_ADA_Single_Set_DA(int DA_data, int CH){
	switch(CH){
		case 0:
			out8(T104_ADA_DA0L, DA_data & 0x00FF);
			out8(T104_ADA_DA0U, DA_data >> 8);
			in8(T104_ADA_LDAC);
			break;
		case 1:
			out8(T104_ADA_DA1L, DA_data & 0x00FF);
			out8(T104_ADA_DA1U, DA_data >> 8);
			in8(T104_ADA_LDAC);
			break;
		case 2:
			out8(T104_ADA_DA2L, DA_data & 0x00FF);
			out8(T104_ADA_DA2U, DA_data >> 8);
			in8(T104_ADA_LDAC);
			break;
		case 3:
			out8(T104_ADA_DA3L, DA_data & 0x00FF);
			out8(T104_ADA_DA3U, DA_data >> 8);
			in8(T104_ADA_LDAC);
			break;
		default:
			break;
	}
}

void T104_ADA_Set_DA(int DA_data[], int N){

	out8(T104_ADA_DA0L, DA_data[0] & 0x00FF);
	out8(T104_ADA_DA0U, DA_data[0] >> 8);
	if(1 < N){
		out8(T104_ADA_DA1L, DA_data[1] & 0x00FF);
		out8(T104_ADA_DA1U, DA_data[1] >> 8);
	}
	if(2 < N){
		out8(T104_ADA_DA2L, DA_data[2] & 0x00FF);
		out8(T104_ADA_DA2U, DA_data[2] >> 8);
	}
	if(3 < N){
		out8(T104_ADA_DA3L, DA_data[3] & 0x00FF);
		out8(T104_ADA_DA3U, DA_data[3] >> 8);
	}
	
	//start D/A converting
	in8(T104_ADA_LDAC);

}

int T104_ADA_FlagToDA(int flag){

	switch(flag){
		case 0:
			return 0;
			break;
		case 1:
			return 4095;
			break;
		default:
			return 0;
	}

}

int T104_ADA_VoltToDA(double Volt){

	int DA_value;

	DA_value = (int)(819.0*Volt);

	if(DA_value < 0){
		DA_value = 0;
	}
	if(4095 < DA_value){
		DA_value = 4095;
	}

	return DA_value;

}



#endif
