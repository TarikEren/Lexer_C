#include "../headers/logger.h"
#include <stdio.h>

/*
 * \brief       Logs the provided message with the related color to the provided log level
 * \param[in]   message: Message to log
 * \param[in]   level: The log level
*/
void Log(const char *message, LogLevel level) {
    char* COLOR_SEQ;
    if (level == LOG_INFO)
        COLOR_SEQ = COLOR_INFO_SEQ;
    else if (level == LOG_WARN)
        COLOR_SEQ = COLOR_WARN_SEQ;
    else if (level == LOG_ERROR)
        COLOR_SEQ = COLOR_ERROR_SEQ;
    else {
        COLOR_SEQ = COLOR_DEFAULT_SEQ;
    }
    printf("%s%s%s\n", COLOR_SEQ, message, COLOR_RESET_SEQ);
}
