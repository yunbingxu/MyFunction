/****************** Copyright (c) 2019 南京大全电气研究院有限公司*****************
 * 程序名称：
 * 版本号：1.0
 * 功  能：日志管理模块，生成历史日志文件
 * 开发人：txx
 * 开发时间：2019.12.16
 * 修改者：
 * 修改时间：
 * 修改简要说明：参考开源easylogger做简化
 * 其他：win32下依赖pthread
***********************************************************************************/
#include "usr_public.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <sys/stat.h>

#if defined(_WIN32)
#    include <Windows.h>
#    include <direct.h>
#    include <io.h>
#else
#    include <execinfo.h>
#    include <unistd.h>
#endif

#ifdef _WIN32
#    define snprintf _snprintf
#else
#    define SLOG_COLOR_ENABLE // enalbe print color. support on linux/unix platform
#endif

#define SLOG_PRINT_ENABLE  // enalbe print output to console fd=stdout

#define SLOG_FILE_MAX_SIZE (512 * 1024) //max size of logfile
#define SLOG_FILE_MAX_ROTATE 5          //rotate file max num

#define MAX_LEVEL_STR (10)
#define MAX_DATE_STR (10)
#define DATE_STR_FMT "%04d%02d%02d"
#define MAX_TIME_STR (20)
#define TIME_STR_FMT "%04d/%02d/%02d %02d:%02d:%02d"
#define MAX_FILE_PATH (260)
#define MAX_LOG_LINE (10 * 1024)

#ifdef SLOG_COLOR_ENABLE
/**
 * CSI(Control Sequence Introducer/Initiator) sign
 * more information on https://en.wikipedia.org/wiki/ANSI_escape_code
 */
#define CSI_START "\033["
#define CSI_END "\033[0m"
/* output log front color */
#define F_BLACK "30;"
#define F_RED "31;"
#define F_GREEN "32;"
#define F_YELLOW "33;"
#define F_BLUE "34;"
#define F_MAGENTA "35;"
#define F_CYAN "36;"
#define F_WHITE "37;"
/* output log background color */
#define B_NULL
#define B_BLACK "40;"
#define B_RED "41;"
#define B_GREEN "42;"
#define B_YELLOW "43;"
#define B_BLUE "44;"
#define B_MAGENTA "45;"
#define B_CYAN "46;"
#define B_WHITE "47;"
/* output log fonts style */
#define S_BOLD "1m"
#define S_UNDERLINE "4m"
#define S_BLINK "5m"
#define S_NORMAL "22m"

/*---------------------------------------------------------------------------*/
/* enable log color */

/* change the some level logs to not default color if you want */
//#define SLOG_COLOR_ASSERT							 F_MAGENTA B_NULL S_NORMAL
#define SLOG_COLOR_ERROR F_RED B_NULL S_NORMAL
#define SLOG_COLOR_WARN F_YELLOW B_NULL S_NORMAL
#define SLOG_COLOR_INFO F_CYAN B_NULL S_NORMAL
#define SLOG_COLOR_DEBUG F_GREEN B_NULL S_NORMAL
#define SLOG_COLOR_TRACE F_BLUE B_NULL S_NORMAL

#endif // SLOG_COLOR_ENABLE

#define SLOG_MUTEX pthread_mutex_t

typedef struct _logger_cfg
{
    char file_path[128];     /* file path */
    char file_name[64];      /* file name */
    FILE *log_file;          /* log file pointer */
    SLOG_MUTEX mtx;          /* mutex for safety */
    slogLevel filter_level; /* output levle control*/
    int inited;              /* initial flag*/
    size_t max_size;         /* file max size */
    int max_rotate;          /* max rotate file count */
} logger_cfg;

static logger_cfg g_logger_cfg =
#ifdef _WIN32
    {{0}, {0}, NULL, {0}, S_INFO, FALSE, SLOG_FILE_MAX_SIZE, SLOG_FILE_MAX_ROTATE};
#else
    {{0}, {0}, NULL, {{0}}, S_INFO, FALSE, SLOG_FILE_MAX_SIZE, SLOG_FILE_MAX_ROTATE};
