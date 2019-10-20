#include "NetworkLog.h"

void NetworkLog(const char* file, int line, const char* function, const char* format, ...)
{
	wchar_t* s = nullptr;

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&s),
		0,
		NULL);

	Log(file, line, function, format);

	LocalFree(s);
}
