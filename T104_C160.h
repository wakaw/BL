#ifndef __T104_C160__
#define __T104_C160__


#include <hw/inout.h>		// out8(), in8();
#include <sys/neutrino.h>		// TreadCtl():, mmap_device_io();

#define	BIO_T104_C160		0x320
	//Base I/O Address
#define	SIZE_T104_C160		1
	//Size of I/O Address

#define		C160_COM1	BIO_T104_C160 + 0x00
#define		C160_DATA1_1	BIO_T104_C160 + 0x01
#define		C160_DATA1_2	BIO_T104_C160 + 0x02
#define		C160_DATA1_3	BIO_T104_C160 + 0x03

#define		C160_COM2	BIO_T104_C160 + 0x04
#define		C160_DATA2_1	BIO_T104_C160 + 0x05
#define		C160_DATA2_2	BIO_T104_C160 + 0x06
#define		C160_DATA2_3	BIO_T104_C160 + 0x07

#define		COUNTER_A_WRITE		0x00
#define		COUNTER_A_READ		0x04
#define		COUNTER_B_WRITE		0x02
#define		COUNTER_B_READ		0x06


void T104_C160_Initialize(void)
{
	ThreadCtl( _NTO_TCTL_IO , 0 );
	mmap_device_io(BIO_T104_C160 , SIZE_T104_C160 );
	
	out8(C160_COM1,0x0f);
		//CLAM1(CLBM1) Pin falling edge clears the CH1(CH2) pulse count
	out8(C160_COM1,0x11);
		//A1 2-phase x1 Mode
	out8(C160_COM1,0x19);
		//B1 2-phase x1 Mode

	out8(C160_COM2,0x0f);
		//CLAM2(CLBM2) Pin falling edge clears the CH3(CH4) pulse count
	out8(C160_COM2,0x11);
		//A2 2-phase x1
	out8(C160_COM2,0x19);
		//B2 2-phase x1 Mode

	out8(C160_DATA1_1,0x00);
	out8(C160_DATA1_2,0x00);
	out8(C160_DATA1_3,100);
		//initial value of CH1, CH2 pulse count
	out8(C160_COM1,COUNTER_A_WRITE);
		//writing CH1 initial value
	out8(C160_COM1,COUNTER_B_WRITE);
		//writing CH2 initial value

	out8(C160_DATA2_1,0x00);
	out8(C160_DATA2_2,0x00);
	out8(C160_DATA2_3,100);
		//initial value of CH3, CH4 pulse count
	out8(C160_COM2,COUNTER_A_WRITE);
		//writing CH3 initial value
	out8(C160_COM2,COUNTER_B_WRITE);
		//writing CH4 initial value

}

void T104_C160_Finishalize(void){
	munmap_device_io(BIO_T104_C160 , 0x320);
}

int T104_C160_Count_Read(int ch)
{
	int CNT_data;
	switch(ch){
		case 1:
			out8(C160_COM1, COUNTER_A_READ);
			//printf("%d,%d,%d\n",in8(C160_DATA1_1),in8(C160_DATA1_2),in8(C160_DATA1_3));
			//printf("%d\n",(in8(C160_DATA1_1)<<16) + (in8(C160_DATA1_2)<<8) + in8(C160_DATA1_3));
			CNT_data = (in8(C160_DATA1_1)<<16) + (in8(C160_DATA1_2)<<8) + in8(C160_DATA1_3);
			break;
		case 2:
			out8(C160_COM1, COUNTER_B_READ);
			//printf("%d,%d,%d\n",in8(C160_DATA1_1),in8(C160_DATA1_2),in8(C160_DATA1_3));
			//printf("%d\n",(in8(C160_DATA1_1)<<16) + (in8(C160_DATA1_2)<<8) + in8(C160_DATA1_3));
			CNT_data = (in8(C160_DATA1_1)<<16) + (in8(C160_DATA1_2)<<8) + in8(C160_DATA1_3);
			break;
		case 3:
			out8(C160_COM2, COUNTER_A_READ);
			//printf("%d,%d,%d\n",in8(C160_DATA2_1),in8(C160_DATA2_2),in8(C160_DATA2_3));
			//printf("%d\n",(in8(C160_DATA2_1)<<16) + (in8(C160_DATA2_2)<<8) + in8(C160_DATA2_3));
			CNT_data = (in8(C160_DATA2_1)<<16) + (in8(C160_DATA2_2)<<8) + in8(C160_DATA2_3);
			break;
		case 4:
			out8(C160_COM2, COUNTER_B_READ);
			//printf("%d,%d,%d\n",in8(C160_DATA2_1),in8(C160_DATA2_2),in8(C160_DATA2_3));
			//printf("%d\n",(in8(C160_DATA2_1)<<16) + (in8(C160_DATA2_2)<<8) + in8(C160_DATA2_3));
			CNT_data = (in8(C160_DATA2_1)<<16) + (in8(C160_DATA2_2)<<8) + in8(C160_DATA2_3);
			break;
	}
	return CNT_data;
}

#endif
