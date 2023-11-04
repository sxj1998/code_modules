#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <list>

/*执行事件函数*/
typedef void (*EventHandleCall)(int); 

typedef void (*EventAckCall)(int); 

typedef void (*EventOutTimeCall)(int); 

// 事件结构体，包括事件类型、数据、优先级等信息
typedef struct{
    int id;
    int priority;
    EventHandleCall EventHandle;
    EventAckCall EventAck;
    EventOutTimeCall EventOutTime;
}Event_t;


class UpstreamMachine{
public:
    // 获取单例实例的静态方法
    static UpstreamMachine& getInstance() {
        static UpstreamMachine instance; // 创建唯一的实例
        return instance;
    }

    UpstreamMachine(){
        std::cout << "UpstreamMachine" << std::endl;
    }
    ~UpstreamMachine() {
        std::cout << "~UpstreamMachine" << std::endl;
    }

    void registEvent(int id, int priority,EventHandleCall EventHandle, EventAckCall EventAck,EventOutTimeCall EventOutTime)
    {
        Event_t event = {id, priority,EventHandle,EventAck,EventOutTime};
        EventList.push_back(event);
    }

    void delEvent(int id)
    {
        eventTrgList.remove(id);
    }

    void trggerEvent(int id)
    {
        eventTrgList.push_back(id);
    }

 

    void EventProcess(void);

    void EventMachineInit(void);

private:
    std::thread eventThread;
    std::list<int> eventTrgList;
    std::mutex eventMutex;
    std::list<Event_t> EventList;
    const int OUT_TIME = 1000;

    void DebugPrint(const std::list<Event_t>& eventList) {
        std::cout << "DEBUG PRINT LIST -------------" << std::endl;
        for (const Event_t& event : eventList) {
            std::cout << "Event ID: " << event.id << ", Priority: " << event.priority << std::endl;
        }
    }

};



