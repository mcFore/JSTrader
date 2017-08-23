#ifndef EVENTENGINE_H
#define EVENTENGINE_H
#ifdef WIN32
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif
#ifdef  EVENTENGINE_EXPORTS
#define EVENTENGINE_API __declspec(dllexport)
#else
#define EVENTENGINE_API __declspec(dllimport)
#endif
#endif
#ifdef linux
#define EVENTENGINE_API
#endif
#include<string>
#include<queue>
#include<mutex>
#include<memory>
#include<condition_variable>
#include<map>
#include<thread>
#include<atomic>
#include<chrono>
#include<time.h>
#include"../include/structs.hpp"
#include"../include/utils.hpp"
template<typename EVENT>
class EVENTENGINE_API SynQueue
{
public:
    void push(std::shared_ptr<EVENT>Event) 
	{
        std::unique_lock<std::recursive_mutex>lck(mutex);
        queue.push(Event);
        cv.notify_all();
    }
    std::shared_ptr<Event> take() 
	{
        std::unique_lock<std::recursive_mutex>lck(mutex);
        while (queue.empty()) {
            cv.wait(lck);
        }
        std::shared_ptr<Event>e = queue.front();
        queue.pop();
        return e;
    }

private:
    std::recursive_mutex mutex;
    std::queue<std::shared_ptr<EVENT>> queue;
    std::condition_variable_any cv;
};


typedef std::function<void(std::shared_ptr<Event>)> TASK;

class EVENTENGINE_API EventEngine
{
public:
    EventEngine();
    ~EventEngine();
    void startEngine();
    void stopEngine();
    void regEvent(const std::string &eventtype,const TASK &task);
    void unregEvent(const std::string &eventtype);
    void doTask();
    void put(std::shared_ptr<Event>e);
    void timer();
private:
    std::mutex mutex;
    std::condition_variable cv;
    std::vector<std::thread*>*task_pool = nullptr;
    SynQueue<Event>*event_queue = nullptr;
    std::multimap<std::string, TASK>*task_map = nullptr;
    std::thread* timer_thread = nullptr;;
    std::atomic<bool>active;
};
#endif
