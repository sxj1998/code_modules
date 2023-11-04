// #include <iostream>
// #include <vector>
// #include <mutex>
// #include <condition_variable>

// // 环形缓冲区类
// class CircularBuffer {
// public:
//     CircularBuffer(size_t size) : buf_(size), max_size_(size), head_(0), tail_(0), full_(0) {}

//     // 写入数据
//     void write(int data) {
//         std::unique_lock<std::mutex> lock(mutex_);
//         buf_[head_] = data;
//         if (full_) {
//             tail_ = (tail_ + 1) % max_size_;
//         }
//         head_ = (head_ + 1) % max_size_;
//         full_ = head_ == tail_;
//         lock.unlock();
//         cond_.notify_one();
//     }

//     // 读取数据
//     int read() {
//         std::unique_lock<std::mutex> lock(mutex_);
//         cond_.wait(lock, [this]() { return full_ || head_ != tail_; });
//         auto val = buf_[tail_];
//         full_ = false;
//         tail_ = (tail_ + 1) % max_size_;
//         return val;
//     }

// private:
//     std::vector<int> buf_;
//     size_t head_;
//     size_t tail_;
//     const size_t max_size_;
//     bool full_;
//     std::mutex mutex_;
//     std::condition_variable cond_;
// };

// int main() {
//     CircularBuffer cb(19);

//     // 模拟音视频数据的生产过程
//     for (int i = 0; i < 20; ++i) {
//         cb.write(i);
//         std::cout << "Producing: " << i << std::endl;
//     }

//     // 模拟音视频数据的消费过程
//     for (int i = 0; i < 20; ++i) {
//         int data = cb.read();
//         std::cout << "Consuming: " << data << std::endl;
//     }

//     return 0;
// }




// #include <iostream>
// #include <vector>
// #include <thread>
// #include <mutex>
// #include <condition_variable>

// class CircularBuffer {
// public:
//     CircularBuffer(size_t size) : data_(size), head_(0), tail_(0), size_(0), capacity_(size) {}

//     void push_back(char val) {
//         std::unique_lock<std::mutex> lock(mutex_);
//         cond_empty_.wait(lock, [this]() { return size_ < capacity_; });

//         data_[head_] = val;
//         head_ = (head_ + 1) % capacity_;
//         ++size_;

//         lock.unlock();
//         cond_full_.notify_one();
//     }

//     char pop_front() {
//         std::unique_lock<std::mutex> lock(mutex_);
//         cond_full_.wait(lock, [this]() { return size_ > 0; });

//         char val = data_[tail_];
//         tail_ = (tail_ + 1) % capacity_;
//         --size_;

//         lock.unlock();
//         cond_empty_.notify_one();

//         return val;
//     }

// private:
//     std::vector<char> data_;
//     size_t head_;
//     size_t tail_;
//     size_t size_;
//     size_t capacity_;
//     std::mutex mutex_;
//     std::condition_variable cond_empty_;
//     std::condition_variable cond_full_;
// };

// void producer(CircularBuffer& buffer) {
//     for (char c = 'a'; c <= 'z'; ++c) {
//         buffer.push_back(c);
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }
// }

// void consumer(CircularBuffer& buffer) {
//     for (int i = 0; i < 26; ++i) {
//         char c = buffer.pop_front();
//         std::cout << "Consumer received: " << c << std::endl;
//         std::this_thread::sleep_for(std::chrono::milliseconds(150));
//     }
// }

// int main() {
//     CircularBuffer buffer(5);

//     std::thread prod(producer, std::ref(buffer));
//     std::thread cons(consumer, std::ref(buffer));

//     prod.join();
//     cons.join();

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