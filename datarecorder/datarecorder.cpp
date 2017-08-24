#include"datarecorder.h"
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif
Datarecorder::Datarecorder(EventEngine *eventengine) :connectStatus(false)
{
	my_logger = spdlog::rotating_logger_mt("Datarecorder", "logs/datarecorder.txt", 1048576 * 10, 3);
	my_logger->flush_on(spdlog::level::info);
	this->eventengine = eventengine;
	this->ctpgateway = new Ctpmd(eventengine);
	eventengine->regEvent(EVENT_LOG, std::bind(&Datarecorder::showLog, this, std::placeholders::_1));
	eventengine->regEvent(EVENT_TICK, std::bind(&Datarecorder::onTick, this, std::placeholders::_1));
	eventengine->regEvent(EVENT_TIMER, std::bind(&Datarecorder::onDailybar, this));
	eventengine->regEvent(EVENT_TIMER, std::bind(&Datarecorder::autoConnect, this));
	mongoc_init();
	this->uri = mongoc_uri_new("mongodb://localhost/");
	// 创建客户端池
	this->pool = mongoc_client_pool_new(uri);
	std::vector<std::string>ninetoeleven = { "bu", "rb", "hc", "ru" };
	this->ninetoeleven.insert(ninetoeleven.begin(), ninetoeleven.end()); //9点到11点的合约列表
	std::vector<std::string>ninetohalfeleven = { "p", "j", "m", "y", "a", "b", "jm", "i", "SR", "CF", "RM", "MA", "ZC", "FG", "OI" };
	this->ninetohalfeleven.insert(ninetohalfeleven.begin(), ninetohalfeleven.end()); //9点到11点半的合约
	std::vector<std::string>ninetoone = { "cu", "al", "zn", "pb", "sn", "ni" };
	this->ninetoone.insert(ninetoone.begin(), ninetoone.end()); //9点到1点的合约列表
	std::vector<std::string>ninetohalftwo = { "ag", "au" };
	this->ninetohalftwo.insert(ninetohalftwo.begin(), ninetohalftwo.end()); //9点到2点半的合约
}

Datarecorder::~Datarecorder()
{
	this->ctpgateway->close();
	delete this->ctpgateway;
	mongoc_client_pool_destroy(this->pool);
	mongoc_uri_destroy(this->uri);
	mongoc_cleanup();
}

void Datarecorder::readSymbols()
{
	if (Utils::checkExist("./CTPGateway")) {
		std::fstream file("./CTPGateway/symbols.txt", std::ios::in);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line)) {
				this->conf_symbols.insert(line);
			}
		}
		else {
			this->writeLog("/CTPGateway/symbols.txt不存在，无法读取合约列表");
		}
		file.close();
	}
	else {
		this->writeLog("CTPGateway目录不存在，无法读取合约列表");
	}
	for (std::set<std::string>::iterator iter = this->conf_symbols.begin(); iter != this->conf_symbols.end(); ++iter) {
		this->barHourmap[*iter] = 99;
		this->barMinutemap[*iter] = 99;
		std::cout << Utils::UTF8_2_GBK("开始订阅合约:") << *iter << std::endl;
		this->ctpgateway->subscribe(*iter);//订阅一个合约
	}
}

inline void Datarecorder::writeLog(const std::string& msg) const
{
	std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
	e->msg = msg;
	e->gatewayname = "ctp";
	e->logTime = Utils::getCurrentDateTime();
	this->eventengine->put(e);
}

inline void Datarecorder::showLog(std::shared_ptr<Event>e)
{
	std::unique_lock<std::mutex>lck(log_mutex);
	std::shared_ptr<Event_Log>elog = std::static_pointer_cast<Event_Log>(e);
	std::cout << Utils::UTF8_2_GBK("时间:") << elog->logTime << Utils::UTF8_2_GBK("接口:") << Utils::UTF8_2_GBK(elog->gatewayname) << Utils::UTF8_2_GBK("消息:") << Utils::UTF8_2_GBK(elog->msg) << std::endl;
	if (elog->msg == "行情服务器登录完成") {
		this->readSymbols();//开始订阅合约
		this->writeLog("开始获取合约并订阅");
	}
}

