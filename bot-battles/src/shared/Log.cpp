#include "Log.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	void Log(const char* file, int line, const char* function, LogTypes type, const char* format, ...)
	{
		static char tmp_string[4096];
		static char tmp_string2[4096];
		static va_list ap;

		va_start(ap, format);
		vsprintf_s(tmp_string, 4096, format, ap);
		va_end(ap);
		switch (type)
		{
		case LogTypes::ILOG:
		{
			sprintf_s(tmp_string2, 4096, "%s(%d)%s [INFO]: %s\n", file, line, function, tmp_string);
			break;
		}
		case LogTypes::WLOG:
		{
			sprintf_s(tmp_string2, 4096, "%s(%d)%s [WARNING]: %s\n", file, line, function, tmp_string);
			break;
		}
		case LogTypes::ELOG:
		{
			sprintf_s(tmp_string2, 4096, "%s(%d)%s [ERROR]: %s\n", file, line, function, tmp_string);
			break;
		}
		}
		OutputDebugString(tmp_string2);
	}

	//----------------------------------------------------------------------------------------------------
	void NetLog(const char* file, int line, const char* function, const char* format, ...)
	{
		wchar_t* s = nullptr;

		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPWSTR>(&s),
			0,
			NULL);

		Log(file, line, function, LogTypes::ELOG, format);

		LocalFree(s);
	}
}
