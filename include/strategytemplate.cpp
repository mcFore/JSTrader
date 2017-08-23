#include"strategytemplate.h"
void StrategyData::insertParam(const std::string &key, const std::string &value)
{
	std::unique_lock<std::mutex>lck(mtx);
	this->paramMap[key] = value;
}

void StrategyData::insertVar(const std::string &key, const std::string &value)
{
	std::unique_lock<std::mutex>lck(mtx);
	this->varMap[key] = value;
}

std::string StrategyData::getParam(const std::string& key)
{
	std::unique_lock<std::mutex>lck(mtx);
	if (this->paramMap.find(key) != this->paramMap.end())
	{
		return this->paramMap[key];
	}
	else
	{
		return "Null";
	}
}

std::string StrategyData::getVar(const std::string &key)
{
	std::unique_lock<std::mutex>lck(mtx);
	if (this->varMap.find(key) != this->varMap.end())
	{
		return this->varMap[key];
	}
	else
	{
		return "Null";
	}
}

inline std::map<std::string, std::string>StrategyData::getParamMap() const
{
	std::unique_lock<std::mutex>lck(mtx);
	return this->paramMap;
}

inline std::map<std::string, std::string>StrategyData::getVarMap() const
{
	std::unique_lock<std::mutex>lck(mtx);
	return this->varMap;
}

StrategyTemplate::StrategyTemplate(AlgoTradingAPI *algotrading) :inited(false), trading(false)
{
	this->algotrading = algotrading;
	this->algorithmorder = new AlgorithmOrder(this);
	std::vector<std::string>ninetoeleven = { "bu", "rb", "hc", "ru" };
	this->ninetoeleven.insert(ninetoeleven.begin(), ninetoeleven.end()); //9点到11点的合约列表
	std::vector<std::string>ninetohalfeleven = { "p", "j", "m", "y", "a", "b", "jm", "i", "SR", "CF", "RM", "MA", "ZC", "FG", "OI" };
	this->ninetohalfeleven.insert(ninetohalfeleven.begin(), ninetohalfeleven.end()); //9点到11点半的合约
	std::vector<std::string>ninetoone = { "cu", "al", "zn", "pb", "sn", "ni" };
	this->ninetoone.insert(ninetoone.begin(), ninetoone.end()); //9点到1点的合约列表
	std::vector<std::string>ninetohalftwo = { "ag", "au" };
	this->ninetohalftwo.insert(ninetohalftwo.begin(), ninetohalftwo.end()); //9点到2点半的合约
}

StrategyTemplate::~StrategyTemplate()
{
	delete this->algorithmorder;
}

void  StrategyTemplate::onInit()
{
	this->algotrading->writeAlgoTradingLog("策略初始化" + this->getParam("name"));
	std::vector<std::string>symbollist = Utils::split(this->getParam("symbol"), ",");
	if (this->getParam("mode") == "bar")
	{
		std::vector<jsstructs::BarData>alldatalist;
		for (std::vector<std::string>::const_iterator it = symbollist.cbegin(); it != symbollist.cend(); ++it)
		{
			std::vector<jsstructs::BarData>datalist = this->algotrading->loadBar(this->barDbName, *it, std::stoi(this->getParam("initdays")));
			for (std::vector<jsstructs::BarData>::const_iterator it = datalist.cbegin(); it != datalist.cend(); ++it)
			{
				alldatalist.push_back(*it);
			}
		}

		std::sort(alldatalist.begin(), alldatalist.end(), BarGreater());

		for (std::vector<jsstructs::BarData>::const_iterator it = alldatalist.cbegin(); it != alldatalist.cend(); ++it)
		{
			if (it == alldatalist.cend() - 1)
			{
				this->onBar_template(*it, true);
			}
			else
			{
				this->onBar_template(*it, false);
			}
		}
	}
	else if (this->getParam("mode") == "tick")
	{
		std::vector<std::shared_ptr<Event_Tick>>alldatalist;
		for (std::vector<std::string>::const_iterator it = symbollist.cbegin(); it != symbollist.cend(); ++it)
		{
			std::vector<std::shared_ptr<Event_Tick>>datalist = this->algotrading->loadTick(tickDbName, *it, std::stoi(this->getParam("initdays")));
			for (std::vector<std::shared_ptr<Event_Tick>>::const_iterator it = datalist.cbegin(); it != datalist.cend(); ++it)
			{
				alldatalist.push_back(*it);
			}
		}

		std::sort(alldatalist.begin(), alldatalist.end(), TickGreater());

		for (std::vector<std::shared_ptr<Event_Tick>>::const_iterator it = alldatalist.cbegin(); it != alldatalist.cend(); ++it)
		{
			this->onTick_template(*it);
		}
	}
	this->loadMongoDB();  //读取持仓
	std::map<std::string, double>map = this->getPosMap();
	for (std::map<std::string, double>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		this->algorithmorder->setTargetPos(iter->first, iter->second);
	}
	this->inited = true;
	this->putEvent();
}

