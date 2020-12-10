/*
* Change Logs:
* Date           Author       Notes
* 2020-01-27     zhangran     the first version
* 2020-01-29     zhangran     add assert for developer
* 2020-06-04     zhangran     modify delay_tick type
* 2020-11-30     zhangran     fix bug when ticks overflow
*/

#include "toolkit.h"
#ifdef TOOLKIT_USING_TIMER
typedef uint32_t (*tk_timer_get_tick_callback)(void);
static tk_timer_get_tick_callback tk_timer_get_tick = NULL;

#ifndef TK_TIMER_USING_CREATE
static struct tk_timer tk_timer_node;
#endif /* TK_TIMER_USING_CREATE */
static struct tk_timer *tk_timer_head_node = NULL;

/**
 * @brief ���Ҷ�ʱ������β�ڵ�(�ڲ�����)
 * 
 * @param tk_timer_start_node ��ʼ���ҵ��׽ڵ�
 * @return struct tk_timer* β�ڵ��ַ
 */
static struct tk_timer *_tk_timer_find_node_tail(struct tk_timer *tk_timer_start_node)
{
    TK_ASSERT(tk_timer_start_node);
    struct tk_timer *node_curr = tk_timer_start_node;
    struct tk_timer *node_next = tk_timer_start_node;
    do
    {
        node_curr = node_next;
        node_next = node_curr->next;
    } while (node_next != NULL);
    return node_curr;
}

/**
 * @brief ���붨ʱ��������(�ڲ�����)
 * 
 * @param tk_timer_node ��ʱ������
 * @return true ����ɹ�
 * @return false ����ʧ��
 */
static bool _tk_timer_insert_node_to_list(struct tk_timer *tk_timer_node)
{
    TK_ASSERT(tk_timer_node);
    struct tk_timer *node_tail = _tk_timer_find_node_tail(tk_timer_head_node);
    node_tail->next = tk_timer_node;
    tk_timer_node->prev = node_tail;
    return true;
}

/**
 * @brief �����ʱ�����ܳ�ʼ��
 * 
 * @param get_tick_func ��ȡϵͳtick�ص�����
 * @return true ��ʼ���ɹ�
 * @return false ��ʼʧ��
 */
bool tk_timer_func_init(uint32_t (*get_tick_func)(void))
{
    TK_ASSERT(get_tick_func);
#ifdef TK_TIMER_USING_CREATE
    struct tk_timer *tk_timer_node;
    tk_timer_node = (struct tk_timer *)malloc(sizeof(struct tk_timer));
    if (tk_timer_node == NULL)
        return false;
    tk_timer_head_node = tk_timer_node;
#else
    tk_timer_head_node = &tk_timer_node;
#endif /* TK_TIMER_USING_CREATE */
    tk_timer_head_node->prev = NULL;
    tk_timer_head_node->next = NULL;
    tk_timer_get_tick = get_tick_func;
    return true;
}

/**
 * @brief ��̬��ʼ����ʱ��
 * 
 * @param timer Ҫ��ʼ���Ķ�ʱ������
 * @param timeout_callback ��ʱ����ʱ�ص���������ʹ�ÿ�����ΪNULL
 * @return true ��ʼ���ɹ�
 * @return false ��ʼ��ʧ��
 */
bool tk_timer_init(struct tk_timer *timer, void (*timeout_callback)(struct tk_timer *timer))
{
    TK_ASSERT(tk_timer_head_node);
    TK_ASSERT(tk_timer_get_tick);
    TK_ASSERT(timer);
    if (tk_timer_head_node == NULL || tk_timer_get_tick == NULL)
        return false;
    timer->enable = false;
    timer->mode = TIMER_MODE_LOOP;
    timer->state = TIMER_STATE_STOP;
    timer->delay_tick = 0;
    timer->timer_tick_timeout = 0;
    timer->prev = NULL;
    timer->next = NULL;
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
    timer->timeout_callback = timeout_callback;
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
    bool result = _tk_timer_insert_node_to_list(timer);
    return result;
}

/**
 * @brief ��̬���붨ʱ��
 * 
 * @param timer Ҫ����Ķ�ʱ������
 * @return true ����ɹ�
 * @return false ����ʧ��
 */
bool tk_timer_detach(struct tk_timer *timer)
{
    TK_ASSERT(tk_timer_head_node);
    TK_ASSERT(timer);
    timer->prev->next = timer->next;
    if (timer->next != NULL)
        timer->next->prev = timer->prev;
    return true;
}

#ifdef TK_TIMER_USING_CREATE
/**
 * @brief ��̬������ʱ��
 * 
 * @param timeout_callback ��ʱ����ʱ�ص���������ʹ�ÿ�����ΪNULL
 * @return struct tk_timer* �����Ķ�ʱ������NULLΪ����ʧ��
 */
