#ifndef __NETWORK_LOG_H__
#define __NETWORK_LOG_H__

#define NETWORK_LOG(format, ...) NetworkLog(__FILE__, __LINE__, __FUNCTION__, format, __VA_ARGS__)

void NetworkLog(const char* file, int line, const char* function, const char* format, ...);

#endif
