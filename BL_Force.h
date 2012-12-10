#ifndef __BL_ForceTorque__
#define __BL_ForceTorque__


struct Force_Torque{
	double Force[3];
	double Torque[3];
	int EMG[4];
};


void Calc_BL_Force(int ad_data[], struct Force_Torque *DataStruct){

	// MC0014
	double CAL[6][6] =
		{
		  { 0.0228, 0.02651, 0.025775, -1.937675, -0.039569, 1.846959 },
		  { 0.098677, 2.194935, -0.007215, -1.085806, -0.005737, -1.125846 },
		  { 3.434548, 0.100853, 3.340929, -0.035762, 3.361569, 0.121172 },
		  { 0.070831, 1.055859, 1.726459, -0.559931, -1.615722, -0.588073 },
		  { -1.897938, -0.042845, 0.952003, 0.841791, 0.988336, -0.789404 },
		  { 0.010561, 0.789626, -0.013959, 0.938219, 0.032169, 0.811921 }
		};

	int i;
	double ad_voltage[6];
	double emg_voltage[4];

	for(i=0; i<4; i++){
		DataStruct->EMG[i] = ad_data[i];
	}

	for(i=0; i<6; i++){
		ad_voltage[i] = (double)(ad_data[i*2 + 4] - ad_data[i*2 + 5]); // * 4.998 / 2047.0;
	}

	for(i=0; i<3; i++){
		DataStruct->Force[i] = (CAL[i][0] * ad_voltage[0]
						+ CAL[i][1] * ad_voltage[1]
						+ CAL[i][2] * ad_voltage[2]
						+ CAL[i][3] * ad_voltage[3]
						+ CAL[i][4] * ad_voltage[4]
						+ CAL[i][5] * ad_voltage[5]) * 5.0 * 9.80665 / 1000.0;
	}
	for(i=0; i<3; i++){
		DataStruct->Torque[i] = (CAL[i+3][0] * ad_voltage[0]
						+ CAL[i+3][1] * ad_voltage[1]
						+ CAL[i+3][2] * ad_voltage[2]
						+ CAL[i+3][3] * ad_voltage[3]
						+ CAL[i+3][4] * ad_voltage[4]	
						+ CAL[i+3][5] * ad_voltage[5]) * 10.0 * 9.80665 / 100.0;
	}
}

#endif