struct tk_timer *tk_timer_create(void (*timeout_callback)(struct tk_timer *timer))
{
    TK_ASSERT(tk_timer_head_node);
    TK_ASSERT(tk_timer_get_tick);
    struct tk_timer *timer;
    if (tk_timer_head_node == NULL || tk_timer_get_tick == NULL)
        return NULL;
    if ((timer = malloc(sizeof(struct tk_timer))) == NULL)
        return NULL;
    timer->enable = false;
    timer->mode = TIMER_MODE_LOOP;
    timer->state = TIMER_STATE_STOP;
    timer->delay_tick = 0;
    timer->timer_tick_timeout = 0;
    timer->prev = NULL;
    timer->next = NULL;
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
    timer->timeout_callback = timeout_callback;
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
    _tk_timer_insert_node_to_list(timer);
    return timer;
}

/**
 * @brief ��̬ɾ����ʱ��
 * 
 * @param timer Ҫɾ���Ķ�ʱ������
 * @return true ɾ���ɹ�
 * @return false ɾ��ʧ��
 */
bool tk_timer_delete(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    if (tk_timer_detach(timer) == true)
    {
        free(timer);
        return true;
    }
    return false;
}
#endif /* TK_TIMER_USING_CREATE */

/**
 * @brief ���ö�ʱ����������(�ڲ�����)
 * 
 * @param timer ��ʱ������
 * @return true �ɹ�
 * @return false ʧ��
 */
static bool _tk_timer_set_start_param(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    if (timer->delay_tick == 0 || tk_timer_get_tick == NULL)
        return false;
    timer->timer_tick_timeout = tk_timer_get_tick() + timer->delay_tick;
    timer->enable = true;
    timer->state = TIMER_STATE_RUNNING;
    return true;
}

/**
 * @brief ��ʱ������
 * 
 * @param timer Ҫ�����Ķ�ʱ������
 * @param mode ģʽ: ����TIMER_MODE_SINGLE; ѭ��TIMER_MODE_LOOP
 * @param delay_tick ��ʱ��ʱ��(��λtick)
 * @return true �����ɹ�
 * @return false ����ʧ��
 */
bool tk_timer_start(struct tk_timer *timer, tk_timer_mode mode, uint32_t delay_tick)
{
    TK_ASSERT(timer);
    TK_ASSERT(delay_tick);
    timer->mode = mode;
    timer->delay_tick = delay_tick;
    bool result = _tk_timer_set_start_param(timer);
    return result;
}

/**
 * @brief ��ʱ��ֹͣ
 * 
 * @param timer Ҫֹͣ�Ķ�ʱ������
 * @return true ֹͣ�ɹ�
 * @return false ֹͣʧ��
 */
bool tk_timer_stop(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    timer->enable = false;
    timer->state = TIMER_STATE_STOP;
    return true;
}

/**
 * @brief ��ʱ������
 * 
 * @param timer Ҫ�����Ķ�ʱ������
 * @return true �����ɹ�
 * @return false ����ʧ��
 */
bool tk_timer_continue(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    timer->enable = true;
    timer->state = TIMER_STATE_RUNNING;
    return true;
}

/**
 * @brief ��ʱ������
 * 
 * @param timer Ҫ�����Ķ�ʱ������
 * @return true �����ɹ�
 * @return false ����ʧ��
 */
bool tk_timer_restart(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    bool result = _tk_timer_set_start_param(timer);
    return result;
}

/**
 * @brief ��ȡ��ʱ��ģʽ
 * 
 * @param timer Ҫ��ȡ�Ķ�ʱ������
 * @return tk_timer_mode ��ʱ��ģʽ������TIMER_MODE_SINGLE; ѭ��TIMER_MODE_LOOP
 */
tk_timer_mode tk_timer_get_mode(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    return timer->mode;
}

/**
 * @brief ��ȡ��ʱ��״̬
 * 
 * @param timer Ҫ��ȡ�Ķ�ʱ������
 * @return tk_timer_state ��ʱ��״̬ 
 * TIMER_STATE_RUNNING��TIMER_STATE_STOP��TIMER_STATE_TIMEOUT
 */
tk_timer_state tk_timer_get_state(struct tk_timer *timer)
{
    TK_ASSERT(timer);
    return timer->state;
}

/**
 * @brief ��ʱ������
 * 
 * @return true ����
 * @return false �쳣
 */
bool tk_timer_loop_handler(void)
{
    struct tk_timer *timer = NULL;

    if (tk_timer_head_node != NULL)
        timer = tk_timer_head_node->next;
    else
        return false;

    while (timer != NULL)
    {
        if (timer->enable && (tk_timer_get_tick() - timer->timer_tick_timeout) < (UINT32_MAX / 2))
        {
            timer->enable = false;
            timer->state = TIMER_STATE_TIMEOUT;
#ifndef TK_TIMER_USING_INTERVAL
            if (timer->mode == TIMER_MODE_LOOP)
                tk_timer_restart(timer);
#endif /* TK_TIMER_USING_INTERVAL */
#ifdef TK_TIMER_USING_TIMEOUT_CALLBACK
            if (timer->timeout_callback != NULL)
                timer->timeout_callback(timer);
#endif /* TK_TIMER_USING_TIMEOUT_CALLBACK */
#ifdef TK_TIMER_USING_INTERVAL
            if (timer->mode == TIMER_MODE_LOOP)
                tk_timer_restart(timer);
#endif /* TK_TIMER_USING_INTERVAL */
        }
        timer = timer->next;
    }

    return true;
}

#endif /* TOOLKIT_USING_TIMER */
