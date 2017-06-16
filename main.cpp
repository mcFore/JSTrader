/*数据记录程序，确保程序调试过程中继续记录数据*/
#include"eventengine.h"
#include"datarecorder.h"

//主程序
int main()
{
    if(!Utils::checkExist("./logs")) {
        Utils::createDirectory("./logs");
    }
    EventEngine eventengine;
    Datarecorder datarecorder(&eventengine);
    eventengine.startEngine();
    std::string line;
    std::cout<<"typing \"exit\" close program"<<std::endl;
    while (std::getline(std::cin, line)) {
        if (line == "exit") {
            break;
        }
    }
    return 0;
}
