#include <cstdarg>


#define LOGGER_WARNING	 	0
#define LOGGER_ERROR 		1
#define LOGGER_INFO 		2
namespace Logger{
	void initialize(char* cfgfile);
	void log(int moduleID, int level, const char* format, ...);
};
