/*
 * log.h - header-only lightweight logger
 *
 * Features:
 *  - Clear log levels with threshold filtering
 *  - Portable pid / tid retrieval
 *  - Atomic, thread-safe output
 *  - Header-only, static inline implementation
 *  - Configurable components via macros
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
#  include <windows.h>
#  include <process.h>
#else
#  include <unistd.h>
#  include <pthread.h>
#   include <sys/syscall.h>
#endif

/* 配置：可在编译时通过 -D 覆盖 */
#ifndef LOG_OUT_FILE
#define LOG_OUT_FILE stderr
#endif

/* 默认日志阈值：低于该级别的日志不会输出 */
#ifndef LOG_LEVEL_THRESHOLD
#define LOG_LEVEL_THRESHOLD 2 /* INFO */
#endif

/* 级别 (从低到高) */
#define LOG_TRACE 0
#define LOG_DEBUG 1
#define LOG_INFO  2
#define LOG_WARN  3
#define LOG_ERROR 4
#define LOG_FATAL 5

/* 可选输出开关，1 为启用 */
#ifndef LOG_ENABLE_TIME
#define LOG_ENABLE_TIME 1
#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S "
#define LOG_TIME_BUFSZ 32
#endif
#ifndef LOG_ENABLE_PID
#define LOG_ENABLE_PID 1
#define LOG_PID_FORMAT "[%lu:%llu] "
#endif
#ifndef LOG_ENABLE_LOCATION
#define LOG_ENABLE_LOCATION 1
#define LOG_LOCATION_FORMAT "[%s:%03d] "
#endif
#ifndef LOG_ENABLE_FUNCTION
#define LOG_ENABLE_FUNCTION 1
#define LOG_FUNCTION_FORMAT "%s() "
#endif
#ifndef LOG_ENABLE_LEVEL
#define LOG_ENABLE_LEVEL 1
#define LOG_LEVEL_FORMAT "[%-5s] "
#endif
#ifndef LOG_ENABLE_ENDSTR
#define LOG_ENABLE_ENDSTR 1
#define LOG_ENDSTR_FORMAT ": "
#endif

#define LOGDEF

/* Helper: level -> string */
static inline const char *log_level_to_string(int lvl)
{
    switch (lvl) {
    case LOG_TRACE: return "TRACE";
    case LOG_DEBUG: return "DEBUG";
    case LOG_INFO:  return "INFO";
    case LOG_WARN:  return "WARN";
    case LOG_ERROR: return "ERROR";
    case LOG_FATAL: return "FATAL";
    default: return "UNK";
    }
}

#if defined(_MSC_VER)
#  include <sal.h>
#  define LOG_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) \
     _Printf_format_string_
#elif defined(__GNUC__) || defined(__clang__)
#  define LOG_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) \
     __attribute__((format(printf, STRING_INDEX, FIRST_TO_CHECK)))
#else
#  define LOG_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

LOGDEF void log_printf(const char *format, ...) LOG_PRINTF_FORMAT(1, 2);
LOGDEF unsigned long log_get_pid(void);
LOGDEF unsigned long long log_get_tid(void);
LOGDEF void log_print_time(void);

#if LOG_ENABLE_TIME
#define LOG_PRINT_TIME() log_print_time()
#else
#define LOG_PRINT_TIME() ((void)0)
#endif

#if LOG_ENABLE_PID
#   define LOG_PRINT_PID() \
        log_printf(LOG_PID_FORMAT, log_get_pid(), log_get_tid())
#else
#   define LOG_PRINT_PID() ((void)0)
#endif

#if LOG_ENABLE_LOCATION
#   define LOG_PRINT_LOCATION() \
        log_printf(LOG_LOCATION_FORMAT, __FILE__, __LINE__)
#else
#   define LOG_PRINT_LOCATION() ((void)0)
#endif

#if LOG_ENABLE_FUNCTION
#   define LOG_PRINT_FUNCTION() \
        log_printf(LOG_FUNCTION_FORMAT, __func__)
#else
#   define LOG_PRINT_FUNCTION() ((void)0)
#endif

#if LOG_ENABLE_LEVEL
#   define LOG_PRINT_LEVEL(level_str) \
        log_printf(LOG_LEVEL_FORMAT, level_str)
#else
#   define LOG_PRINT_LEVEL(level) ((void)0)
#endif

#if LOG_ENABLE_ENDSTR
#   define LOG_PRINT_END() \
        log_printf(LOG_ENDSTR_FORMAT)
