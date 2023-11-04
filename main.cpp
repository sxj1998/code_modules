#include "DisplayEvent.h"

#include <iostream>
#include <thread>

// 这是要在新线程中执行的函数
void myFunction() {
    while (true)
    {
        std::cout << "\nEventProcess" << std::endl;
        UpstreamMachine::getInstance().EventProcess();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

// 这是要在新线程中执行的函数
void test() {
    while (true)
    {
        std::cout << "\ntest" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        handle1ACK(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        handle2ACK(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        handle3ACK(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        handle4ACK(4);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

int main() {

    UpstreamMachine::getInstance().EventMachineInit();

    std::thread myThread(myFunction);
    // 等待新线程执行完成
    myThread.detach();

    std::thread myThread2(test);
    // 等待新线程执行完成
    myThread2.detach();
    // 主线程继续执行
    std::cout << "主线程中的内容" << std::endl;
    while (true)
    {
        
    }
    return 0;
}