#endif

static void _slog_init_mutex(SLOG_MUTEX *mtx)
{
    pthread_mutex_init(mtx, NULL);
}

static void _slog_lock(SLOG_MUTEX *mtx)
{
    pthread_mutex_lock(mtx);
}

static void _slog_unlock(SLOG_MUTEX *mtx)
{
    pthread_mutex_unlock(mtx);
}

static char *_slog_get_process_info(void)
{
    static char cur_process_info[10] = {0};
#ifdef _WIN32
    snprintf(cur_process_info, 10, "pid:%04ld", GetCurrentProcessId());
#else
    snprintf(cur_process_info, 10, "pid:%04d", getpid());
#endif
    return cur_process_info;
}

static char *_slog_get_thread_info(void)
{
    static char cur_thread_info[10] = {0};
#ifdef _WIN32
    snprintf(cur_thread_info, 10, "tid:%04ld", GetCurrentThreadId());
#else
    snprintf(cur_thread_info, 10, "tid:%04ld", pthread_self());
#endif
    return cur_thread_info;
}


static void _get_curr_time(int timestr_size, char timestr[])
{
    time_t tt = {0};
    struct tm *curr_time = NULL;

    time(&tt);
    curr_time = localtime(&tt);
    snprintf(timestr, timestr_size, TIME_STR_FMT,
             curr_time->tm_year + 1900, curr_time->tm_mon + 1, curr_time->tm_mday,
             curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
}

static char* _get_level_str(slogLevel level)
{
    switch (level)
    {
    case S_TRACE:
        return "[TRACE]";
    case S_DEBUG:
        return "[DEBUG]";
    case S_INFO:
        return "[INFO ]";
    case S_WARN:
        return "[WARN ]";
    case S_ERROR:
        return "[ERROR]";
    default:
        return "[     ]";
    }
}

static int _slog_mkdir(const char *log_dir)
{
#if defined(WIN32)
    if (_mkdir(log_dir) != 0)
    {
        return FALSE;
    }
#else
    if (mkdir(log_dir, 0744) != 0)
    {
        return FALSE;
    }
#endif
    return TRUE;
}

static void _slog_file_rotate(void)
{
#define SUFFIX_LEN 10
    /* mv xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0 */
    int n;
    char oldpath[256] = {0}, newpath[256] = {0};
    size_t base = 0;
    snprintf(oldpath, sizeof(oldpath) - 1, "%s/%s", g_logger_cfg.file_path, g_logger_cfg.file_name);
    base = strlen(oldpath);
    strncpy(newpath, oldpath, base);
    for (n = g_logger_cfg.max_rotate - 1; n >= 0; --n)
    {
        snprintf(oldpath + base, SUFFIX_LEN, n ? ".%d" : "", n - 1);
        snprintf(newpath + base, SUFFIX_LEN, ".%d", n);
        remove(newpath);
        rename(oldpath, newpath);
    }
}
/**************************external function *******************************/
//logdir :log file path
//filename：log file name
//level：log output level
int SLOG_init(const char* log_dir, const char* file_name, slogLevel level)
{
    char log_filepath[MAX_FILE_PATH] = {0};

    if (log_dir == NULL || file_name == NULL) {
        return FALSE;
    }
    g_logger_cfg.filter_level = level;
    if (TRUE == g_logger_cfg.inited) {
        return TRUE;
    }
    if (access(log_dir, 0) != 0) {
        if (_slog_mkdir(log_dir) != TRUE) {
            return FALSE;
        }
    }
    _slog_init_mutex(&g_logger_cfg.mtx);
    snprintf(g_logger_cfg.file_path, sizeof(g_logger_cfg.file_path) - 1, "%s", log_dir);
    snprintf(g_logger_cfg.file_name, sizeof(g_logger_cfg.file_name) - 1, "%s", file_name);
    snprintf(log_filepath, sizeof(log_filepath) - 1, "%s/%s", log_dir, g_logger_cfg.file_name);
    g_logger_cfg.log_file = fopen(log_filepath, "a+");
    if (NULL == g_logger_cfg.log_file) {
        return FALSE;
    }
    g_logger_cfg.inited = TRUE;

    return TRUE;
}

void write_log(slogLevel level, int braw, const char *func_name, int line, const char *fmt, ...)
{
    va_list     args;
    char*       level_str                 = NULL;
    char*       process_info              = NULL;
    char*       thread_info               = NULL;
    char        timestr[MAX_TIME_STR]     = {0};
    static char log_content[MAX_LOG_LINE] = {0};
    static char log_line[MAX_LOG_LINE]    = {0};

    if (g_logger_cfg.filter_level > level || g_logger_cfg.inited != TRUE) {
        return;
    }

    _slog_lock(&g_logger_cfg.mtx);

    va_start(args, fmt);
    vsnprintf(log_content, sizeof(log_content) - 1, fmt, args);
    va_end(args);

// console print enable
#ifdef SLOG_PRINT_ENABLE
// console print with color
#    ifdef SLOG_COLOR_ENABLE
    switch (level) {
        case S_TRACE: printf(CSI_START SLOG_COLOR_TRACE "%s" CSI_END, log_content); break;
        case S_DEBUG: printf(CSI_START SLOG_COLOR_DEBUG "%s" CSI_END, log_content); break;
        case S_INFO:  printf(CSI_START SLOG_COLOR_INFO  "%s" CSI_END, log_content); break;
        case S_WARN:  printf(CSI_START SLOG_COLOR_WARN  "%s" CSI_END, log_content); break;
        case S_ERROR: printf(CSI_START SLOG_COLOR_ERROR "%s" CSI_END, log_content); break;
        default: printf("%s", log_content);
    }
#    else
    printf("%s", log_content);
#    endif  // SLOG_COLOR_ENABLE
#endif      // SLOG_PRINT_ENABLE

    if (!braw) {
        _get_curr_time(sizeof(timestr), timestr); // time
        level_str = _get_level_str(level); // log level
        process_info = _slog_get_process_info();  // pid
        thread_info  = _slog_get_thread_info();   // tid

        // log format for different level please modify below
        switch (level) {
            case S_TRACE:
            case S_DEBUG:
            case S_INFO:
                snprintf(log_line,
                         sizeof(log_line) - 1,
                         "%s %s | %s\n",
                         level_str,
                         timestr,
                         log_content);
                break;
            case S_WARN:
            case S_ERROR:
                snprintf(log_line,
                         sizeof(log_line) - 1,
                         "%s %s- %s:%d | %s\n",
                         level_str,
                         timestr,
                         func_name,
                         line,
                         log_content);
                break;
            default:
                snprintf(log_line,
                         sizeof(log_line) - 1,
                         "%s %s [%s %s]- %s:%d | %s\n",
                         level_str,
                         timestr,
                         process_info,
                         thread_info,
                         func_name,
                         line,
                         log_content);
        }
    }
    else {
        snprintf(log_line, sizeof(log_line) - 1, "%s", log_content);  // output raw data
    }

    //log_file_rotate_check
    struct stat statbuf;
    statbuf.st_size = 0;
    if (g_logger_cfg.log_file) {
        fstat(fileno(g_logger_cfg.log_file), &statbuf);
    }
    if (statbuf.st_size >= g_logger_cfg.max_size) {
        fclose(g_logger_cfg.log_file);
        g_logger_cfg.log_file = NULL;
        /* rotate the log file */
        _slog_file_rotate();
    }
    // reopen the log file
    if (g_logger_cfg.log_file == NULL) {
        char full_file_name[256] = {0};
        snprintf(full_file_name,
                 sizeof(full_file_name) - 1,
                 "%s/%s",
                 g_logger_cfg.file_path,
                 g_logger_cfg.file_name);
        g_logger_cfg.log_file = fopen(full_file_name, "a+");
    }

    if (g_logger_cfg.log_file) {
        fwrite(log_line, sizeof(char), strlen(log_line), g_logger_cfg.log_file);
        fflush(g_logger_cfg.log_file);
    }
    _slog_unlock(&g_logger_cfg.mtx);
}
