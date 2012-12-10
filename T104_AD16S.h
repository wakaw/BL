//	Range	0/+10[V]	-5/+5[V]	-10/+10[V]
//	JP1	A		B		B
//	JP2	A		B		B
//	JP3	B		B		A



#ifndef __T104_AD16S__
#define __T104_AD16S__



#include <hw/inout.h>		// out8(), in8(), etc.
#include <sys/neutrino.h>		// TreadCtl(), mmap_device_io(), etc.



#define	BIO_T104_AD16S	0x300
	//Base I/O Address
		
#define	SIZE_T104_AD16S	1
	//Size of I/O Address [Byte]


#define T104_AD16S_ADL	BIO_T104_AD16S+0x01
	// low order byte of A/D conversio

#define T104_AD16S_ADU	BIO_T104_AD16S+0x00
	// upper order byte of A/D conversion

#define T104_AD16S_ADCH	BIO_T104_AD16S+0x02
	// channel selecting status of A/Dconverter

#define T104_AD16S_ADCVT12	BIO_T104_AD16S+0x00
	// start trigger of 12bit A/D conversion

#define T104_AD16S_ADSTS	BIO_T104_AD16S+0x02
	// satus of A/D converter
	// while A/D converter is busy, bit 7 is H



void T104_AD16S_Initialize(void){

	ThreadCtl( _NTO_TCTL_IO , 0 );

	mmap_device_io(BIO_T104_AD16S , SIZE_T104_AD16S );

}



void T104_AD16S_Finishalize(void){

	munmap_device_io(BIO_T104_AD16S ,SIZE_T104_AD16S );

}



int T104_AD16S_Single_AD_Read( int ch ){

	int AD_data;

	out8(T104_AD16S_ADCH,ch);
	out8(T104_AD16S_ADCVT12,00);
	while((in8(T104_AD16S_ADSTS) & 0x80) !=0){;}
	AD_data=((in8(T104_AD16S_ADU)<<4)+(in8(T104_AD16S_ADL)>>4));
	return AD_data;

}



void T104_AD16S_AD_Read(int AD_data[], int n){

	int i;

	for(i = 0; i < n; i++){
		AD_data[i] = T104_AD16S_Single_AD_Read(i);
	}

}

#endif