void Datarecorder::autoConnect()
{
	//判断一下当前时间是不是在早上3点到8点50之间
	auto nowtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获取当前的系统时间
	if (((nowtime > Utils::timeTotime_t(2, 30, 0)) && (nowtime < Utils::timeTotime_t(8, 58, 0)))
		|| ((nowtime > Utils::timeTotime_t(15, 01, 0)) && (nowtime < Utils::timeTotime_t(20, 58, 0)))) {
		if (this->connectStatus == true) {
			this->connectStatus = false;
			this->writeLog("CTP接口主动(断开连接)");
			this->ctpgateway->close();
		}
	}
	else {
		if (this->connectStatus == false)
		{
			if (Utils::getWeekDay(Utils::getCurrentDate()) != 6 && Utils::getWeekDay(Utils::getCurrentDate()) != 7)
			{
				this->connectStatus = true;
				this->writeLog("CTP接口主动(开始连接)");
				this->ctpgateway->connect();
			}
		}
	}
}

void Datarecorder::onTick(std::shared_ptr<Event>e)
{
	std::shared_ptr<Event_Tick> Tick = std::static_pointer_cast<Event_Tick>(e);
	//过滤CTP的时间***************************************************************************************************************************
	auto ticktime_t = Tick->getTime_t();
	auto systemtime_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获取当前的系统时间
	std::string symbolHead = Utils::regexSymbol(Tick->symbol);
	if (this->ninetoeleven.find(symbolHead) != this->ninetoeleven.end()) {
		if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(23, 0, 0))))) {
			//在9点到11点里头的对立事件则返回
			return;
		}
		else {
			//数据包的时间是在交易时间段的在判断一下真实时间是否也在交易时间段
			if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(23, 0, 0))))) {
				return;
			}
		}
	}
	else if (this->ninetohalfeleven.find(symbolHead) != this->ninetohalfeleven.end()) {
		if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(23, 30, 0))))) {
			return;
		}
		else {
			if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(23, 30, 0))))) {
				return;
			}
		}
	}
	else if (this->ninetoone.find(symbolHead) != this->ninetoone.end()) {
		if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(24, 0, 0))) || (ticktime_t <= Utils::timeTotime_t(1, 0, 0)))) {
			return;
		}
		else {
			if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(24, 0, 0))) || (systemtime_t <= Utils::timeTotime_t(1, 0, 0)))) {
				return;
			}
		}
	}
	else if (this->ninetohalftwo.find(symbolHead) != this->ninetohalftwo.end()) {
		if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(24, 0, 0))) || (ticktime_t <= Utils::timeTotime_t(2, 30, 0)))) {
			return;
		}
		else {
			if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(24, 0, 0))) || (systemtime_t <= Utils::timeTotime_t(2, 30, 0)))) {
				return;
			}
		}
	}
	else {
		//如果只是日盘没有夜盘的合约，就判断一下发过来的时间是否是在日盘交易时间之内
		if (!(ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) {
			return;
		}
		else {
			if (!(systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) {
				return;
			}
		}
	}
	//更新日线缓存，并插入tick数据

	this->dailyBarmap[Tick->symbol] = Tick;//每次更新
	this->insertTicktoDb("CTPTickDb", Tick->symbol, Tick);
	if (Tick->getMinute() != this->barMinutemap.at(Tick->symbol) || Tick->getHour() != this->barHourmap.at(Tick->symbol)) {
		if (!((this->barHourmap.at(Tick->symbol) == 11 && this->barMinutemap.at(Tick->symbol) == 30) || (this->barHourmap.at(Tick->symbol) == 15 && this->barMinutemap.at(Tick->symbol) == 00) || (this->barHourmap.at(Tick->symbol) == 10 && this->barMinutemap.at(Tick->symbol) == 15))) { //剔除10点15分11点半下午3点的一根TICK合成出来的K线
			if (this->ninetoeleven.find(symbolHead) != this->ninetoeleven.end()) {
				if (this->barHourmap.at(Tick->symbol) == 23) {
					this->barHourmap.at(Tick->symbol) = 99;
					this->barmap.erase(Tick->symbol);
					return;
				}
			}
			else if (this->ninetohalfeleven.find(symbolHead) != this->ninetohalfeleven.end()) {
				if (this->barHourmap.at(Tick->symbol) == 23 && this->barMinutemap.at(Tick->symbol) == 30) {
					this->barHourmap.at(Tick->symbol) = 99;
					this->barMinutemap.at(Tick->symbol) = 99;
					this->barmap.erase(Tick->symbol);
					return;
				}
			}
			else if (this->ninetoone.find(symbolHead) != this->ninetoone.end()) {
				if (this->barHourmap.at(Tick->symbol) == 1) {
					this->barHourmap.at(Tick->symbol) = 99;
					this->barmap.erase(Tick->symbol);
					return;
				}
			}
			else if (this->ninetohalftwo.find(symbolHead) != this->ninetohalftwo.end()) {
				if (this->barHourmap.at(Tick->symbol) == 2 && this->barMinutemap.at(Tick->symbol) == 30) {
					this->barHourmap.at(Tick->symbol) = 99;
					this->barMinutemap.at(Tick->symbol) = 99;
					this->barmap.erase(Tick->symbol);
					return;
				}
			}
			if (this->barmap.find(Tick->symbol) != this->barmap.end()) {
				onBar(this->barmap.at(Tick->symbol));
			}
		}
		jsstructs::BarData bar;
		bar.symbol = Tick->symbol;
		bar.exchange = Tick->exchange;
		bar.open = Tick->lastprice;
		bar.high = Tick->lastprice;
		bar.low = Tick->lastprice;
		bar.close = Tick->lastprice;
		bar.openPrice = Tick->openPrice;//今日开
		bar.highPrice = Tick->highPrice;//今日高
		bar.lowPrice = Tick->lowPrice;//今日低
		bar.preClosePrice = Tick->preClosePrice;//昨收
		bar.upperLimit = Tick->upperLimit;//涨停
		bar.lowerLimit = Tick->lowerLimit;//跌停
		bar.volume = Tick->volume;
		bar.openInterest = Tick->openInterest;
		bar.date = Tick->date;
		bar.time = Tick->time;
		bar.setUnixDatetime();
		this->barmap[Tick->symbol] = bar;
		this->barMinutemap[Tick->symbol] = Tick->getMinute();
		this->barHourmap[Tick->symbol] = Tick->getHour();
	}
	else {
		this->barmap[Tick->symbol].high = std::max(this->barmap[Tick->symbol].high, Tick->lastprice);
		this->barmap[Tick->symbol].low = std::min(this->barmap[Tick->symbol].low, Tick->lastprice);
		this->barmap[Tick->symbol].close = Tick->lastprice;
		this->barmap[Tick->symbol].highPrice = Tick->highPrice;
		this->barmap[Tick->symbol].lowPrice = Tick->lowPrice;
		this->barmap[Tick->symbol].openInterest = Tick->openInterest;
		this->barmap[Tick->symbol].volume += Tick->volume;
	}
}

void Datarecorder::onBar(const jsstructs::BarData &bar)
{
	mongoc_client_t     *client = mongoc_client_pool_pop(this->pool);
	mongoc_collection_t *collection = mongoc_client_get_collection(client, "CTPMinuteDb", bar.symbol.c_str());
	bson_error_t error;
	bson_t *doc = bson_new();
	auto milliseconds = Utils::getMilliseconds();
	std::string lastmilliseconds = milliseconds.substr(milliseconds.size() - 3);
	long long id = std::stoll(std::to_string(bar.getTime_t()) + lastmilliseconds);
	BSON_APPEND_INT64(doc, "_id", id);
	BSON_APPEND_UTF8(doc, "exchange", bar.exchange.c_str());
	BSON_APPEND_UTF8(doc, "symbol", bar.symbol.c_str());
	BSON_APPEND_DOUBLE(doc, "open", bar.open);
	BSON_APPEND_DOUBLE(doc, "high", bar.high);
	BSON_APPEND_DOUBLE(doc, "low", bar.low);
	BSON_APPEND_DOUBLE(doc, "close", bar.close);
	BSON_APPEND_DOUBLE(doc, "volume", bar.volume);
	BSON_APPEND_DATE_TIME(doc, "datetime", id);
	BSON_APPEND_UTF8(doc, "date", bar.date.c_str());
	BSON_APPEND_UTF8(doc, "time", bar.time.c_str());
	BSON_APPEND_DOUBLE(doc, "openInterest", bar.openInterest);
	BSON_APPEND_DOUBLE(doc, "openPrice", bar.openPrice);
	BSON_APPEND_DOUBLE(doc, "highPrice", bar.highPrice);
	BSON_APPEND_DOUBLE(doc, "lowPrice", bar.lowPrice);
	BSON_APPEND_DOUBLE(doc, "preClosePrice", bar.preClosePrice);
	BSON_APPEND_DOUBLE(doc, "upperLimit", bar.upperLimit);
	BSON_APPEND_DOUBLE(doc, "lowerLimit", bar.lowerLimit);
	// 将bson文档插入到集合
	if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
		this->writeLog("mongoc Bar insert failed");
	}
	bson_destroy(doc);
	mongoc_collection_destroy(collection);
	mongoc_client_pool_push(this->pool, client);
	this->writeLog("合约:" + bar.symbol + "开" + std::to_string(bar.open) + "高" + std::to_string(bar.high) + "低" + std::to_string(bar.low) + "收" + std::to_string(bar.close));
}

