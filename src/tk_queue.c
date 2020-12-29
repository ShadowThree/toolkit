/*
* MIT License
* 
* Copyright (c) 2020 Cproape (911830982@qq.com)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
* Change Logs:
* Date           Author       Notes
* 2019-12-12     zhangran     the first version
* 2020-01-14     zhangran     add keep fresh mode
* 2020-01-29     zhangran     add assert for developer
* 2020-02-28     zhangran     add value type define
* 2020-03-04     zhangran     add queue clean code
* 2020-06-04     zhangran     support any type
* 2020-11-28     zhangran     add queue peep&remove code
*/

#include "toolkit.h"
#ifdef TOOLKIT_USING_QUEUE

/**
 * @brief ��̬��ʼ������
 * 
 * @param queue ���ж���
 * @param queuepool ���л�����
 * @param pool_size ��������С(��λ�ֽ�)
 * @param queue_size ����Ԫ�ش�С(��λ�ֽ�)
 * @param keep_fresh �Ƿ�Ϊ��������ģʽ,true���������� false��Ĭ��(���������ٴ�)
 * @return true ��ʼ���ɹ�
 * @return false ��ʼ��ʧ��
 */
bool tk_queue_init(struct tk_queue *queue, void *queuepool, uint16_t pool_size,
                   uint16_t queue_size, bool keep_fresh)
{
    TK_ASSERT(queue);
    TK_ASSERT(queuepool);
    TK_ASSERT(queue_size);
    TK_ASSERT(pool_size);
    if (queue == NULL || queuepool == NULL)
        return false;
    queue->keep_fresh = keep_fresh;
    queue->queue_pool = queuepool;
    queue->queue_size = queue_size;
    queue->max_queues = pool_size / queue->queue_size;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return true;
}

/**
 * @brief ��̬�������
 * ��ʹ��������������еĹ���������Ϊ��̬��ʽ�����Ķ��ж���
 * 
 * @param queue Ҫ����Ķ��ж���
 * @return true ����ɹ�
 * @return false ����ʧ��
 */
bool tk_queue_detach(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    queue->queue_pool = NULL;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return true;
}

#ifdef TK_QUEUE_USING_CREATE
/**
 * @brief ��̬��������
 * 
 * @param queue_size ����Ԫ�ش�С(��λ�ֽ�) 
 * @param max_queues �����и���
 * @param keep_fresh  �Ƿ�Ϊ��������ģʽ,true���������� false��Ĭ��(���������ٴ�)
 * @return struct tk_queue* �����Ķ��ж���,NULL����ʧ��
 */
struct tk_queue *tk_queue_create(uint16_t queue_size, uint16_t max_queues,
                                 bool keep_fresh)
{
    TK_ASSERT(queue_size);
    struct tk_queue *queue;
    if ((queue = malloc(sizeof(struct tk_queue))) == NULL)
        return NULL;
    queue->queue_size = queue_size;
    queue->max_queues = max_queues;
    queue->queue_pool = malloc(queue->queue_size * queue->max_queues);
    if (queue->queue_pool == NULL)
    {
        free(queue);
        return NULL;
    }
    queue->keep_fresh = keep_fresh;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return queue;
}

/**
 * @brief ��̬ɾ������
 * 
 * @param queue Ҫɾ���Ķ��ж���
 * @return true ɾ���ɹ�
 * @return false ɾ��ʧ��
 */
bool tk_queue_delete(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    free(queue->queue_pool);
    free(queue);
    return true;
}
#endif /* TK_QUEUE_USING_CREATE */

/**
 * @brief ��ն���
 * 
 * @param queue Ҫ��յĶ��ж���
 * @return true ����ɹ�
 * @return false ���ʧ��
 */
bool tk_queue_clean(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    queue->front = 0;
    queue->rear = 0;
    queue->len = 0;
    return true;
}

/**
 * @brief ��ѯ���е�ǰ���ݳ���
 * 
 * @param queue Ҫ��ѯ�Ķ��ж���
 * @return uint16_t �������ݵ�ǰ����(Ԫ�ظ���)
 */
uint16_t tk_queue_curr_len(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue == NULL)
        return false;
    return (queue->len);
}

/**
 * @brief �ж϶����Ƿ�Ϊ��
 * 
 * @param queue Ҫ��ѯ�Ķ��ж���
 * @return true ��
 * @return false ��Ϊ��
 */
bool tk_queue_empty(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue->len == 0)
        return true;
    return false;
}

/**
 * @brief �ж϶����Ƿ�����
 * 
 * @param queue Ҫ��ѯ�Ķ��ж���
 * @return true ��
 * @return false ��Ϊ��
 */
