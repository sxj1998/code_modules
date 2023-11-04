// #include "stdio.h"
// #include "stdlib.h"
// #include "string.h"
// #include <stdint.h>


// #include "ringbuffer.h"
// #include <pthread.h>
// #include <stdlib.h>     // for malloc, free
// #include <stdio.h>      // for printf
// #include <unistd.h>

// #define BUFFER_ELEM_SIZE  sizeof(int)
// #define BUFFER_ELEM_COUNT 1024

// ringBufferConfigTypeDef rb_config;
// ringBufferTypeDef *rb_handler;

// void *writer(void *param) {
//     int data_to_write = 1;

//     while(1) {
//         if(ringBufferWrite(rb_handler, &data_to_write, 1) > 0) {
//             printf("Writer: wrote %d\n", data_to_write);
//             data_to_write++;
//         }
//         usleep(2000*1000);
//     }
// }

// void *reader(void *param) {
//     int read_data;

//     while(1) {
//         if(ringBufferRead(rb_handler, &read_data, 1) > 0) {
//             printf("Reader: read %d\n", read_data);
//         }
//         usleep(100*1000);
//     }
// }

// int main() {
//     pthread_t writer_thread, reader_thread;

//     //配置环形缓冲区
//     rb_config.ringBufferMalloc = malloc;
//     rb_config.ringBufferFree   = free;
//     rb_config.element_size     = BUFFER_ELEM_SIZE;
//     rb_config.element_count    = BUFFER_ELEM_COUNT;

//     //初始化环形缓冲区
//     rb_handler = ringBufferInit(&rb_config);
//     if (rb_handler == RING_BUFFER_NULL) {
//         printf("Ring buffer initialize failed!\n");
//         return -1;
//     }

//     //创建写线程和读线程
//     pthread_create(&writer_thread, NULL, writer, NULL);
//     pthread_create(&reader_thread, NULL, reader, NULL);

//     //等待线程结束
//     pthread_join(writer_thread, NULL);
//     pthread_join(reader_thread, NULL);

//     //销毁环形缓冲区
//     ringBufferDestroy(rb_handler);

//     return 0;
// }



#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

template<typename T>
class CircularBuffer {
public:
    explicit CircularBuffer(size_t size) : data_(size), head_(0), tail_(0), size_(0), max_size_(size) {}

    void push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [&]() { return size_ < max_size_; });

        data_[head_] = item;
        head_ = (head_ + 1) % max_size_;
        ++size_;

        lock.unlock();
        cond_var_.notify_all();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [&]() { return size_ > 0; });

        T item = data_[tail_];
        tail_ = (tail_ + 1) % max_size_;
        --size_;

        lock.unlock();
        cond_var_.notify_all();

        return item;
    }

private:
    vector<T> data_;
    size_t head_;
    size_t tail_;
    size_t size_;
    size_t max_size_;
    mutex mutex_;
    condition_variable cond_var_;
};

void producer(CircularBuffer<int>& buffer) {
    for (int i = 0; i < 100000; ++i) {
        buffer.push(i);
    }
}

void consumer(CircularBuffer<int>& buffer) {
    for (int i = 0; i < 100000; ++i) {
        int value = buffer.pop();
        cout << "Consumer popped " << value << endl;
    }
}

int main() {
    CircularBuffer<int> buffer(1000);

    thread prod(producer, ref(buffer));
    thread cons(consumer, ref(buffer));

    prod.join();
    cons.join();

    return 0;
}


