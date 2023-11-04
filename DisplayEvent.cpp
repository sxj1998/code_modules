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
    UpstreamMachine::getInstance().delEvent(1);
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
    UpstreamMachine::getInstance().delEvent(2);
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
    UpstreamMachine::getInstance().delEvent(3);
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
    UpstreamMachine::getInstance().delEvent(4);
    std::cout << "handle4ACK " << num << std::endl;
}
void handle4OUT(int num)
{
    std::cout << "handle4OUT " << num << std::endl;
}

void UpstreamMachine::EventMachineInit(void){
    std::cout << "EventMachineInit" << std::endl;
    registEvent(1,3,handle1,handle1ACK,handle1OUT);
    registEvent(2,4,handle2,handle2ACK,handle2OUT); 
    registEvent(3,2,handle3,handle3ACK,handle3OUT);
    registEvent(4,2,handle4,handle4ACK,handle4OUT); 
    trggerEvent(1);
    trggerEvent(2);
    trggerEvent(3);
    trggerEvent(2);
    trggerEvent(2);
    // trggerEvent(4);
    DebugPrint(EventList);
    DebugPrintTri(eventTrgList);
}


void UpstreamMachine::EventProcess(void)
{
    // 检查事件队列是否为空, 非空就按顺序执行队列中的任务
    if (!eventTrgList.empty()) {
        std::cout << "trigEventId " << eventTrgList.front() << std::endl;
        int trigEventId = eventTrgList.front();
        for (const Event_t& event : EventList) {
            if(trigEventId == event.id)
            {
                if(event.EventHandle)
                    event.EventHandle(event.id);
                break;
            }
        }
    } 
    DebugPrintTri(eventTrgList);
}

