#ifndef LOG_H
#define LOG_H

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

class Log {
public:
	typedef enum {
		log_never = 0,
		log_fatal,
		log_error,
		log_warn,
		log_info,
		log_debug,
		log_trace,
		log_end
	} Levels;

	static void log0(const char *file, const char *function, int line,
			int level, const char *str, ...);
	static void log_bare(int level, const char *str, ...);

	static int _log_level;
	static bool _enable_log_position;
	static bool _enable_log_color;
	static bool _about_to_exit;

	static const char _log_text[][20]; //= { "NEVER","FATAL","ERROR","WARN","INFO","DEBUG","TRACE","" };
	static const char _log_color[][20]; //= { RED,RED,RED,YEL,GRN,MAG,"" };
};

#endif
