#ifndef _S_LOG_H_
#define _S_LOG_H_

#ifndef NULL
#define NULL         (0)
#endif

#ifndef TRUE
#define TRUE         (1)
#endif

#ifndef FALSE        
#define FALSE        (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _slog_level {
    S_TRACE = 1,
    S_DEBUG = 2,
    S_INFO  = 3,
    S_WARN  = 4,
    S_ERROR = 5
} slog_level;

//log必须先初始化，设置log目录、文件名称以及日志级别
//注意：目录目前只能自动新建一级文件夹！！！
int slog_init(const char* log_dir, const char* file_name, slog_level level);

//代码中可以利用下面的宏来替换printf输出
//注意：接口一次输出的内容最大长度约为10KB，超出长度会发生截断。通过MAX_LOG_LINE宏可修改最大长度。
void write_log(slog_level level, int braw, const char *func_name, int line, const char *fmt, ...);

#define SLOG_ERROR(fmt, ...)     write_log(S_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN(fmt, ...)      write_log(S_WARN,  0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO(fmt, ...)      write_log(S_INFO,  0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG(fmt, ...)     write_log(S_DEBUG, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE(fmt, ...)     write_log(S_TRACE, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_ERROR(fmt, ...)     write_log(S_ERROR, 0, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define SLOG_ERROR_RAW(fmt, ...) write_log(S_ERROR, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_WARN_RAW(fmt, ...)  write_log(S_WARN,  1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_INFO_RAW(fmt, ...)  write_log(S_INFO,  1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_DEBUG_RAW(fmt, ...) write_log(S_DEBUG, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define SLOG_TRACE_RAW(fmt, ...) write_log(S_TRACE, 1, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

/* demo
 * slog_init("./log", "slog.log", S_TRACE);
 * SLOG_ERROR("SLOG ERROR TEST");
 * SLOG_WARN("SLOG WARN TEST");
**/

#ifdef __cplusplus
}
#endif

#endif // !_S_LOG_H_
