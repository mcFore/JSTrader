#include"CTPMD.h"
#include"RecorderManager.h"
#include"JSRecordClass.h"
#include"Recorder.h"
#include<mutex>
#include<iostream>
#include<thread>
#include<chrono>
#include<fstream>
#include<string>
#include<chrono>
#include<stdlib.h>
#include<algorithm>
#include<regex>
#include <iterator>
Recorder::Recorder(EventEngine *eventengine, Recordermanager *recordermanager)
{
	m_ptr_eventengine = eventengine;
	m_ptr_Recordermanager = recordermanager;
	m_connectstatus = false;
	std::string ninetoeleven[] = { "bu", "rb", "hc", "ru" };//9点到11点的合约列表
	std::string ninetohalfeleven[] = { "p", "j", "m", "y", "a", "b", "jm", "i", "SR", "CF", "RM", "MA", "ZC", "FG", "OI" };//9点到11点半的合约
	std::string ninetoone[] = { "cu", "al", "zn", "pb", "sn", "ni" };//9点到1点的合约列表
	std::string ninetohalftwo[] = { "ag", "au" };//9点到2点半的合约
	for (int i = 0; i < sizeof(ninetoeleven) / sizeof(ninetoeleven[0]); ++i)
	{
		m_ninetoeleven.insert(ninetoeleven[i]);
	}
	for (int i = 0; i < sizeof(ninetohalfeleven) / sizeof(ninetohalfeleven[0]); ++i)
	{
		m_ninetohalfeleven.insert(ninetohalfeleven[i]);
	}
	for (int i = 0; i < sizeof(ninetoone) / sizeof(ninetoone[0]); ++i)
	{
		m_ninetoone.insert(ninetoone[i]);
	}
	for (int i = 0; i < sizeof(ninetohalftwo) / sizeof(ninetohalftwo[0]); ++i)
	{
		m_ninetohalftwo.insert(ninetohalftwo[i]);
	}

	mongoc_init();
	uri = mongoc_uri_new("mongodb://localhost/");
	// 创建客户端池
	m_pool = mongoc_client_pool_new(uri);
}

Recorder::~Recorder()
{

}

void Recorder::exit()
{
	// 是否客户端池
	mongoc_client_pool_destroy(m_pool);
	mongoc_uri_destroy(uri);
	mongoc_cleanup();
}

void Recorder::readsymbols(std::string gatewayname)
{
	if (gatewayname == "CTP")
	{
		if (_access("./CTPGateway", 0) != -1)
		{
			std::fstream f;
			f.open("./CTPGateway/CTP_datarecord");
			if (!f.is_open())
			{
				//如果打不开文件
				std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
				e->msg = "无法读取CTP_datarecord合约文件";
				e->gatewayname = "CTP";
				m_ptr_eventengine->Put(e);
				return;
			}
			std::string line;
			std::map<std::string, std::string>symbols;
			while (!f.eof())
			{
				getline(f, line);
				std::string::size_type pos = line.find("=");//按照等号分隔
				symbols.insert(std::make_pair(line.substr(0, pos), line.substr(pos + 1, line.size() - 1)));
			}
			for (std::map<std::string, std::string>::iterator it = symbols.begin(); it != symbols.end(); it++)
			{
				//一个一个订阅
				if (it->first == "tick")
				{
					std::cout << "tick代码:";
					std::string line = it->second;
					char* pch = strtok(const_cast<char*>(line.c_str()), ",[]");
					while (pch != NULL)
					{
						std::cout << pch << " ";
						m_ticksymbols.insert(std::string(pch));
						m_barMinute.insert(std::pair<std::string,int>(std::string(pch), 99));
						m_barHour.insert(std::pair<std::string,int>(std::string(pch), 99));
						m_allsymbols.insert(std::string(pch));
						pch = strtok(NULL, ",[]");
					}
					std::cout << "\n";
				}
				if (it->first == "bar")
				{
					std::cout << "bar代码:";
					std::string line = it->second;
					char* pch = strtok(const_cast<char*>(line.c_str()), ",[]");
					while (pch != NULL)
					{
						std::cout << pch << " ";
						m_barsymbols.insert(std::string(pch));
						m_allsymbols.insert(std::string(pch));
						pch = strtok(NULL, ",[]");
					}
					std::cout << "\n";
				}
				if (it->first == "dailybar")
				{
					std::cout << "日线代码:";
					std::string line = it->second;
					char* pch = strtok(const_cast<char*>(line.c_str()), ",[]");
					while (pch != NULL)
					{
						std::cout << pch << " ";
						m_dailybarsymbols.insert(std::string(pch));
						m_allsymbols.insert(std::string(pch));
						pch = strtok(NULL, ",[]");
					}
					std::cout << "\n";
				}
			}
			for (std::set<std::string>::iterator it = m_allsymbols.begin(); it != m_allsymbols.end(); it++)
			{
				SubscribeReq req;
				req.symbol = *it;
				m_ptr_Recordermanager->subscribe(req, "CTP");//订阅一个合约
			}
		}
		else
		{
			std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
			e->msg = "无法读取本地配置文件";
			e->gatewayname = gatewayname;
			m_ptr_eventengine->Put(e);
		}
	}
}

