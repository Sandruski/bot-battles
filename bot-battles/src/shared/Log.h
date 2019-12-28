#ifndef __LOG_H__
#define __LOG_H__

#ifdef _DEBUG
#define LOG(type, format, ...) sand::Log(__FILE__, __LINE__, __FUNCTION__, type, format, __VA_ARGS__)
#else
#define LOG(type, format, ...) void(0)
#endif
#define ILOG(format, ...) LOG(sand::LogTypes::ILOG, format, __VA_ARGS__)
#define WLOG(format, ...) LOG(sand::LogTypes::WLOG, format, __VA_ARGS__)
#define ELOG(format, ...) LOG(sand::LogTypes::ELOG, format, __VA_ARGS__)

#define NETLOG(format, ...) sand::NetLog(__FILE__, __LINE__, __FUNCTION__, format, __VA_ARGS__)

namespace sand {

enum class LogTypes {

    ILOG,
    WLOG,
    ELOG
};

void Log(const char* file, int line, const char* function, LogTypes type, const char* format, ...);
void NetLog(const char* file, int line, const char* function, const char* format, ...);
}

#endif
