#pragma once
#ifndef STRATEGYTEMPLATE_H
#define STRATEGYTEMPALTE_H
#include<set>
#include"talib.h"
#include"algotradingAPI.h"
#include"algorithmorder.h"
class StrategyData
{
public:
	void insertParam(const std::string &key, const std::string &value);
	void insertVar(const std::string &key, const std::string &value);
	std::string getParam(const std::string& key);
	std::string getVar(const std::string &key);
	inline std::map<std::string, std::string>getParamMap() const;
	inline std::map<std::string, std::string>getVarMap() const;
private:
	mutable std::mutex mtx;					//变量锁
	std::map<std::string, std::string>paramMap;//参数列表
	std::map<std::string, std::string>varMap;//变量列表
};
class AlgoTradingAPI;
class AlgorithmOrder;
class StrategyTemplate
{
public:
	bool inited;
	bool trading;
	AlgorithmOrder *algorithmorder;
	jsstructs::BacktestGodData backtestgoddata;
	std::set<std::string>ninetoeleven;
	std::set<std::string>ninetohalfeleven;
	std::set<std::string>ninetoone;
	std::set<std::string>ninetohalftwo;
	explicit StrategyTemplate(AlgoTradingAPI *algotrading);

	~StrategyTemplate();
	virtual void showStrategy();

	void  onInit();

	void  onStart();

	virtual void  onStop();

	void  cancelOrder(const std::string &orderID);

	void  cancelOrders();

	void  onTick_template(const std::shared_ptr<Event_Tick>&tick);

	void  onBar_template(const jsstructs::BarData &bar,bool inited);

	void  onOrder_template(const std::shared_ptr<Event_Order>&order);

	void  onTrade_template(const std::shared_ptr<Event_Trade>&trade);

	void  addParam(const std::string &paramName, const std::string &paramValue);

	std::string  getParam(const std::string &paramName);

	void  modifyPos(const std::string &symbol, double pos);

	double  getPos(const std::string &symbol);

	std::map<std::string, double> getPosMap();

	//做多
	std::vector<std::string>  buy(const std::string &symbol, double price, double volume);
	//平多
	std::vector<std::string>  sell(const std::string &symbol, double price, double volume);
	//做空
	std::vector<std::string>  sellshort(const std::string &symbol, double price, double volume);
	//平空
	std::vector<std::string>  buycover(const std::string &symbol, double price, double volume);

	virtual void modifyVar(double value, const std::string &varname);

protected:
	virtual void  onTick(const std::shared_ptr<Event_Tick>&tick);

	virtual void  onBar(const jsstructs::BarData &bar);

	virtual void  onOrder(const std::shared_ptr<Event_Order>&order);

	virtual void  onTrade(const std::shared_ptr<Event_Trade>&trade);

	virtual void  putEvent();

	std::vector<std::string>  sendOrder(const std::string &symbol, const std::string &orderType, double price, double volume);

	AlgoTradingAPI *algotrading=nullptr;
	StrategyData strategyData;
	const std::string tickDbName = "CTPTickDb";
	const std::string barDbName = "CTPMinuteDb";
	std::map<std::string, double>posMap; std::mutex posMapmtx;

	std::map<std::string, double>name_mapping_var;
private:
	void  saveMongoDB();

	void  loadMongoDB();

	jsstructs::BarData Bar;
	int minute=0;
	int hour=0;
	std::vector<std::string>orderList; std::mutex orderListmtx;
};

#endif