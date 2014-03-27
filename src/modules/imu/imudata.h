#ifndef _IMU_DATA_H
#define _IMU_DATA_H
struct imudata{
	union{
		float accel_xyz[3];
		struct{
			float A_X,A_Y,A_Z;
		};
	};
	union{
		float gyro_ypr[3];
		struct{
			float G_Y, G_P, G_R;
		};
	};
	float heading;
};
#endif