void Recorder::showlog(std::shared_ptr<Event>e)
{
	std::unique_lock<std::mutex>lck(m_logmtx);
	std::shared_ptr<Event_Log> log = std::static_pointer_cast<Event_Log>(e);
	if (log->gatewayname == "CTP")
	{
		if (log->msg == "行情服务器登录完成")
		{
			readsymbols("CTP");//开始订阅合约
			m_connectstatus = true;
		}
	}
	else if (log->gatewayname == "LTS")
	{
		//各种接口巴拉巴拉巴拉...
		if (log->msg == "行情服务器登录完成")
			readsymbols("LTS");//开始订阅合约
	}
	
	std::cout << "接口:  " << log->gatewayname << "时间:" << log->logTime << "消息:" << log->msg << std::endl;
}

const time_t Recorder::timetounixtimestamp(int hour, int minute, int seconds)
{
	//时分秒转换成unix时间戳
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm *l = localtime(&tt);//将UNIX时间戳转TM结构
	l->tm_hour = hour;
	l->tm_min = minute;
	l->tm_sec = seconds;
	time_t ft = mktime(l);
	return ft;

}

void Recorder::OnTick(std::shared_ptr<Event>e)
{

	std::shared_ptr<Event_Tick> Tick = std::static_pointer_cast<Event_Tick>(e);
	//判断一下合约时间是否满足规则
	std::shared_ptr<TickData>data = std::make_shared<TickData>();
	data->gatewayname = Tick->gatewayname;
	data->symbol = Tick->symbol;
	data->exchange = Tick->exchange;
	data->lastprice = Tick->lastprice;
	data->volume = Tick->volume;
	data->openInterest = Tick->openInterest;
	data->unixdatetime = getsystemunixdatetime(Tick->time, "ms");
	data->date = Tick->date;
	data->time = Tick->time;
	data->openPrice = Tick->openPrice;
	data->highPrice = Tick->highPrice;
	data->lowPrice = Tick->lowPrice;
	data->preClosePrice = Tick->preClosePrice;
	data->upperLimit = Tick->upperLimit;
	data->lowerLimit = Tick->lowerLimit;
	data->bidprice1 = Tick->bidprice1;
	data->askprice1 = Tick->askprice1;
	data->bidvolume1 = Tick->bidvolume1;
	data->askvolume1 = Tick->askvolume1;

	int tickMinute = data->getminute();//获取分钟
	int tickHour = data->gethour();//获取小时
	//首先修改时间避免两个tick来了还没能搞定

	if (Tick->gatewayname == "CTP")
	{
		//过滤CTP的时间***************************************************************************************************************************
		auto nowtime = getsystemunixdatetime(Tick->time, "s");//数据包时间
		auto nowtime2 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::string s = Tick->symbol;
		std::regex regex("\\d");   // matches words beginning by "sub"
		// using string/c-string (3) version:
		std::string symbol = std::regex_replace(s, regex, "");
		if (m_ninetoeleven.find(symbol) != m_ninetoeleven.end())
		{
			if (!(((nowtime >= timetounixtimestamp(9, 0, 0) && nowtime <= timetounixtimestamp(15, 0, 0))) || ((nowtime >= timetounixtimestamp(21, 0, 0)) && (nowtime <= timetounixtimestamp(23, 0, 0)))))
			{
				//在9点到11点里头的对立事件则返回
				return;
			}
			else
			{
				//数据包的时间是在交易时间段的在判断一下真实时间是否也在交易时间段
				if (!(((nowtime2 >= timetounixtimestamp(9, 0, 0) && nowtime2 <= timetounixtimestamp(15, 0, 0))) || ((nowtime2 >= timetounixtimestamp(21, 0, 0)) && (nowtime2 <= timetounixtimestamp(23, 0, 0)))))
				{
					return;
				}
			}
		}
		else if (m_ninetohalfeleven.find(symbol) != m_ninetohalfeleven.end())
		{
			if (!(((nowtime >= timetounixtimestamp(9, 0, 0) && nowtime <= timetounixtimestamp(15, 0, 0))) || ((nowtime >= timetounixtimestamp(21, 0, 0)) && (nowtime <= timetounixtimestamp(23, 30, 0)))))
			{
				return;
			}
			else
			{
				if (!(((nowtime2 >= timetounixtimestamp(9, 0, 0) && nowtime2 <= timetounixtimestamp(15, 0, 0))) || ((nowtime2 >= timetounixtimestamp(21, 0, 0)) && (nowtime2 <= timetounixtimestamp(23, 30, 0)))))
				{
					return;
				}
			}

		}
		else if (m_ninetoone.find(symbol) != m_ninetoone.end())
		{
			if (!(((nowtime >= timetounixtimestamp(9, 0, 0) && nowtime <= timetounixtimestamp(15, 0, 0))) || ((nowtime >= timetounixtimestamp(21, 0, 0)) && (nowtime <= timetounixtimestamp(24, 0, 0))) || (nowtime <= timetounixtimestamp(1, 0, 0))))
			{
				return;
			}
			else
			{
				if (!(((nowtime2 >= timetounixtimestamp(9, 0, 0) && nowtime2 <= timetounixtimestamp(15, 0, 0))) || ((nowtime2 >= timetounixtimestamp(21, 0, 0)) && (nowtime2 <= timetounixtimestamp(24, 0, 0))) || (nowtime2 <= timetounixtimestamp(1, 0, 0))))
				{
					return;
				}
			}
		}
		else if (m_ninetohalftwo.find(symbol) != m_ninetohalftwo.end())
		{
			if (!(((nowtime >= timetounixtimestamp(9, 0, 0) && nowtime <= timetounixtimestamp(15, 0, 0))) || ((nowtime >= timetounixtimestamp(21, 0, 0)) && (nowtime <= timetounixtimestamp(24, 0, 0))) || (nowtime <= timetounixtimestamp(2, 30, 0))))
			{
				return;
			}
			else
			{
				if (!(((nowtime2 >= timetounixtimestamp(9, 0, 0) && nowtime2 <= timetounixtimestamp(15, 0, 0))) || ((nowtime2 >= timetounixtimestamp(21, 0, 0)) && (nowtime2 <= timetounixtimestamp(24, 0, 0))) || (nowtime2 <= timetounixtimestamp(2, 30, 0))))
				{
					return;
				}
			}
		}
		else
		{
			//如果只是日盘没有夜盘的合约，就判断一下发过来的时间是否是在日盘交易时间之内
			if (!(nowtime >= timetounixtimestamp(9, 0, 0) && nowtime <= timetounixtimestamp(15, 0, 0)))
			{
				return;
			}
			else
			{
				if (!(nowtime2 >= timetounixtimestamp(9, 0, 0) && nowtime2 <= timetounixtimestamp(15, 0, 0)))
				{
					return;
				}
			}
		}
		//****************************************************************************************************************************************************
		if (m_ticksymbols.find(Tick->symbol) != m_ticksymbols.end())
		{
			//收集最新一根TICK
			m_dailybarmap[data->symbol] = *data;//每次更新
			dbInsert("CTPTickDb", Tick->symbol.c_str(), data, m_pool);
		}
		//插入TICK之后对分钟数据进行合成

		m_hourminutemtx.lock();

		if ((tickMinute != m_barMinute[data->symbol])||(tickHour!=m_barHour[data->symbol]))
		{
			if (m_barmap.find(Tick->symbol) != m_barmap.end())
			{//判断这个合约是否要存分钟bar
				if (!((m_barHour[data->symbol] == 11 && m_barMinute[data->symbol] == 30) || (m_barHour[data->symbol] == 15 && m_barMinute[data->symbol] == 00) || (m_barHour[data->symbol] == 10 && m_barMinute[data->symbol] == 15)))//剔除10点15分11点半下午3点的一根TICK合成出来的K线
				{
					if (m_ninetoeleven.find(symbol) != m_ninetoeleven.end())
					{
						if (m_barHour[data->symbol] == 23)
						{
							m_barHour[data->symbol] = 99;
							m_barmap.erase(data->symbol);
							m_hourminutemtx.unlock();
							return;
						}
					}
					else if (m_ninetohalfeleven.find(symbol) != m_ninetohalfeleven.end())
					{
						if (m_barHour[data->symbol] == 23 && m_barMinute[data->symbol] == 30)
						{
							m_barHour[data->symbol] = 99;
							m_barMinute[data->symbol] = 99;
							m_barmap.erase(data->symbol);
							m_hourminutemtx.unlock();
							return;
						}
					}
					else if (m_ninetoone.find(symbol) != m_ninetoone.end())
					{
						if (m_barHour[data->symbol] == 1)
						{
							m_barHour[data->symbol] = 99;
							m_barmap.erase(data->symbol);
							m_hourminutemtx.unlock();
							return;
						}
					}
					else if (m_ninetohalftwo.find(symbol) != m_ninetohalftwo.end())
					{
						if (m_barHour[data->symbol] == 2 && m_barMinute[data->symbol] == 30)
						{
							m_barHour[data->symbol] = 99;
							m_barMinute[data->symbol] = 99;
							m_barmap.erase(data->symbol);
							m_hourminutemtx.unlock();
							return;
						}
					}
					OnBar(m_barmap[data->symbol]);
				}
			}
			BarData bar;

			bar.symbol = data->symbol;
			bar.exchange = data->exchange;
			bar.open = data->lastprice;
			bar.high = data->lastprice;
			bar.low = data->lastprice;
			bar.close = data->lastprice;

			bar.openPrice=data->openPrice;//今日开
			bar.highPrice=data->highPrice;//今日高
			bar.lowPrice=data->lowPrice;//今日低
			bar.preClosePrice=data->preClosePrice;//昨收

			bar.upperLimit=data->upperLimit;//涨停
			bar.lowerLimit=data->lowerLimit;//跌停

			bar.volume=data->volume;
			bar.openInterest=data->openInterest;

			bar.date = data->date;
			bar.time = data->time;
			bar.unixdatetime = data->unixdatetime;
			m_barmap[data->symbol] = bar;
			m_barMinute[data->symbol] = tickMinute;
			m_barHour[data->symbol] = tickHour;

		}
		else
		{
			m_barmap[data->symbol].high = std::max(m_barmap[data->symbol].high, data->lastprice);
			m_barmap[data->symbol].low = std::min(m_barmap[data->symbol].low, data->lastprice);
			m_barmap[data->symbol].close = data->lastprice;
			m_barmap[data->symbol].highPrice = data->highPrice;
			m_barmap[data->symbol].lowPrice = data->lowPrice;
			m_barmap[data->symbol].volume = data->volume;
		}
	}
	else if (Tick->gatewayname == "IB")
	{
		//如果是IB然后处理各种
	}
	m_hourminutemtx.unlock();
}

