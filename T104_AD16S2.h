//	Range	0/+10[V]	-5/+5[V]	-10/+10[V]
//	JP1	A		B		B
//	JP2	A		B		B
//	JP3	B		B		A



#ifndef __T104_AD16S2__
#define __T104_AD16S2__



#include <hw/inout.h>		// out8(), in8(), etc.
#include <sys/neutrino.h>		// TreadCtl(), mmap_device_io(), etc.



#define	BIO_T104_AD16S2	0x320
	//Base I/O Address

#define	SIZE_T104_AD16S2	1
	//Size of I/O Address [Byte]


#define T104_AD16S2_ADL	BIO_T104_AD16S2+0x01
	// low order byte of A/D conversio

#define T104_AD16S2_ADU	BIO_T104_AD16S2+0x00
	// upper order byte of A/D conversion

#define T104_AD16S2_ADCH	BIO_T104_AD16S2+0x02
	// channel selecting status of A/Dconverter

#define T104_AD16S2_ADCVT12	BIO_T104_AD16S2+0x00
	// start trigger of 12bit A/D conversion

#define T104_AD16S2_ADSTS	BIO_T104_AD16S2+0x02
	// satus of A/D converter
	// while A/D converter is busy, bit 7 is H



void T104_AD16S2_Initialize(void){

	ThreadCtl( _NTO_TCTL_IO , 0 );

	mmap_device_io(BIO_T104_AD16S2 , SIZE_T104_AD16S2 );

}



void T104_AD16S2_Finishalize(void){

	munmap_device_io(BIO_T104_AD16S2, SIZE_T104_AD16S2 );

}



int T104_AD16S2_Single_AD_Read( int ch ){

	int AD_data;

	out8(T104_AD16S2_ADCH,ch);
	out8(T104_AD16S2_ADCVT12,00);
	while((in8(T104_AD16S2_ADSTS) & 0x80) !=0){;}
	AD_data=((in8(T104_AD16S2_ADU)<<4)+(in8(T104_AD16S2_ADL)>>4));
	return AD_data;

}



void T104_AD16S2_AD_Read(int AD_data[], int n){

	int i;

	for(i = 0; i < n; i++){
		AD_data[i] = T104_AD16S2_Single_AD_Read(i);
	}

}

#endif


