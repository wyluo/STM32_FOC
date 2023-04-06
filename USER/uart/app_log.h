#ifndef APP_LOG_H
#define APP_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#define DEBUG 1
#define MAIN_LOG_SWITCH 1
#define TIM_LOG_SWITCH 1

/* log level */
#define ELOG_LVL_ASSERT  0
#define ELOG_LVL_ERROR   1
#define ELOG_LVL_WARN    2
#define ELOG_LVL_INFO    3
#define ELOG_LVL_DEBUG   4
#define ELOG_LVL_VERBOSE 5

/* set log level */
#define ELOG_OUTPUT_LVL ELOG_LVL_VERBOSE

/* Assert */
#define LOG_A(args, ...) \
			do \
			{ \
				if(ELOG_OUTPUT_LVL >= ELOG_LVL_ASSERT) \
				{ \
					printf("[A/%s Line:%.4d]" args "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
				} \
			}while(0)
			
/* Error */
#define LOG_E(args, ...) \
			do \
			{ \
				if(ELOG_OUTPUT_LVL >= ELOG_LVL_ERROR) \
				{ \
					printf("[A/%s Line:%.4d]" args "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
				} \
			}while(0)
			
/* Warn */
#define LOG_W(args, ...) \
			do \
			{ \
				if(ELOG_OUTPUT_LVL >= ELOG_LVL_WARN) \
				{ \
					printf("[A/%s Line:%.4d]" args "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
				} \
			}while(0)

/* Info */
#define LOG_I(args, ...) \
			do \
			{ \
				if(ELOG_OUTPUT_LVL >= ELOG_LVL_INFO) \
				{ \
					printf("[A/%s Line:%.4d]" args "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
				} \
			}while(0)

/* Debug */
#define LOG_D(args, ...) \
			do \
			{ \
				if(ELOG_OUTPUT_LVL >= ELOG_LVL_DEBUG) \
				{ \
					printf("[A/%s Line:%.4d]" args "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
				} \
			}while(0)

/* Verbose */
#define LOG_V(args, ...) \
			do \
			{ \
				if(ELOG_OUTPUT_LVL >= ELOG_LVL_VERBOSE) \
				{ \
					printf("[A/%s Line:%.4d]" args "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
				} \
			}while(0)

#if DEBUG
#define DBG_PRINTF(fmt, args...) \
			do \
			{ \
				printf("<<Files:%s  Line:%d  Function:%s>>", __FILE__, __LINE__, __FUNCTION__); \
				printf(fmt, ##args); \
			}while(0)
#else
			#define DBG_PRINTF(fmt, args...)
#endif
#endif

void Easylogger_Init(void);

			
#ifdef __cplusplus
		}
#endif

#endif