void Recorder::OnBar(BarData bar)
{
	std::shared_ptr<BarData>data = std::make_shared<BarData>(bar);
	//存一分钟K线
	if (m_barsymbols.find(bar.symbol) != m_barsymbols.end())
	{
		dbInsert("CTPMinuteDb", bar.symbol.c_str(), data, m_pool);
	}
}

void Recorder::OnDailyBar()
{
	auto nowtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto time1 = timetounixtimestamp(15, 0, 0);
	auto time2 = timetounixtimestamp(15, 10, 0);
	if ((nowtime > time1) && (nowtime < time2))
	{
		m_mutex.lock();
		if (!m_dailybarmap.empty())
		{
			//遍历一遍
			for (std::map<std::string, TickData>::iterator it = m_dailybarmap.begin(); it != m_dailybarmap.end(); it++)
			{
				//开高收低
				std::shared_ptr<DailyBar>data = std::make_shared<DailyBar>();
				data->open = it->second.openPrice;
				data->high = it->second.highPrice;
				data->low = it->second.lowPrice;
				data->close = it->second.lastprice;
				data->symbol = it->second.symbol;
				data->date = it->second.date;
				data->exchange = it->second.exchange;
				data->openInterest = it->second.openInterest;
				data->time = it->second.time;
				data->volume = it->second.volume;
				data->unixdatetime = it->second.unixdatetime;
				dbInsert("CTPDailyDb", data->symbol.c_str(), data, m_pool);
			}
			m_dailybarmap.clear();//清空
		}
		m_mutex.unlock();
	}
}

