#include "app_log.h"
#include "config.h"
#include "elog.h"

/*
* 函数名称：Easylogger_Init
* 输入参数：None
* 返 回 值：None
* 作    者：Barry
* 功能描述：初始化Easylogger打印，配置log输出的颜色、字体、输出内容等
* 修改记录：None
*/
void Easylogger_Init(void)
{
#if(EASYLOGGER_ENABLE == 1)
    elog_init();
    elog_set_text_color_enabled(true);

    //输出所有内容
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);

    /* 输出日志级别信息和日志TAG */
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);

    /* 除了时间、进程信息、线程信息之外，其余全部输出 */
    elog_set_fmt(ELOG_LVL_DEBUG, \
                    ELOG_FMT_ALL & ~(ELOG_FMT_TIME | ELOG_FMT_P_INFO | ELOG_FMT_T_INFO));

    /* 输出所有内容 */
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL);

    /* 启动Easylogger */
    elog_start();

#endif
}

