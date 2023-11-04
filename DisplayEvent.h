#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <list>
#include <algorithm> 
#include <functional>

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


    /*根据链表中的优先级 将TRIGER 链表中执行id进行排序*/
    bool comparePriority(const int& id1, const int& id2) {

    auto event1 = std::find_if(EventList.begin(), EventList.end(), [id1](const Event_t& event) {
        return event.id == id1;
    });

    auto event2 = std::find_if(EventList.begin(), EventList.end(), [id2](const Event_t& event) {
        return event.id == id2;
    });

    if (event1 != EventList.end() && event2 != EventList.end()) {
        return event1->priority < event2->priority;
    }
    return false; 
}

  

    void trggerEvent(int id)
    {
        std::unique_lock<std::mutex> lock(eventMutex);

        auto it = std::find(eventTrgList.begin(), eventTrgList.end(), id);
        if (it != eventTrgList.end()) {
            std::cout << "EVENT HAS PUSHED" << std::endl;
        } else {
            for (const Event_t& event : EventList) {
                if (id == event.id) {
                    eventTrgList.push_back(id);
                    break;
                }
            }
            eventTrgList.sort(std::bind(&UpstreamMachine::comparePriority, this, std::placeholders::_1, std::placeholders::_2));
        }


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

    void DebugPrintTri(const std::list<int>& eventTrgList) {
        std::cout << "DebugPrintTri LIST -------------" << std::endl;
        for (const int& id : eventTrgList) {
            // 在这里，您只有事件的 id，无法访问 Event_t 结构体的其他属性
            std::cout << "Event ID: " << id << std::endl;
        }
    }

};




void handle1ACK(int num);
void handle2ACK(int num);
void handle3ACK(int num);
void handle4ACK(int num);



