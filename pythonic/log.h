#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <ctime>
#include <cctype>

#define PYC_LEVEL_VERBOSE 0
#define PYC_LEVEL_DEBUG 1
#define PYC_LEVEL_INFO 2
#define PYC_LEVEL_ERR 3
#define PYC_LEVEL_FATAL 4

#define FILTER_LEVEL 0

#define _PYC_LOG(level, levelName) if (level >= CldLog::filterLevel) std::wcerr << "[" << levelName << "] " << __FUNCTION__ << "@[" << \
			CldLog::GetFilename(__FILE__).c_str() << ", " << __LINE__ << "] "

#define PYC_INFO _PYC_LOG(PYC_LEVEL_INFO, "INFO")

#define PYC_DEBUG _PYC_LOG(PYC_LEVEL_DEBUG, "DEBUG")

#define PYC_WARN _PYC_LOG(PYC_LEVEL_INFO, "WARN")
#define PYC_ERROR _PYC_LOG(PYC_LEVEL_ERR, "ERROR")
#define PYC_FATAL _PYC_LOG(PYC_LEVEL_FATAL, "FATAL")

#define LOG_EXPR(x) #x << L" = " << (x) 

namespace CldLog
{
	namespace FilterLevel
	{
		enum FilterLevel
		{
			Verbose = PYC_LEVEL_VERBOSE,
			Debug = PYC_LEVEL_DEBUG,
			Info = PYC_LEVEL_INFO,
			Err = PYC_LEVEL_ERR,
			Fatal = PYC_LEVEL_FATAL
		};
	}

	static inline FilterLevel::FilterLevel filterLevel = FilterLevel::Err;

	static inline void SetFilterLevel(FilterLevel::FilterLevel level)
	{
		filterLevel = level;
	}

	static inline std::string Now() {
		time_t now = time(0);
		char buf[1024];
		auto e = ctime_s(buf, sizeof(buf), &now);
		std::string date = buf;
		for (int i = date.length() - 1; i >= 0; i--) {
			if (std::isspace(date[i])) {
				date.pop_back();
			}
			else {
				return date;
			}
		}
	}

	static inline std::string GetFilename(std::string path) {
		int i;
		for (i = path.length() - 1; i >= 0; i--) {
			if (path[i] == '/' || path[i] == '\\') {
				break;
			}
		}
		return path.substr(i + 1);
	}
}