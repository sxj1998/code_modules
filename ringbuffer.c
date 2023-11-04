#include "ringbuffer.h"
#include <stdio.h>


/**
 * @brief  operation mutex lock
 * @param  handler, handler of ring buffer
 * @retval none
 */
static void ringBufferMutexLock(ringBufferTypeDef *handler)
{
    if (!handler)
        return;

    handler->mutex = RING_BUFFER_MUTEX_LOCK;

    return;
}

/**
 * @brief  operation mutex unlock
 * @param  handler, handler of ring buffer
 * @retval none
 */
static void ringBufferMutexUnlock(ringBufferTypeDef *handler)
{
    if (!handler)
        return;

    handler->mutex = RING_BUFFER_MUTEX_UNLOCK;

    return;
}

/**
 * @brief  get operation mutex status
 * @param  handler, handler of ring buffer
 * @retval mutex status
 */
ringBufferMutexStatusTypeDef ringBufferGetMutexStatus(ringBufferTypeDef *handler)
{
    if (!handler)
        return RING_BUFFER_ERROR;

    return handler->mutex;
}

/**
 * @brief  clear the buffer
 * @param  handler, handler of ring buffer
 * @retval 0 : success
 * @retval -1 : error
 */
ringBufferRetvTepeDef ringBufferClear(ringBufferTypeDef *handler)
{
    if (!handler)
        return RING_BUFFER_ERROR;

    handler->read_pos  = 0;
    handler->write_pos = 0;

    return RING_BUFFER_OK;
}

/**
 * @brief  ring buffer initialize with configuration information
 * @param  config_info, file of configuration information
 * @retval pointer to handler
 */
ringBufferTypeDef *ringBufferInit(ringBufferConfigTypeDef *config_info)
{
    ringBufferTypeDef *handler;

    /*  parameter check  */
    if ((!config_info) || (!config_info->ringBufferFree) || (!config_info->ringBufferMalloc) ||
        ((config_info->element_count * config_info->element_size) == 0))
        return RING_BUFFER_NULL;

    /*  initialize handler  */
    handler = (ringBufferTypeDef *)(config_info->ringBufferMalloc(sizeof(ringBufferTypeDef)));
    if (!handler)
        return RING_BUFFER_NULL;

    /*  initialize configuration information  */
    handler->config_info = (*config_info);

    /*  initialize buffer  */
    handler->buffer =
        (void *)(handler->config_info.ringBufferMalloc(handler->config_info.element_count * handler->config_info.element_size));
    if (!handler->buffer) {
        handler->config_info.ringBufferFree(handler);
        return RING_BUFFER_NULL;
    }

    ringBufferClear(handler);

    ringBufferMutexUnlock(handler);

    return handler;
}

/**
 * @brief  destory the buffer
 * @param  handler, handler of ring buffer
 * @retval 0 : success
 * @retval -1 : error
 */
ringBufferRetvTepeDef ringBufferDestroy(ringBufferTypeDef *handler)
{
    if (!handler || !handler->buffer)
        return RING_BUFFER_ERROR;

    handler->config_info.ringBufferFree(handler->buffer);
    handler->config_info.ringBufferFree(handler);

    return RING_BUFFER_OK;
}

/**
 * @brief  detect if the buffer is empty
 * @param  handler, handler of ring buffer
 * @retval 1 : empty
 * @retval -1 : error
 * @retval 0 : no-empty
 */
ringBufferRetvTepeDef ringBufferIsEmpty(ringBufferTypeDef *handler)
{
    if (!handler)
        return RING_BUFFER_ERROR;

    return (handler->read_pos == handler->write_pos);
}

/**
 * @brief  detect if the buffer is full
 * @param  handler, handler of ring buffer
 * @retval 0 : no-full
 * @retval -1 : error
 * @retval 1 : full
 */
ringBufferRetvTepeDef ringBufferIsFull(ringBufferTypeDef *handler)
{
    if (!handler)
        return RING_BUFFER_ERROR;

    return (((handler->write_pos + 1) % handler->config_info.element_count) == handler->read_pos);
}

/**
 * @brief  get space count of the buffer
 * @param  handler, handler of ring buffer
 * @retval space count
 */
