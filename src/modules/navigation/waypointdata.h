#ifndef _WAYPOINT_DATA_H
#define _WAYPOINT_DATA_H
struct waypointdata{
		float lat;
		float lon;
		struct waypointdata* nextWaypoint;
};
#endif