void Datarecorder::onDailybar()
{
	std::unique_lock<std::mutex>lck(dailyBarmtx);
	auto nowtime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	auto close_time_begin = Utils::timeTotime_t(15, 0, 0);
	auto close_time_end = Utils::timeTotime_t(15, 10, 0);
	if ((nowtime > close_time_begin) && (nowtime < close_time_end)) {
		if (!this->dailyBarmap.empty()) {
			//遍历一遍
			for (std::map<std::string, std::shared_ptr<Event_Tick>>::iterator it = this->dailyBarmap.begin(); it != this->dailyBarmap.end(); ++it) {
				mongoc_client_t     *client = mongoc_client_pool_pop(this->pool);
				mongoc_collection_t *collection = mongoc_client_get_collection(client, "CTPDailyDb", it->second->symbol.c_str());
				bson_error_t error;
				bson_t *doc = bson_new();
				auto milliseconds = Utils::getMilliseconds();
				std::string lastmilliseconds = milliseconds.substr(milliseconds.size() - 3);
				long long id = std::stoll(std::to_string(it->second->getTime_t()) + lastmilliseconds);
				BSON_APPEND_INT64(doc, "_id", id);
				BSON_APPEND_DOUBLE(doc, "open", it->second->openPrice);
				BSON_APPEND_DOUBLE(doc, "high", it->second->highPrice);
				BSON_APPEND_DOUBLE(doc, "low", it->second->lowPrice);
				BSON_APPEND_DOUBLE(doc, "close", it->second->lastprice);
				BSON_APPEND_UTF8(doc, "symbol", it->second->symbol.c_str());
				BSON_APPEND_UTF8(doc, "exchange", it->second->exchange.c_str());
				BSON_APPEND_UTF8(doc, "date", it->second->date.c_str());
				BSON_APPEND_UTF8(doc, "time", it->second->time.c_str());
				BSON_APPEND_DOUBLE(doc, "openInterest", it->second->openInterest);
				BSON_APPEND_DOUBLE(doc, "volume", it->second->volume);
				BSON_APPEND_DATE_TIME(doc, "datetime", id);
				// 将bson文档插入到集合
				if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
					this->writeLog("mongoc insert dailybar failed");
				}
				bson_destroy(doc);
				mongoc_collection_destroy(collection);
				mongoc_client_pool_push(this->pool, client);
			}
		}
		this->dailyBarmap.clear();//清空
	}
}

