#include "eventengine.h"
EventEngine::EventEngine()
{
	this->active = false;
	this->event_queue = new SynQueue<Event>;
	this->task_map = new std::multimap<std::string, TASK>;
}

EventEngine::~EventEngine()
{
	this->stopEngine();
	if (this->event_queue) {
		delete this->event_queue;
		this->event_queue = nullptr;
	}
	if (this->task_map) {
		delete this->task_map;
		this->task_map = nullptr;
	}
}

void EventEngine::regEvent(const std::string &eventtype, const TASK &task)
{
	std::unique_lock<std::mutex>lck(mutex);
	this->task_map->insert(std::make_pair(eventtype, task));
}

void EventEngine::unregEvent(const std::string &eventtype)
{
	std::unique_lock<std::mutex>lck(mutex);
	this->task_map->erase(eventtype);
}

void EventEngine::put(std::shared_ptr<Event>e)
{
	this->event_queue->push(e);
}

void EventEngine::startEngine()
{
	this->active = true;
	this->timer_thread = new std::thread(std::bind(&EventEngine::timer, this));
	this->task_pool = new std::vector<std::thread*>;
	std::function<void()>f = std::bind(&EventEngine::doTask, this);
	for (unsigned i = 0; i < std::thread::hardware_concurrency(); ++i) {
		std::thread* thread_worker = new std::thread(f);
		this->task_pool->push_back(thread_worker);
	}
}

void EventEngine::stopEngine()
{
	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i) {
		std::shared_ptr<Event_Exit>e = std::make_shared<Event_Exit>();
		this->put(e);
	}
	this->active = false;
	if (this->timer_thread) {
		this->timer_thread->join();
		delete  this->timer_thread;
		this->timer_thread = nullptr;
	}
	if (this->task_pool != nullptr) {
		for (std::vector<std::thread*>::iterator it = this->task_pool->begin(); it != this->task_pool->end(); it++) {
			(*it)->join();
			delete (*it);
		}
		delete  this->task_pool;
		this->task_pool = nullptr;
	}
}

void EventEngine::doTask()
{
	while (this->active) 
	{
		std::shared_ptr<Event>e = event_queue->take();
		e->GetEventType();
		if (e->GetEventType() == EVENT_QUIT)
		{
			break;
		}
		std::pair<std::multimap<std::string, TASK>::iterator, std::multimap<std::string, TASK>::iterator>ret;
		this->mutex.lock();
		ret = task_map->equal_range(e->GetEventType());
		this->mutex.unlock();
		for (std::multimap<std::string, TASK>::iterator it = ret.first; it != ret.second; ++it) 
		{
			TASK t;
			t = it->second;
			t(e);
		}
	}
}

void EventEngine::timer()
{
	while (this->active) 
	{
		std::shared_ptr<Event_Timer>e = std::make_shared<Event_Timer>();
		this->put(e);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