ringBufferCountTypeDef ringBufferGetSpaceCount(ringBufferTypeDef *handler)
{
    if (!handler)
        return 0;

    ringBufferCountTypeDef write_pos = handler->write_pos;
    ringBufferCountTypeDef read_pos  = handler->read_pos;

    if (write_pos >= read_pos) {
        return (handler->config_info.element_count - write_pos + read_pos - 1);
    } else {
        return (read_pos - write_pos - 1);
    }
}

/**
 * @brief  write data to buffer
 * @param  handler, handler of ring buffer
 * @param  data_p, pointer to the data that wait write
 * @param  element_count, count of the data
 * @retval success count of written
 */
ringBufferCountTypeDef ringBufferWrite(ringBufferTypeDef *handler, void *data_p, ringBufferCountTypeDef element_count)
{
    ringBufferCountTypeDef count_wait_write = element_count;
    ringBufferCountTypeDef count_to_tail    = 0;
    ringBufferCountTypeDef remain_count;

    if ((!handler) || (!handler->buffer) || (!data_p))
        return 0;

    ringBufferMutexLock(handler);

    remain_count = ringBufferGetSpaceCount(handler);
#ifdef RING_BUFFER_DEBUG
    printf("before write, space:%d\n", remain_count);
#endif

    /*  if remain count is enough  */
    if (count_wait_write > remain_count) {
        count_wait_write = remain_count;
    }
    if (handler->write_pos >= handler->read_pos) {
        count_to_tail = handler->config_info.element_count - handler->write_pos;
        if (count_to_tail >= count_wait_write) {
            memcpy(handler->buffer + handler->write_pos * handler->config_info.element_size, data_p,
                   count_wait_write * handler->config_info.element_size);
            handler->write_pos += count_wait_write;
        } else {
            memcpy(handler->buffer + handler->write_pos * handler->config_info.element_size, data_p,
                   count_to_tail * handler->config_info.element_size);
            memcpy(handler->buffer, data_p, (count_wait_write - count_to_tail) * handler->config_info.element_size);
            handler->write_pos = count_wait_write - count_to_tail;
        }
    } else {
        memcpy(handler->buffer + handler->write_pos * handler->config_info.element_size, data_p,
               count_wait_write * handler->config_info.element_size);
        handler->write_pos += count_wait_write;
    }
    ringBufferMutexUnlock(handler);
#ifdef RING_BUFFER_DEBUG
    printf("after write, w:%d, r:%d, wait_count:%d\n", handler->write_pos, handler->read_pos, count_wait_write);
#endif

    return count_wait_write;
}

/**
 * @brief  read data from buffer
 * @param  handler, handler of ring buffer
 * @param  data_p, pointer to the data that wait read
 * @param  element_count, count of the data
 * @retval success count of read
 */
ringBufferCountTypeDef ringBufferRead(ringBufferTypeDef *handler, void *data_p, ringBufferCountTypeDef element_count)
{
    ringBufferCountTypeDef count_wait_read = element_count;
    ringBufferCountTypeDef count_to_tail   = 0;
    ringBufferCountTypeDef remain_count;

    if (!handler || !handler->buffer || !data_p)
        return 0;

    ringBufferMutexLock(handler);

    remain_count = ringBufferGetSpaceCount(handler);

    /*  if remain data count is enough  */
    if (handler->config_info.element_count - remain_count - 1 < count_wait_read) {
        count_wait_read = handler->config_info.element_count - remain_count - 1;
    }

    if (handler->read_pos >= handler->write_pos) {
        count_to_tail = handler->config_info.element_count - handler->read_pos;
        if (count_to_tail >= count_wait_read) {
            memcpy(data_p, handler->buffer + handler->read_pos * handler->config_info.element_size,
                   count_wait_read * handler->config_info.element_size);
            handler->read_pos += count_wait_read;
        } else {
            memcpy(data_p, handler->buffer + handler->read_pos * handler->config_info.element_size,
                   count_to_tail * handler->config_info.element_size);
            memcpy(data_p, handler->buffer, (count_wait_read - count_to_tail) * handler->config_info.element_size);
            handler->read_pos = count_wait_read - count_to_tail;
        }

    } else {
        memcpy(data_p, handler->buffer + handler->read_pos * handler->config_info.element_size,
               count_wait_read * handler->config_info.element_size);
        handler->read_pos += count_wait_read;
    }

    ringBufferMutexUnlock(handler);
#ifdef RING_BUFFER_DEBUG
    printf("after read, w:%d, r:%d\n", handler->write_pos, handler->read_pos);
#endif
    return count_wait_read;
}