#else
#   define LOG_PRINT_END() ((void)0)
#endif

LOGDEF void log_lock(void);
LOGDEF void log_unlock(void);


//  TODO: implement change the file, line, and level position
//  TODO: implement color output
//  TODO: Output the log message with all enabled components
//  TODO: Support variadic arguments for formatted output
//  TODO: Optimize performance
//  TODO: provide buf to write all at once
#define LOG_OUTPUT(level, fmt, ...)         \
    do {                                    \
        log_lock();                         \
        LOG_PRINT_TIME();                   \
        LOG_PRINT_PID();                    \
        LOG_PRINT_LOCATION();               \
        LOG_PRINT_FUNCTION();               \
        LOG_PRINT_LEVEL(level);             \
        LOG_PRINT_END();                    \
        log_printf(fmt, ##__VA_ARGS__);     \
        log_printf("\n");                   \
        log_unlock();                       \
    } while (0)

#define LOG(fmt, ...) LOG_L(LOG_INFO, fmt, ##__VA_ARGS__)

#define LOG_L(level, fmt, ...)                          \
    do {                                                \
        int _lvl = (level);                             \
        if (_lvl < LOG_LEVEL_THRESHOLD) break;          \
        LOG_OUTPUT(log_level_to_string(_lvl), fmt,      \
                   ##__VA_ARGS__);                      \
        if(level == LOG_FATAL) {abort();}               \
    } while (0)

#if defined(__GNUC__) || defined(__clang__)
#  define LOG_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#  define LOG_NORETURN __declspec(noreturn)
#else
#  define LOG_NORETURN
#endif
LOGDEF LOG_NORETURN void log_abort(void);
#define LOG_ASSERT(cond)  \
    do {                            \
        if (!(cond)) {              \
            log_lock();             \
            log_print_time();       \
            log_printf(LOG_PID_FORMAT LOG_LOCATION_FORMAT LOG_FUNCTION_FORMAT "\n" \
                "Assertion failed:\n\t" "%s is false", \
                log_get_pid(), log_get_tid(),                   \
                __FILE__, __LINE__, __func__, #cond);   \
            log_abort();                                        \
        } \
    } while (0)

#ifdef LOG_DISABLE
#  define LOG(...)        ((void)0)
#  define LOG_L(...)      ((void)0)
#  define LOG_ASSERT(...) ((void)0)
#endif

#ifdef LOG_IMPLEMENTATION
LOGDEF LOG_NORETURN void log_abort(void)
{
    abort();
}

LOGDEF unsigned long log_get_pid(void)
{
#if defined(_WIN32)
    return (unsigned long)GetCurrentProcessId();
#elif defined(__unix__) || defined(__APPLE__)
    return (unsigned long)getpid();
#else
    return 0;
#endif
}

LOGDEF unsigned long long log_get_tid(void)
{
#if defined(_WIN32)
    return (unsigned long long)GetCurrentThreadId();
#elif defined(__linux__)
    return (unsigned long long)((unsigned long)pthread_self());
#elif defined(__APPLE__)
    uint64_t tid = 0;
    pthread_threadid_np(NULL, &tid);
    return (unsigned long long)tid;
#else
    return (unsigned long long)((unsigned long)pthread_self());
#endif
}

LOGDEF void log_print_time(void)
{
    char buf[LOG_TIME_BUFSZ];
    time_t t = time(NULL);
    struct tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    strftime(buf, sizeof(buf), LOG_TIME_FORMAT, &tm);
    log_printf("%s", buf);
}

#if defined(_WIN32)
static CRITICAL_SECTION g_log_lock;
static int g_log_lock_inited = 0;
LOGDEF void log_lock(void)
{
    if (!g_log_lock_inited) {
        InitializeCriticalSection(&g_log_lock);
        g_log_lock_inited = 1;
    }
    EnterCriticalSection(&g_log_lock);
}
LOGDEF void log_unlock(void)
{
    LeaveCriticalSection(&g_log_lock);
}
#else
static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;
LOGDEF void log_lock(void)
{
    pthread_mutex_lock(&g_log_lock);
}
LOGDEF void log_unlock(void)
{
    pthread_mutex_unlock(&g_log_lock);
}
#endif

//  TODO: instead of multiple calls to log_printf, use a single call with a buffer
LOGDEF void log_printf(const char *format, ...) 
{
    va_list args;
    va_start(args, format);
    vfprintf(LOG_OUT_FILE, format, args);
    va_end(args);
}

#endif /* LOG_IMPLEMENTATION */

#endif /* LOG_H */