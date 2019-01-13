#include "Common/Core.h"
#include "Platform.h"
#include "Utility/String.h"
#include <stdarg.h>

#define MAX_LOG_MSG_SIZE 1024

#include <Windows.h>

static HANDLE s_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

// NOTE(Zero)
// When `display` is true, the buffer is flushed and `ch` is not displayed
// When log buffer gets full then it is automatically flushed
// This function is not Thread safe
inline void InternalPutCharToBuffer(char ch, b32 display = false)
{
	static utf8 s_LogBuffer[MAX_LOG_MSG_SIZE];
	static u32 s_LogBufferIndex;

	// Flush when true and not to display `ch`
	if (display)
	{
		DWORD written;
		WriteConsoleA(s_ConsoleHandle, s_LogBuffer, s_LogBufferIndex * sizeof(utf8), &written, 0);
		s_LogBufferIndex = 0;
		return;
	}

	// Need to flush because buffer is full
	if (s_LogBufferIndex == (MAX_LOG_MSG_SIZE - 2))
	{
		s_LogBuffer[s_LogBufferIndex + 1] = '\0';
		DWORD written;
		WriteConsoleA(s_ConsoleHandle, s_LogBuffer, s_LogBufferIndex * sizeof(utf8), &written, 0);
		s_LogBufferIndex = 0;
		return;
	}
	s_LogBuffer[s_LogBufferIndex++] = ch;
}

inline void InternalParseAndLogString(s8 string)
{
	for (i32 si = 0; string[si] != '\0'; ++si)
		InternalPutCharToBuffer(string[si]);
}

void a3_Log(s8 file, u32 line, a3::log_type type, s8 format, ...)
{
	switch (type)
	{
	case a3::LogTypeStatus:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_GREEN);
		InternalParseAndLogString("[STATUS]  ");
		break;
	}
	case a3::LogTypeWarn:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN);
		InternalParseAndLogString("[WARNING] ");
		break;
	}
	case a3::LogTypeError:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED);
		InternalParseAndLogString("[ERROR]   ");
		break;
	}
	case a3::LogTypeTrace:
	{
		SetConsoleTextAttribute(s_ConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		InternalParseAndLogString("[TRACE]   ");
		break;
	}
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	InternalParseAndLogString("[Time:");
	static utf8 temporaryBuffer[100] = {};
	a3Assert(a3::ParseU32(temporaryBuffer, 100, sysTime.wHour, 10) > 0);
	InternalParseAndLogString(temporaryBuffer);
	InternalParseAndLogString(":");
	a3Assert(a3::ParseU32(temporaryBuffer, 100, sysTime.wMinute, 10) > 0);
	InternalParseAndLogString(temporaryBuffer);
	InternalParseAndLogString(":");
	a3Assert(a3::ParseU32(temporaryBuffer, 100, sysTime.wSecond, 10) > 0);
	InternalParseAndLogString(temporaryBuffer);
	InternalParseAndLogString(":");
	a3Assert(a3::ParseU32(temporaryBuffer, 100, sysTime.wMilliseconds, 10) > 0);
	InternalParseAndLogString(temporaryBuffer);
	InternalParseAndLogString("] [Thread:");
	a3Assert(a3::ParseU32(temporaryBuffer, 100, GetCurrentThreadId(), 10) > 0);
	InternalParseAndLogString(temporaryBuffer);
	InternalParseAndLogString("] [File:");
	InternalParseAndLogString(file);
	InternalParseAndLogString("] [Line:");
	a3Assert(a3::ParseU32(temporaryBuffer, 100, line, 10));
	InternalParseAndLogString(temporaryBuffer);
	InternalParseAndLogString("]\n");

	va_list arg;
	utf8* traverser;
	va_start(arg, format);
	for (traverser = (char*)format; *traverser != '\0'; ++traverser)
	{
		if (*traverser == '{')
		{
			if (*(traverser + 2) == '}')
			{
				switch (*(traverser + 1))
				{
				case 'c': // character
				{
					InternalPutCharToBuffer(va_arg(arg, utf8));
					traverser += 2;
					break;
				}
				case 's': // string
				{
					InternalParseAndLogString(va_arg(arg, utf8*));
					traverser += 2;
					break;
				}
				case 'i': // integer
				{
					i32 num = va_arg(arg, i32);
					if (num < 0)
					{
						num = -num;
						InternalPutCharToBuffer('-');
					}
					u64 unum = (u64)num;
					a3Assert(a3::ParseU32(temporaryBuffer, 100, (u64)num, 10) > 0);
					InternalParseAndLogString(temporaryBuffer);
					traverser += 2;
					break;
				}
				case 'x': // integer to hex
				{
					a3Assert(a3::ParseU32(temporaryBuffer, 100, va_arg(arg, u32), 16) > 0);
					InternalParseAndLogString(temporaryBuffer);
					traverser += 2;
					InternalPutCharToBuffer('h');
					break;
				}
				case 'o': // integer to oct
				{
					a3Assert(a3::ParseU32(temporaryBuffer, 100, va_arg(arg, u32), 8) > 0);
					InternalParseAndLogString(temporaryBuffer);
					traverser += 2;
					InternalPutCharToBuffer('o');
					break;
				}
				case 'b': // integer to binary
				{
					a3Assert(a3::ParseU32(temporaryBuffer, 100, va_arg(arg, u32), 2) > 0);
					InternalParseAndLogString(temporaryBuffer);
					traverser += 2;
					InternalPutCharToBuffer('b');
					break;
				}
				case 'u': // unsigned integer
				{
					a3Assert(a3::ParseU32(temporaryBuffer, 100, va_arg(arg, u32), 10) > 0);
					InternalParseAndLogString(temporaryBuffer);
					traverser += 2;
					break;
				}
				case 'f': // floats
				{
					a3Assert(a3::ParseF32(temporaryBuffer, 100, (f32)va_arg(arg, f64)) > 0);
					InternalParseAndLogString(temporaryBuffer);
					traverser += 2;
					break;
				}
				default: // unknown
				{
					InternalPutCharToBuffer(*traverser);
					break;
				}
				}
			}
			else if (*(traverser + 3) == '}')
			{
				if (*(traverser + 1) == 'v')
				{
					switch (*(traverser + 2))
					{
					case '2': // v2
					{
						v2 vec = va_arg(arg, v2);
						InternalPutCharToBuffer('(');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.x) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(',');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.y) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(')');
						traverser += 3;
						traverser += 3;
						break;
					}
					case '3': // v3
					{
						v3 vec = va_arg(arg, v3);
						InternalPutCharToBuffer('(');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.x) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(',');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.y) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(',');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.z) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(')');
						traverser += 3;
						break;
					}
					case '4': // v4
					{
						v4 vec = va_arg(arg, v4);
						InternalPutCharToBuffer('(');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.x) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(',');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.y) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(',');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.z) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(',');
						a3Assert(a3::ParseF32(temporaryBuffer, 100, vec.w) > 0);
						InternalParseAndLogString(temporaryBuffer);
						InternalPutCharToBuffer(')');
						traverser += 3;
						traverser += 3;
						break;
					}
					default:
					{
						InternalPutCharToBuffer(*traverser);
						break;
					}
					}
				}
				else
				{
					InternalPutCharToBuffer(*traverser);
				}
			}
			else
			{
				InternalPutCharToBuffer(*traverser);
			}
		}
		else
		{
			InternalPutCharToBuffer(*traverser);
		}
	}
	va_end(arg);
	InternalParseAndLogString("\n\n");
	InternalPutCharToBuffer(0, true);
}
