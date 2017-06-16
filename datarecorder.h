#ifndef DATARECORDER_H
#define DATARECORDER_H
#include<set>
#include<string>
#include<memory>
#include<map>
#include<mutex>
#include "eventengine.h"
#include "ctpmd.h"
#include "bson.h"
#include "mongoc.h"
#include "spdlog/spdlog.h"
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif
class Datarecorder
{
public:
	Datarecorder(EventEngine *eventengine);
	~Datarecorder();

private:
	EventEngine *eventengine;
	Ctpmd *ctpgateway;

	bool connectStatus;
	std::set<std::string>ninetoeleven;
	std::set<std::string>ninetohalfeleven;
	std::set<std::string>ninetoone;
	std::set<std::string>ninetohalftwo;

	std::set<std::string>conf_symbols;
	void readSymbols();

	inline void writeLog(const std::string& msg) const;
	inline void showLog(std::shared_ptr<Event>e);

	void onTick(std::shared_ptr<Event>e);
	void OnBar(const BarData &bar);
	void onDailybar();
	void autoConnect();

	void insertTicktoDb(const std::string &dbname,const std::string &symbol, std::shared_ptr<Event_Tick> tick);
	mutable std::mutex log_mutex;


	std::map<std::string,int>barMinutemap;
	std::map<std::string,int>barHourmap;
	std::map<std::string,BarData>barmap;
	std::map<std::string,std::shared_ptr<Event_Tick>>dailyBarmap;
	std::mutex dailyBarmtx;

	mongoc_client_pool_t *pool;
	mongoc_uri_t         *uri;

	std::map<std::string,long long>lastTickIdmap;

	std::shared_ptr<spdlog::logger> my_logger;
};
#endif
