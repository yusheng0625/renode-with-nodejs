#include "log.h"
#include <ctime>
#include <stdio.h>
#include <cstdarg>

int Log::_log_level = Log::Levels::log_trace;
bool Log::_enable_log_position = true;
bool Log::_enable_log_color = true;
bool Log::_about_to_exit = false;

const char Log::_log_text[][20] = { "NEVER", "FATAL", "ERROR", "WARN", "INFO",
		"DEBUG", "TRACE", "" };
const char Log::_log_color[][20] = { RED, RED, RED, YEL, GRN, MAG, "" };

void Log::log0(const char *file, const char *function, int line, int level,
		const char *str, ...) {

	if (level > _log_level)
		return;
	if (level > Log::Levels::log_trace || level < 0)
		return;

	time_t timer;
	char buffer[100];
	struct tm *tm_info;

	time(&timer);
	tm_info = localtime(&timer);

	if (_enable_log_color)
		printf("%s", _log_color[level]);

	strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", tm_info);
	printf("[%s][%s]", buffer, _log_text[level]);

	if (_enable_log_position)
		printf("[%s,func:%s,line:%d]", file, function, line);

	va_list vlist;
	va_start(vlist, str);
	vfprintf(stdout, str, vlist);
	va_end(vlist);
	if (_enable_log_color)
		printf("%s", RESET);

	//printf("\n");
	//if(enable_log_color)
	//printf(log_color[level]);
	fflush (stdout);

	if (_log_level == log_fatal) {
		_about_to_exit = true;
	}
}

void Log::log_bare(int level, const char *str, ...) {
	if (level > _log_level)
		return;
	if (level > Log::Levels::log_trace || level < 0)
		return;
	if (_enable_log_color)
		printf("%s", _log_color[level]);
	va_list vlist;
	va_start(vlist, str);
	vfprintf(stdout, str, vlist);
	va_end(vlist);
	if (_enable_log_color)
		printf("%s", RESET);
	printf("\n");
	fflush (stdout);
}
