#ifndef _GPS_DATA_H
#define _GPS_DATA_H
#define MAX_GPS	16
struct gpsdata{
	struct{
		float latitude;		//degrees only (minutes seconds converted)
		float longitude;	//degrees only
		float altitude;		//dunno what format? akira?
		int numSatelites;	//num satelites
		int fix;		//fix or not
	}gps[MAX_GPS];
	int num_gps;
};
#endif