void Recorder::dbInsert(const char* dbname, const char* collectionname, std::shared_ptr<JSData>data, mongoc_client_pool_t *mongopool)
{
	mongoc_client_pool_t *pool = mongopool;
	mongoc_client_t      *client;

	// 从客户端池中获取一个客户端
	client = mongoc_client_pool_pop(pool);
	mongoc_collection_t *collection;
	collection = mongoc_client_get_collection(client, dbname, collectionname);
	if (data->GetDataType() == "tick")
	{
		std::shared_ptr<TickData> Tickdata = std::static_pointer_cast<TickData>(data);
		bson_error_t error;
		bson_oid_t oid;
		bson_t *doc;
		doc = bson_new();
		bson_oid_init(&oid, NULL);
		BSON_APPEND_OID(doc, "_id", &oid);

		BSON_APPEND_DOUBLE(doc, "bidVolume5", Tickdata->bidvolume5);
		BSON_APPEND_DOUBLE(doc, "bidVolume4", Tickdata->bidvolume4);
		BSON_APPEND_DOUBLE(doc, "bidVolume3", Tickdata->bidvolume3);
		BSON_APPEND_DOUBLE(doc, "bidVolume2", Tickdata->bidvolume2);
		BSON_APPEND_DOUBLE(doc, "bidVolume1", Tickdata->bidvolume1);

		BSON_APPEND_DOUBLE(doc, "askVolume1", Tickdata->askvolume1);
		BSON_APPEND_DOUBLE(doc, "askVolume2", Tickdata->askvolume2);
		BSON_APPEND_DOUBLE(doc, "askVolume3", Tickdata->askvolume3);
		BSON_APPEND_DOUBLE(doc, "askVolume4", Tickdata->askvolume4);
		BSON_APPEND_DOUBLE(doc, "askVolume5", Tickdata->askvolume5);

		BSON_APPEND_DOUBLE(doc, "askPrice5", Tickdata->askprice5);
		BSON_APPEND_DOUBLE(doc, "askPrice4", Tickdata->askprice4);
		BSON_APPEND_DOUBLE(doc, "askPrice3", Tickdata->askprice3);
		BSON_APPEND_DOUBLE(doc, "askPrice2", Tickdata->askprice2);
		BSON_APPEND_DOUBLE(doc, "askPrice1", Tickdata->askprice1);

		BSON_APPEND_DOUBLE(doc, "bidPrice5", Tickdata->bidprice5);
		BSON_APPEND_DOUBLE(doc, "bidPrice4", Tickdata->bidprice4);
		BSON_APPEND_DOUBLE(doc, "bidPrice3", Tickdata->bidprice3);
		BSON_APPEND_DOUBLE(doc, "bidPrice2", Tickdata->bidprice2);
		BSON_APPEND_DOUBLE(doc, "bidPrice1", Tickdata->bidprice1);

		BSON_APPEND_DOUBLE(doc, "lastPrice", Tickdata->lastprice);
		BSON_APPEND_DOUBLE(doc, "volume", Tickdata->volume);

		BSON_APPEND_DOUBLE(doc, "openInterest", Tickdata->openInterest);
		BSON_APPEND_DOUBLE(doc, "lowerLimit", Tickdata->lowerLimit);
		BSON_APPEND_DOUBLE(doc, "upperLimit", Tickdata->upperLimit);

		BSON_APPEND_UTF8(doc, "exchange", Tickdata->exchange.c_str());
		BSON_APPEND_UTF8(doc, "symbol", Tickdata->symbol.c_str());

		BSON_APPEND_DATE_TIME(doc, "datetime", Tickdata->unixdatetime);
		BSON_APPEND_UTF8(doc, "date", Tickdata->date.c_str());
		BSON_APPEND_UTF8(doc, "time", Tickdata->time.c_str());

		// 将bson文档插入到集合
		if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error))
		{
			std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
			e->msg = error.message;
			m_ptr_eventengine->Put(e);
		}
		bson_destroy(doc);
	}
	else if (data->GetDataType() == "bar")
	{
		std::shared_ptr<BarData> Bardata = std::static_pointer_cast<BarData>(data);
		bson_error_t error;
		bson_oid_t oid;
		bson_t *doc;
		doc = bson_new();
		bson_oid_init(&oid, NULL);
		BSON_APPEND_OID(doc, "_id", &oid);
		BSON_APPEND_UTF8(doc, "exchange", Bardata->exchange.c_str());
		BSON_APPEND_UTF8(doc, "symbol", Bardata->symbol.c_str());

		BSON_APPEND_DOUBLE(doc, "open", Bardata->open);
		BSON_APPEND_DOUBLE(doc, "high", Bardata->high);
		BSON_APPEND_DOUBLE(doc, "low", Bardata->low);
		BSON_APPEND_DOUBLE(doc, "close", Bardata->close);
		BSON_APPEND_DOUBLE(doc, "volume", Bardata->volume);

		BSON_APPEND_DATE_TIME(doc, "datetime", Bardata->unixdatetime);
		BSON_APPEND_UTF8(doc, "date", Bardata->date.c_str());
		BSON_APPEND_UTF8(doc, "time", Bardata->time.c_str());
		BSON_APPEND_DOUBLE(doc, "openInterest", Bardata->openInterest);

		BSON_APPEND_DOUBLE(doc, "openPrice", Bardata->openPrice);
		BSON_APPEND_DOUBLE(doc, "highPrice", Bardata->highPrice);
		BSON_APPEND_DOUBLE(doc, "lowPrice", Bardata->lowPrice);
		BSON_APPEND_DOUBLE(doc, "preClosePrice", Bardata->preClosePrice);

		BSON_APPEND_DOUBLE(doc, "upperLimit", Bardata->upperLimit);
		BSON_APPEND_DOUBLE(doc, "lowerLimit", Bardata->lowerLimit);

		// 将bson文档插入到集合
		if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error))
		{
			std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
			e->msg = error.message;
			m_ptr_eventengine->Put(e);
		}
		bson_destroy(doc);

		std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
		e->msg = "合约:" + Bardata->symbol + "开" + Utils::doubletostring(Bardata->open) + "收"+Utils::doubletostring(Bardata->close);
		m_ptr_eventengine->Put(e);

	}
	else if (data->GetDataType() == "dailybar")
	{
		std::shared_ptr<DailyBar> DailyBardata = std::static_pointer_cast<DailyBar>(data);
		bson_error_t error;
		bson_oid_t oid;
		bson_t *doc;
		doc = bson_new();
		bson_oid_init(&oid, NULL);
		BSON_APPEND_OID(doc, "_id", &oid);

		BSON_APPEND_UTF8(doc, "exchange", DailyBardata->exchange.c_str());
		BSON_APPEND_UTF8(doc, "symbol", DailyBardata->symbol.c_str());

		BSON_APPEND_DOUBLE(doc, "open", DailyBardata->open);
		BSON_APPEND_DOUBLE(doc, "high", DailyBardata->high);
		BSON_APPEND_DOUBLE(doc, "low", DailyBardata->low);
		BSON_APPEND_DOUBLE(doc, "close", DailyBardata->close);
		BSON_APPEND_DOUBLE(doc, "volume", DailyBardata->volume);


		BSON_APPEND_DATE_TIME(doc, "datetime", DailyBardata->unixdatetime);
		BSON_APPEND_UTF8(doc, "date", DailyBardata->date.c_str());
		BSON_APPEND_UTF8(doc, "time", DailyBardata->time.c_str());
		BSON_APPEND_DOUBLE(doc, "openInterest", DailyBardata->openInterest);

		// 将bson文档插入到集合
		if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error))
		{
			std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
			e->msg = error.message;
			m_ptr_eventengine->Put(e);
		}
		bson_destroy(doc);
	}
	mongoc_client_pool_push(pool, client);
}
///将CTP时间转化
const long long Recorder::getsystemunixdatetime(std::string time, std::string type)
{
	std::vector<std::string>v;
	char* pch = strtok(const_cast<char*>(time.c_str()), ":.");
	while (pch != NULL)
	{
		v.push_back(pch);
		pch = strtok(NULL, ":.");
	}
	if (v.size() == 4)
	{
		auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获取UNIX时间戳
		struct tm *l = localtime(&tt);//将UNIX时间戳转TM结构
		l->tm_hour = atoi(v[0].c_str());
		l->tm_min = atoi(v[1].c_str());
		l->tm_sec = atoi(v[2].c_str());
		time_t ft = mktime(l);
		if (type == "s")
		{
			return (ft);//秒
		}
		else if (type == "ms")
		{
			return (ft * 1000 + atoi(v[3].c_str()) * 100);//转换成毫秒
		}
	}
	else
	{
		//时间错误
		std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
		e->msg = "时间错误";
		m_ptr_eventengine->Put(e);
	}
}

void Recorder::autoconnect()
{
	//判断一下当前时间是不是在早上3点到8点50之间
	auto nowtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获取当前的系统时间
	if (((nowtime > timetounixtimestamp(3, 0, 0)) && (nowtime < timetounixtimestamp(8, 50, 0))) 
		|| ((nowtime > timetounixtimestamp(15, 30, 0)) && (nowtime < timetounixtimestamp(20, 50, 0))))
	{
		if (m_connectstatus == true)
		{
			m_connectstatus = false;
			m_mutex.lock();
			std::cout << "CTP接口主动断开连接！" << std::endl;
			m_ptr_Recordermanager->close("CTP");//主动断开连接
			m_mutex.unlock();
		}
	}
	else
	{
		//连接
		if (m_connectstatus == false)
		{
			m_connectstatus = true;
			m_ptr_Recordermanager->connect("CTP");
		}
	}
}
