#ifndef __BL2_ForceTorque__
#define __BL2_ForceTorque__


struct Force_Torque2{
	double Force[3];
	double Torque[3];
};


void Calc_BL2_Force(int ad_data[], struct Force_Torque2 *DataStruct){

	// MC0109
	double CAL[6][6] =
		{
		  { 0.039052, 0.011317, 0.096994, -1.894633, -0.014261, 1.926911 },
		  { 0.148487, 2.218685,-0.070890, -1.069420, -0.041683, -1.135125 },
		  { 3.329368, 0.160186, 3.284859, -0.061224,  3.310655,  0.183727 },
		  { 0.037989, 1.028789, 1.576418, -0.517350, -1.676306, -0.663189 },
		  {-1.886809,-0.073895, 1.008176, 0.820446,   0.926455, -0.833764 },
		  { 0.022961, 0.846131,-0.036291, 0.806084,   0.047398,  0.832543 }
			};

	int i;
	double ad_voltage[6];

	for(i=0; i<6; i++){
		//ad_voltage[i] = (double)(ad_data[i*2 + 4] - ad_data[i*2 + 5]); // * 4.998 / 2047.0;
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
