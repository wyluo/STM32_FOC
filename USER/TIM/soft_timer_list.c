#include "soft_timer_list.h"

/**
 * sfor_timer_list.c
 * ����ʵ�ֵ������ʱ����
 */
#define NULL ((void *)0)
	
typedef enum {FALSE = 0, TRUE = !FALSE} BOOL;

/**
 * �����ʱ���ڲ�����
 */
static SoftTimer *head_point = NULL;

static struct SoftTimer *creat_node(SoftTimer *node);
static char delete_node(SoftTimer *node);
static BOOL is_node_already_creat(SoftTimer *node);