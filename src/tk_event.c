/*
* Change Logs:
* Date           Author       Notes
* 2020-01-31     zhangran     the first version
* 2020-12-09     zhangran     Modify option type to prevent warning
*/

#include "toolkit.h"
#ifdef TOOLKIT_USING_EVENT

#ifdef TK_EVENT_USING_CREATE
/**
 * @brief ��̬����һ���¼���
 * 
 * @return struct tk_event* �������¼�������NULLΪ����ʧ��
 */
struct tk_event *tk_event_create(void)
{
    struct tk_event *event;
    if ((event = malloc(sizeof(struct tk_event))) == NULL)
        return NULL;
    event->event_set = 0;
    return event;
}

/**
 * @brief ��̬ɾ��һ���¼���
 * 
 * @param event Ҫɾ�����¼�������
 * @return true ɾ���ɹ�
 * @return false ɾ��ʧ��
 */
bool tk_event_delete(struct tk_event *event)
{
    TK_ASSERT(event);
    free(event);
    return true;
}
#endif /* TK_EVENT_USING_CREATE */

/**
 * @brief ��̬��ʼ��һ���¼���
 * 
 * @param event Ҫ��ʼ�����¼�������
 * @return true ��ʼ���ɹ�
 * @return false ��ʼ��ʧ��
 */
bool tk_event_init(struct tk_event *event)
{
    TK_ASSERT(event);
    event->event_set = 0;
    return true;
}

/**
 * @brief �����¼���־
 * 
 * @param event ����Ŀ���¼�������
 * @param event_set �¼���־��ÿ����־ռ1Bit�����Ͷ����־��"|""
 * @return true ���ͳɹ�
 * @return false ����ʧ��
 */
bool tk_event_send(struct tk_event *event, uint32_t event_set)
{
    TK_ASSERT(event);
    event->event_set |= event_set;
    return true;
}

/**
 * @brief �����¼���־
 * 
 * @param event ����Ŀ���¼�������
 * @param event_set ����Ȥ�ı�־��ÿ����־ռ1Bit�������־��"|"
 * @param option ����:��־�룺TK_EVENT_OPTION_AND; ��־��TK_EVENT_OPTION_OR; �����־:TK_EVENT_OPTION_CLEAR
 * @param recved �¼���־
 * @return true ���ճɹ�
 * @return false ����ʧ��
 */
bool tk_event_recv(struct tk_event *event, uint32_t event_set, uint8_t option, uint32_t *recved)
{
    TK_ASSERT(event);
    bool result = false;
    if (option & TK_EVENT_OPTION_AND)
    {
        if ((event->event_set & event_set) == event_set)
            result = true;
    }
    else if (option & TK_EVENT_OPTION_OR)
    {
        if (event->event_set & event_set)
            result = true;
    }
    else
    {
        TK_ASSERT(0);
    }
    if (result == true)
    {
        if (recved)
            *recved = (event->event_set & event_set);

        if (option & TK_EVENT_OPTION_CLEAR)
            event->event_set &= ~event_set;
    }
    return result;
}

#endif /* TOOLKIT_USING_EVENT */
