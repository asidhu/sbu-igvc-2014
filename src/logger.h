


#define LOGGER_WARNING	 	0
#define LOGGER_ERROR 		1
#define LOGGER_INFO 		2
namespace Logger{
	void log(int moduleID, int level, const char* buff, int len);
	void log(int moduleID, int level, const char* nulterm);

};