void  StrategyTemplate::onStart()
{
	this->trading = true;
	this->algotrading->writeAlgoTradingLog("策略" + this->getParam("name") + "启动");
	this->putEvent();
}

void  StrategyTemplate::onStop()
{
	this->trading = false;
	this->algotrading->writeAlgoTradingLog("策略" + this->getParam("name") + "停止");
	this->putEvent();
}

void  StrategyTemplate::cancelOrder(const std::string &orderID)
{
	this->algotrading->cancelOrder(orderID, "ctp");
}

void  StrategyTemplate::cancelOrders()
{
	std::unique_lock<std::mutex>lck(orderListmtx);
	for (std::vector<std::string>::iterator it = this->orderList.begin(); it != this->orderList.end(); ++it)
	{
		this->cancelOrder(*it);
	}
}

void  StrategyTemplate::onTick_template(const std::shared_ptr<Event_Tick>&tick)
{
	if ((tick->getMinute() != this->minute) || tick->getHour() != this->hour)
	{
		if (this->Bar.close != 0)
		{
			if (!((this->hour == 11 && this->minute == 30) || (this->hour == 15 && this->minute == 00) || (this->hour == 10 && this->minute == 15)))
			{
				std::string symbol_head = Utils::regexSymbol(this->Bar.symbol);
				if (ninetoeleven.find(symbol_head) != ninetoeleven.end())
				{
					if (!(this->hour == 23 && this->minute == 00))
					{
						this->onBar_template(this->Bar, true);
					}
				}
				else if (ninetohalfeleven.find(symbol_head) != ninetohalfeleven.end())
				{
					if (!(this->hour == 23 && this->minute == 30))
					{
						this->onBar_template(this->Bar, true);
					}
				}
				else if (ninetoone.find(symbol_head) != ninetoone.end())
				{
					if (!(this->hour == 1 && this->minute == 00))
					{
						this->onBar_template(this->Bar, true);
					}
				}
				else if (ninetohalftwo.find(symbol_head) != ninetohalftwo.end())
				{
					if (!(this->hour == 2 && this->minute == 30))
					{
						this->onBar_template(this->Bar, true);
					}
				}
				else
				{
					this->onBar_template(this->Bar, true);
				}}
		}
		jsstructs::BarData bar;
		bar.symbol = tick->symbol;
		bar.exchange = tick->exchange;
		bar.open = tick->lastprice;
		bar.high = tick->lastprice;
		bar.low = tick->lastprice;
		bar.close = tick->lastprice;
		bar.highPrice = tick->highPrice;
		bar.lowPrice = tick->lowPrice;
		bar.upperLimit = tick->upperLimit;
		bar.lowerLimit = tick->lowerLimit;
		bar.openInterest = tick->openInterest;
		bar.openPrice = tick->openPrice;
		bar.volume = tick->volume;
		bar.date = tick->date;
		bar.time = tick->time;
		bar.setUnixDatetime();
		this->Bar = bar;
		this->hour = tick->getHour();
		this->minute = tick->getMinute();
	}
	else
	{
		this->Bar.high = std::max(this->Bar.high, tick->lastprice);
		this->Bar.low = std::min(this->Bar.low, tick->lastprice);
		this->Bar.close = tick->lastprice;
		this->Bar.volume = tick->volume;
		this->Bar.highPrice = tick->highPrice;
		this->Bar.lowPrice = tick->lowPrice;
	}
	this->onTick(tick);
	this->putEvent();
}

void  StrategyTemplate::onTick(const std::shared_ptr<Event_Tick>&tick)
{

}

void  StrategyTemplate::onBar_template(const jsstructs::BarData &bar, bool inited)
{
	onBar(bar);
	this->algotrading->PutBarChart(bar, this->backtestgoddata, inited);
}

void  StrategyTemplate::onBar(const jsstructs::BarData &bar)
{

}

void  StrategyTemplate::onOrder_template(const std::shared_ptr<Event_Order>&order)
{
	if (order->status == STATUS_CANCELLED)
	{
		this->algorithmorder->setCallback();
	}
	onOrder(order);
}

void  StrategyTemplate::onTrade_template(const std::shared_ptr<Event_Trade>&trade)
{
	this->algorithmorder->setCallback();
	onTrade(trade);
	saveMongoDB();
}

void  StrategyTemplate::onOrder(const std::shared_ptr<Event_Order>&order)
{

}

void  StrategyTemplate::onTrade(const std::shared_ptr<Event_Trade>&trade)
{

}

void  StrategyTemplate::addParam(const std::string &paramName, const std::string &paramValue)
{
	this->strategyData.insertParam(paramName, paramValue);
}

std::string  StrategyTemplate::getParam(const std::string &paramName)
{
	return this->strategyData.getParam(paramName);
}

void  StrategyTemplate::modifyPos(const std::string &symbol, double pos)
{
	std::unique_lock<std::mutex>lck(posMapmtx);
	this->posMap[symbol] = pos;
}

