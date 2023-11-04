#include "DisplayEvent.h"


int main() {

    UpstreamMachine::getInstance().EventMachineInit();

    while (true)
    {
        std::cout << "\nEventProcess" << std::endl;
        UpstreamMachine::getInstance().EventProcess();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
    }
    

    return 0;
}




