#ifndef YVENT_LOGGING_H
#define YVENT_LOGGING_H

#include <stdio.h>

#define LOG_TRACE(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_DEBUG(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_INFO(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_WARN(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_ERROR(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_FATAL(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_SYSERR(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)
#define LOG_SYSFATAL(...) do{ printf("%s(%d)<%s>:",__FILE__,__LINE__,__FUNCTION__);printf(__VA_ARGS__);printf("\n");}while(0)

#endif //YVENT_LOGGING