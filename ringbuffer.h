#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define RING_BUFFER_DEBUG

typedef enum {
    RING_BUFFER_ERROR = -1,
    RING_BUFFER_OK    = 0,
    RING_BUFFER_FULL  = 1,
    RING_BUFFER_EMPTY = 1,
} ringBufferRetvTepeDef;

#define RING_BUFFER_NULL ((void *)(0))

typedef enum {
    RING_BUFFER_MUTEX_UNLOCK = (0),
    RING_BUFFER_MUTEX_LOCK   = (1),
} ringBufferMutexStatusTypeDef;

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef uint32_t ringBufferCountTypeDef;

typedef struct {
    void *(*ringBufferMalloc)(size_t size);
    void (*ringBufferFree)(void *ptr);
    ringBufferCountTypeDef element_count;
    ringBufferCountTypeDef element_size;
} ringBufferConfigTypeDef;

typedef struct {
    void *buffer;
    volatile ringBufferCountTypeDef write_pos;
    volatile ringBufferCountTypeDef read_pos;
    ringBufferMutexStatusTypeDef mutex;
    ringBufferConfigTypeDef config_info;
} ringBufferTypeDef;

ringBufferMutexStatusTypeDef ringBufferGetMutexStatus(ringBufferTypeDef *handler);
ringBufferRetvTepeDef ringBufferClear(ringBufferTypeDef *handler);
ringBufferTypeDef *ringBufferInit(ringBufferConfigTypeDef *config_info);
ringBufferRetvTepeDef ringBufferDestroy(ringBufferTypeDef *handler);
ringBufferRetvTepeDef ringBufferIsEmpty(ringBufferTypeDef *handler);
ringBufferRetvTepeDef ringBufferIsFull(ringBufferTypeDef *handler);
ringBufferCountTypeDef ringBufferGetSpaceCount(ringBufferTypeDef *handler);
ringBufferCountTypeDef ringBufferWrite(ringBufferTypeDef *handler, void *data_p, ringBufferCountTypeDef element_count);
ringBufferCountTypeDef ringBufferRead(ringBufferTypeDef *handler, void *data_p, ringBufferCountTypeDef element_count);

#ifdef __cplusplus
}
#endif

#endif
