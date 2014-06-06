#include <cstdarg>
#include <vector>

#define LOGGER_WARNING	 	0
#define LOGGER_ERROR 		1
#define LOGGER_INFO 		2
class module;
namespace Logger{
	void initialize(char* cfgfile,std::vector<module*>*);
	void log(int moduleID, int level, const char* format, ...);
};