double  StrategyTemplate::getPos(const std::string &symbol)
{
	std::unique_lock<std::mutex>lck(posMapmtx);
	if (this->posMap.find(symbol) != this->posMap.end())
	{
		return this->posMap[symbol];
	}
	else
	{
		return 0;
	}
}

std::map<std::string, double> StrategyTemplate::getPosMap()
{
	std::unique_lock<std::mutex>lck(posMapmtx);
	return this->posMap;
}

//做多
std::vector<std::string>  StrategyTemplate::buy(const std::string &symbol, double price, double volume)
{
	return this->sendOrder(symbol, ALGOBUY, price, volume);
}
//平多
std::vector<std::string>  StrategyTemplate::sell(const std::string &symbol, double price, double volume)
{
	return this->sendOrder(symbol, ALGOSELL, price, volume);
}
//做空
std::vector<std::string>  StrategyTemplate::sellshort(const std::string &symbol, double price, double volume)
{
	return this->sendOrder(symbol, ALGOSHORT, price, volume);
}
//平空
std::vector<std::string>  StrategyTemplate::buycover(const std::string &symbol, double price, double volume)
{
	return this->sendOrder(symbol, ALGOCOVER, price, volume);
}

void StrategyTemplate::modifyVar(double value, const std::string &varname)
{
	if(name_mapping_var.find(varname) != name_mapping_var.end())
	{
		name_mapping_var.at(varname) = value;
	}
}

void StrategyTemplate::showStrategy()
{
	this->strategyData.insertVar("inited", Utils::Bool2String(inited));
	this->strategyData.insertVar("trading", Utils::Bool2String(trading));
	std::map<std::string, double>map = getPosMap();
	for (std::map<std::string, double>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		this->strategyData.insertVar(("pos_" + iter->first), std::to_string(iter->second));
	}

	std::shared_ptr<Event_LoadStrategy>e = std::make_shared<Event_LoadStrategy>();
	e->parammap = strategyData.getParamMap();
	e->varmap = strategyData.getVarMap();
	e->strategyname = strategyData.getParam("name");
	this->algotrading->PutEvent(e);
}

void  StrategyTemplate::putEvent()
{
	this->strategyData.insertVar("inited", Utils::Bool2String(inited));
	this->strategyData.insertVar("trading", Utils::Bool2String(trading));
	std::map<std::string, double>map = getPosMap();
	for (std::map<std::string, double>::iterator iter = map.begin(); iter != map.end(); iter++)
	{
		this->strategyData.insertVar(("pos_" + iter->first), std::to_string(iter->second));
	}
	//将参数和变量传递到界面上去
	std::shared_ptr<Event_UpdateStrategy>e = std::make_shared<Event_UpdateStrategy>();
	e->parammap = this->strategyData.getParamMap();
	e->varmap = this->strategyData.getVarMap();
	e->strategyname = this->getParam("name");
	this->algotrading->PutEvent(e);
}

std::vector<std::string>  StrategyTemplate::sendOrder(const std::string &symbol, const std::string &orderType, double price, double volume)
{
	std::vector<std::string>orderIDv;
	if (trading == true)
	{
		orderIDv = this->algotrading->sendOrder(symbol, orderType, price, volume, this);
		for (std::vector<std::string>::const_iterator it = orderIDv.cbegin(); it != orderIDv.cend(); ++it)
		{
			std::unique_lock<std::mutex>lck(orderListmtx);
			this->orderList.push_back(*it);
		}
		return orderIDv;
	}
	return orderIDv;
}

void  StrategyTemplate::saveMongoDB()
{
	bson_t *query;
	bson_t *update;
	query = BCON_NEW("strategyname", BCON_UTF8(this->strategyData.getParam("name").c_str()));
	std::map<std::string, double>map = this->getPosMap();
	for (std::map<std::string, double>::iterator it = map.begin(); it != map.end(); it++)
	{
		update = BCON_NEW("$set", "{", BCON_UTF8(it->first.c_str()), BCON_DOUBLE(it->second), "}");
		this->algotrading->mongocxx->updateData(query, update, "StrategyPos", "pos");
	}
}

void  StrategyTemplate::loadMongoDB()
{
	std::vector<std::string>result;

	bson_t query;

	bson_init(&query);

	this->algotrading->mongocxx->append_utf8(&query, "strategyname", this->strategyData.getParam("name").c_str());

	result = this->algotrading->mongocxx->findData(&query, "StrategyPos", "pos");

	for (std::vector<std::string>::iterator iter = result.begin(); iter != result.end(); iter++)
	{
		std::string s = (*iter);
		std::string err;


		auto json_parsed = json11::Json::parse(s, err);
		if (!err.empty())
		{
			return;
		}
		auto amap = json_parsed.object_items();

		std::map<std::string, double>temp_posmap = this->getPosMap();
		for (std::map<std::string, json11::Json>::iterator it = amap.begin(); it != amap.end(); it++)
		{
			if (temp_posmap.find(it->first) != temp_posmap.end())
			{
				this->modifyPos(it->first, it->second.number_value());
			}
		}
	}
}