void Datarecorder::insertTicktoDb(const std::string &dbname, const std::string &symbol, std::shared_ptr<Event_Tick> tick)
{
	std::unique_lock<std::mutex>lck(tick_mutex);
	std::vector<std::string>time_milliseconds = Utils::split(tick->time, ".");
	if (time_milliseconds.size() == 2)
	{
		std::string lastmilliseconds = time_milliseconds.back()+"00";
		long long id = std::stoll(std::to_string(tick->getTime_t()) + lastmilliseconds);
		if (this->lastTickIdmap.find(symbol) != this->lastTickIdmap.end())
		{
			if (this->lastTickIdmap.at(symbol) == id)
			{
				this->writeLog(symbol + "收到了重复的时间戳" + std::to_string(id));
				return;
			}
		}
		mongoc_client_t     *client = mongoc_client_pool_pop(this->pool);
		mongoc_collection_t *collection = mongoc_client_get_collection(client, dbname.c_str(), symbol.c_str());
		bson_error_t error;
		bson_t *doc = bson_new();
		this->lastTickIdmap[symbol] = id;
		BSON_APPEND_INT64(doc, "_id", id);
		BSON_APPEND_DOUBLE(doc, "bidVolume5", tick->bidvolume5);
		BSON_APPEND_DOUBLE(doc, "bidVolume4", tick->bidvolume4);
		BSON_APPEND_DOUBLE(doc, "bidVolume3", tick->bidvolume3);
		BSON_APPEND_DOUBLE(doc, "bidVolume2", tick->bidvolume2);
		BSON_APPEND_DOUBLE(doc, "bidVolume1", tick->bidvolume1);
		BSON_APPEND_DOUBLE(doc, "askVolume1", tick->askvolume1);
		BSON_APPEND_DOUBLE(doc, "askVolume2", tick->askvolume2);
		BSON_APPEND_DOUBLE(doc, "askVolume3", tick->askvolume3);
		BSON_APPEND_DOUBLE(doc, "askVolume4", tick->askvolume4);
		BSON_APPEND_DOUBLE(doc, "askVolume5", tick->askvolume5);
		BSON_APPEND_DOUBLE(doc, "askPrice5", tick->askprice5);
		BSON_APPEND_DOUBLE(doc, "askPrice4", tick->askprice4);
		BSON_APPEND_DOUBLE(doc, "askPrice3", tick->askprice3);
		BSON_APPEND_DOUBLE(doc, "askPrice2", tick->askprice2);
		BSON_APPEND_DOUBLE(doc, "askPrice1", tick->askprice1);
		BSON_APPEND_DOUBLE(doc, "bidPrice5", tick->bidprice5);
		BSON_APPEND_DOUBLE(doc, "bidPrice4", tick->bidprice4);
		BSON_APPEND_DOUBLE(doc, "bidPrice3", tick->bidprice3);
		BSON_APPEND_DOUBLE(doc, "bidPrice2", tick->bidprice2);
		BSON_APPEND_DOUBLE(doc, "bidPrice1", tick->bidprice1);
		BSON_APPEND_DOUBLE(doc, "lastPrice", tick->lastprice);
		BSON_APPEND_DOUBLE(doc, "volume", tick->volume);
		BSON_APPEND_DOUBLE(doc, "openInterest", tick->openInterest);
		BSON_APPEND_DOUBLE(doc, "lowerLimit", tick->lowerLimit);
		BSON_APPEND_DOUBLE(doc, "upperLimit", tick->upperLimit);
		BSON_APPEND_UTF8(doc, "exchange", tick->exchange.c_str());
		BSON_APPEND_UTF8(doc, "symbol", tick->symbol.c_str());
		BSON_APPEND_DATE_TIME(doc, "datetime", id);
		BSON_APPEND_UTF8(doc, "date", tick->date.c_str());
		BSON_APPEND_UTF8(doc, "time", tick->time.c_str());
		// 将bson文档插入到集合
		if (!mongoc_collection_insert(collection, MONGOC_INSERT_NONE, doc, NULL, &error)) {
			this->writeLog("mongoc insert failed");
			fprintf(stderr, "Count failed: %s\n", error.message);
		}
		bson_destroy(doc);
		mongoc_collection_destroy(collection);
		mongoc_client_pool_push(this->pool, client);
	}
	else
	{
		my_logger->error("insert Tick DB failed symbol:{} date:{} time:{} ", tick->symbol, tick->date, tick->time);
	}
}