bool tk_queue_full(struct tk_queue *queue)
{
    TK_ASSERT(queue);
    if (queue->len >= queue->max_queues)
        return true;
    return false;
}

/**
 * @brief �����ѹ��(���)1��Ԫ������
 * 
 * @param queue Ҫѹ��Ķ��ж���
 * @param val ѹ��ֵ
 * @return true �ɹ�
 * @return false ʧ��
 */
bool tk_queue_push(struct tk_queue *queue, void *val)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    if (tk_queue_full(queue))
    {
        if (queue->keep_fresh == true)
        {
            memcpy((uint8_t *)queue->queue_pool + (queue->rear * queue->queue_size),
                   (uint8_t *)val, queue->queue_size);

            queue->rear = (queue->rear + 1) % queue->max_queues;
            queue->front = (queue->front + 1) % queue->max_queues;
            queue->len = queue->max_queues;
            return true;
        }
        return false;
    }
    else
    {
        memcpy((uint8_t *)queue->queue_pool + (queue->rear * queue->queue_size),
               (uint8_t *)val, queue->queue_size);

        queue->rear = (queue->rear + 1) % queue->max_queues;
        queue->len++;
    }
    return true;
}

/**
 * @brief �Ӷ��е���(����)1��Ԫ������
 * 
 * @param queue Ҫ�����Ķ��ж���
 * @param pval ����ֵ
 * @return true �ɹ�
 * @return false ʧ��
 */
bool tk_queue_pop(struct tk_queue *queue, void *pval)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    if (tk_queue_empty(queue))
    {
        return false;
    }
    else
    {
        memcpy((uint8_t *)pval,
               (uint8_t *)queue->queue_pool + (queue->front * queue->queue_size),
               queue->queue_size);

        queue->front = (queue->front + 1) % queue->max_queues;
        queue->len--;
    }
    return true;
}

/**
 * @brief �����ѹ��(���)���Ԫ������
 * 
 * @param queue Ҫѹ��Ķ��ж���
 * @param pval ѹ��Ԫ���׵�ַ
 * @param len ѹ��Ԫ�ظ���
 * @return uint16_t ʵ��ѹ�����
 */
uint16_t tk_queue_push_multi(struct tk_queue *queue, void *pval, uint16_t len)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    uint8_t *u8pval = pval;
    uint16_t push_len = 0;
    while (len-- && tk_queue_push(queue, u8pval) == true)
    {
        push_len++;
        u8pval += queue->queue_size;
    }

    return push_len;
}

/**
 * @brief �Ӷ��е���(����)���Ԫ������
 * 
 * @param queue Ҫ�����Ķ��ж���
 * @param pval ��ŵ���Ԫ�ص��׵�ַ
 * @param len ϣ��������Ԫ�ظ���
 * @return uint16_t ʵ�ʵ�������
 */
uint16_t tk_queue_pop_multi(struct tk_queue *queue, void *pval, uint16_t len)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    uint16_t pop_len = 0;
    uint8_t *u8pval = pval;
    if (tk_queue_empty(queue) == true)
        return false;
    while (len-- && tk_queue_pop(queue, u8pval) == true)
    {
        pop_len++;
        u8pval += queue->queue_size;
    }
    return pop_len;
}

/**
 * @brief �Ӷ����ж�ȡһ��Ԫ��(���Ӷ�����ɾ��)
 * 
 * @param queue ���ж���
 * @param pval ��ȡֵ��ַ
 * @return true ��ȡ�ɹ�
 * @return false ��ȡʧ��
 */
bool tk_queue_peep(struct tk_queue *queue, void *pval)
{
    TK_ASSERT(queue);
    TK_ASSERT(queue->queue_pool);
    if (tk_queue_empty(queue))
    {
        return false;
    }
    else
    {
        memcpy((uint8_t *)pval,
               (uint8_t *)queue->queue_pool + (queue->front * queue->queue_size),
               queue->queue_size);
    }
    return true;
}

/**
 * @brief �Ƴ�һ��Ԫ��
 * 
 * @param queue Ҫ�Ƴ�Ԫ�صĶ���
 * @return true �Ƴ��ɹ�
 * @return false �Ƴ�ʧ��
 */
bool tk_queue_remove(struct tk_queue *queue)
{
    if (tk_queue_empty(queue))
    {
        return true;
    }
    queue->front = (queue->front + 1) % queue->max_queues;
    queue->len--;

    return true;
}
#endif /* TOOLKIT_USING_QUEUE */
