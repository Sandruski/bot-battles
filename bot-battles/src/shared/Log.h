#ifndef __LOG_H__
#define __LOG_H__

#define LOG(format, ...) Log(__FILE__, __LINE__, __FUNCTION__, format, __VA_ARGS__)

void Log(const char* file, int line, const char* function, const char* format, ...);

#endif
