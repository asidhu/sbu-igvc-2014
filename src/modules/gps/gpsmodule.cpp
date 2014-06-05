#include "modules/gps/gpsmodule.h"
#include "event.h"
#include "base.h"
#include "osutils.h"
#include "event_flag.h"
#include "Adafruit_GPS.h"
#include <iostream>


/**
   rules of module ettiquette:
   1) only create a thread if you are reading from a file, doing io actions, socket ops, sleeping, blocking OR intensive calculations being performed. (AKA camera manipulation operations, intense geometric operations, anything greater than 5 ms worth of time)
   2) all threads should be monitored and kept track of and cleaned up when asked to.
   3) all modules should respond to standard commands.
**/
	
/**
   Rules:
   1) FEEL FREE to read information in from files. 
   2) If this is like the camera, and a command comes in to connect to camera, dont read configuration than. Load configuration NOW.
   3) dont open hardware or pipes or sockets now, there is a command for that. Open it at that time, only read files.
   4) listeners should also be setup HERE. If you want to listen to some event, take listener_flag and | it with the event flag.
**/
void gpsmodule::initialize(uint32& listener_flag){
  listener_flag |= EFLAG_GPSRAW;
}

void gpsmodule::printEvent(std::ostream& out, const event* evt){
  gpsdata* gps = (gpsdata*)(evt->m_data);
  out<< "gps:CPU:"<<std::endl;
}
/**
   Rules:
   1) DO NOT BLOCK. NO IO, NO SLEEP, NO WAITING. DO NOT PERFORM INTENSIVE CALCULATIONS HERE.
   2) update is a method for publishing events to event queue or for syncing with the rest of the robots systems,(cleaning data structures whatnot)
   3) clean up any data from your previously published events (event objects are deleted after they are pushed be careful about that).
**/
void gpsmodule::update(bot_info* data){
  uint64 timegap = data->m_currentTime - m_last_report_time;
  //after receiving event, parse it, and push to world.
  if(m_dataArrived && timegap > GPS_REPORT_INTERVAL){
    event* evt = this->makeEvent(EFLAG_GPSDATA,&m_gpsdata);
    evt->m_print= gpsmodule::printEvent;
    data->m_eventQueue.push_back(evt);
    m_dataArrived=false;
    m_last_report_time = data->m_currentTime;
  }
}
/**
   Rules:
   1) this is if an event is pushed to you.
   2) this should not block, either just like update (i.e. queries to hardware or reading from file should be offloaded to another thread.
		
**/
void gpsmodule::pushEvent(event* evt){
  const char* TIME_FORMAT = "GPS%d,T,%d:%d:%d.%d"; // Hr:Min:Sec.ms
  const char* DATE_FORMAT = "GPS%d,D,%d/%d/%d";    // Day/Month/Year
  const char* FIX_FORMAT = "GPS%d,F,%d,%d";        // fix(y,n), quality
  const char* LOC_FORMAT = "GPS%d,L,%f%c,%f%c";    // latitude,longitude
  const char* SPEED_FORMAT = "GPS%d,S,%f";         // speed (knots)
  const char* ANGLE_FORMAT = "GPS%d,A,%f";         // angle
  const char* ELEV_FORMAT = "GPS%d,E,%f";          // elevation
  const char* SAT_FORMAT = "GPS%d,N,%d";           // num satelltes
  //read and parse event and create another event to push in next update.
  if (evt->m_event_flag & EFLAG_GPSRAW && 
      evt->m_event_flag & EFLAG_ARDUINORAW) {
    arduinodata* data = (arduinodata*)(evt->m_data);
    int num_gps = gps_data->num_gps;
    if (data->data[0]==ARDUINO_TAG_GPS) {
      int gps_id; // which of the 3 GPS units sent data
      
      // Case on protocol type
      switch(data->data[5]) {

      case ARDUINO_PROTOCOL_TIME:
	int time[4];
	if (sscanf(data->data, TIME_FORMAT, &gps_id, time) == 5) {
	  for (int i = 0; i < 4; i++)
	    m_gpsdata.gps[gps_id].time[i] = time[i];
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read time from arduino data!");
	}
	break;

      case ARDUINO_PROTOCOL_DATE:
	int date[3];
	if (sscanf(data->data, DATE_FORMAT, &gps_id, date) == 4) {
	  for (int i = 0; i < 4; i++)
	    m_gpsdata.gps[gps_id].date[i] = date[i];
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read date from arduino data!");
	}
	break;

      case ARDUINO_PROTOCOL_FIX:
	int fix, quality;
	if (sscanf(data->data, FIX_FORMAT, &gps_id, &fix, &quality)
	    == 3) {
	  m_gpsdata.gps[gps_id].fix = fix;
	  m_gpsdata.gps[gps_id].fix_quality = quality;
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read fix info from arduino message!");
	}
	break;

      case ARDUINO_PROTOCOL_LOC:
	float longitude, latitude;
	char longitude_dir, latitude_dir; // N,S,E,W
	if (sscanf(data->data, LOC_FORMAT, &gps_id, 
		   &latitude, &latitude_dir,
		   &longitude, &longitude_dir) == 5) {
	  m_gpsdata.gps[gps_id].latitude = latitude *
	    (latitude_dir == 'S' | latitude_dir == 's' ? -1 : 1);
	  m_gpsdata.gps[gps_id].longitude = longitude * 
	    (longitude_dir == 'W' | longitude_dir == 'w' ? -1 : 1);
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read fix info from arduino message!");
	}
	break;

      case ARDUINO_PROTOCOL_SPEED:
	float speed;
	if (sscanf(data->data, SPEED_FORMAT, &gps_id, &speed) == 2) {
	  m_gpsdata.gps[gps_id].speed = speed;
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read speed from arduino message!");
	}
	break;

      case ARDUINO_PROTOCOL_ANGLE:
	float angle;
	if (sscanf(data->data, ANGLE_FORMAT, &gps_id, &angle) == 2) {
	  m_gpsdata.gps[gps_id].angle = angle;
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read angle from arduino message!");
	}
	break;

      case ARDUINO_PROTOCOL_ELEV:
	float elevation;
	if (sscanf(data->data, ELEVATION_FORMAT, &gps_id, &elevation) 
	    == 2) {
	  m_gpsdata.gps[gps_id].alititude = elevation;
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read elevation from arduino message!");
	}
	break;

      case ARDUINO_PROTOCOL_SAT:
	int numSatellites;
	if (sscanf(data->data, NUMSATELLITES_FORMAT, 
		   &gps_id, &numSatellites) == 2) {
	  m_gpsdata.gps[gps_id].numSatellites = numSatellites;
	  m_dataArrived=true;
	} else {
	  Logger::log(m_moduleid,LOGGER_WARNING,
		      "Failed to read number of satellites from"
		      " arduino message!");
	}
	break;
	
      default:
	Logger::log(m_moduleid,LOGGER_WARNING,
		    "Protocol Error! Control Character doesn't match expected chars.");
      }
    } else {
      Logger::log(m_moduleid, LOGGER_WARNING, 
		  "Arduino module sent data with wrong tag?");	
    }
  }
}

const char* gpsmodule::myName="GPS Module";
