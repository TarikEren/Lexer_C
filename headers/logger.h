#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define COLOR_INFO_SEQ "\x1b[34m"
#define COLOR_WARN_SEQ "\x1b[33m"
#define COLOR_ERROR_SEQ "\x1b[31m"
#define COLOR_DEFAULT_SEQ "\x1b[37m"
#define COLOR_RESET_SEQ "\x1b[0m"

typedef enum LogLevel {
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
} LogLevel;

void Log(const char* message, LogLevel level);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
