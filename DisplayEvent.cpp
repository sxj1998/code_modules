#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <iostream>
#include "DisplayEvent.h"

void handle1(int num)
{
    std::cout << "handle1 " << num << std::endl;
}
void handle1ACK(int num)
{   
    UpstreamMachine::getInstance().removeTrigEvent(1);
    UpstreamMachine::getInstance().delOutTimeEvent(1);
    std::cout << "handle1ACK " << num << std::endl;
}
void handle1OUT(int num)
{
    std::cout << "handle1OUT " << num << std::endl;
}

void handle2(int num)
{
    std::cout << "handle2 " << num << std::endl;
}
void handle2ACK(int num)
{
    UpstreamMachine::getInstance().removeTrigEvent(2);
    UpstreamMachine::getInstance().delOutTimeEvent(2);
    std::cout << "handle2ACK " << num << std::endl;
}
void handle2OUT(int num)
{
    std::cout << "handle2OUT " << num << std::endl;
}

void handle3(int num)
{
    std::cout << "handle3 " << num << std::endl;
}
void handle3ACK(int num)
{
    UpstreamMachine::getInstance().removeTrigEvent(3);
    UpstreamMachine::getInstance().delOutTimeEvent(3);
    std::cout << "handle3ACK " << num << std::endl;
}
void handle3OUT(int num)
{
    std::cout << "handle3OUT " << num << std::endl;
}


void handle4(int num)
{
    std::cout << "handle4 " << num << std::endl;
}
void handle4ACK(int num)
{
    UpstreamMachine::getInstance().removeTrigEvent(4);
    UpstreamMachine::getInstance().delOutTimeEvent(4);
    std::cout << "handle4ACK " << num << std::endl;
}
void handle4OUT(int num)
{
    std::cout << "handle4OUT " << num << std::endl;
}

void UpstreamMachine::EventMachineInit(void){
    std::cout << "EventMachineInit" << std::endl;
    registEvent(1,3,handle1,handle1ACK,handle1OUT,OUTTIME_SETTINGS);
    registEvent(2,4,handle2,handle2ACK,handle2OUT,OUTTIME_SETTINGS); 
    registEvent(3,2,handle3,handle3ACK,handle3OUT,OUTTIME_SETTINGS);
    registEvent(4,2,handle4,handle4ACK,handle4OUT,OUTTIME_SETTINGS); 
    trggerEvent(1);
    trggerEvent(2);
    trggerEvent(3);
    trggerEvent(4);
    // trggerEvent(4);
    DebugPrint(EventList);
    DebugPrintTri(eventTrgList);
}


void UpstreamMachine::EventProcess(void)
{
    // 检查超时队列是否为空, 非空就按顺序执行队列中的任务
    if (!eventOUTList.empty()) {
        std::cout << "OutEventId " << eventOUTList.front() << std::endl;
        int OutEventId = eventOUTList.front();
        for (Event_t& event : EventList) {
            if(OutEventId == event.id)
            {
                if(event.eventOutTimeCnt%event.eventOutTimeSet == 0)
                {
                    if(event.EventHandle)
                    {
                        event.EventHandle(event.id);
                    }  
                }
                event.eventOutTimeCnt++;
            }
        }
    } 

    // 检查事件队列是否为空, 非空就按顺序执行队列中的任务
    if (!eventTrgList.empty()) {
        std::cout << "trigEventId " << eventTrgList.front() << std::endl;
        int trigEventId = eventTrgList.front();
        for (Event_t& event : EventList) {
            if(trigEventId == event.id)
            {
                if(event.EventHandle)
                {
                    event.EventHandle(event.id);
                    event.eventOutTimeCnt++;
                    /*判断超时执行后将事件压入超市执行列表*/
                    if(event.eventOutTimeCnt >= event.eventOutTimeSet)
                    {
                        addOutTimeEvent(trigEventId);
                        removeTrigEvent(trigEventId);
                    }
                }  
            }
        }
    } 

    DebugPrintTri(eventTrgList);
    DebugPrintOut(eventOUTList);
}


void UpstreamMachine::trggerEvent(int id)
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
