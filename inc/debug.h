// printf style debugging with added file and line awareness
#define LogDebug(format, ...) log_to_file(__FILE__, __LINE__, format, ##__VA_ARGS__)

void log_to_file(char const *file, int line, char const *format, ...);
void debug_init();

