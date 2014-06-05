#ifndef _GPS_DATA_H
#define _GPS_DATA_H
#define MAX_GPS	16
struct gpsdata{
  struct{
    int time[4];        // [hr, min, sec, ms]
    int date[4];        // [day, month, year]
    int fix;		// fix (1) or not (0)
    int fix_quality;    // NOT SURE ABOUT THIS ONE
    float latitude;     // degrees only (minutes seconds converted)
    float longitude;	// degrees only
    float speed;        // knots
    float angle;        // degrees
    float altitude;	// dunno what format? akira?
    int numSatelites;	// num satelites
  } gps[MAX_GPS];
  int num_gps;
};
#endif
