#ifndef DATARECORDERCTPMD_H
#define DATARECORDERCTPMD_H
#include<string>
#include<set>
#include<fstream>
#include<map>
#include"ThostFtdcMdApi.h"
#include"eventengine.h"
#include"utils.hpp"
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif

class Ctpmd : public CThostFtdcMdSpi
{
public:
	Ctpmd(EventEngine *eventengine, std::string gatewayname="ctp");

	~Ctpmd();

	void connect();

	void close();

	void login();

	void logout();

	void subscribe(const std::string &symbol);

protected:
	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	inline void writeLog(const std::string& msg) const ;
	inline bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo) const;
	void connect_md(const std::string &userID, const std::string &password, const std::string &brokerID, const std::string &address);

	std::string gatewayname;
	EventEngine *eventengine;
	CThostFtdcMdApi* mdApi;
	bool connectStatus;
	bool loginStatus;
        CtpConnectData ctpData;
	std::set<std::string>subscribedSymbols;
	int req_ID=0;

	std::shared_ptr<spdlog::logger> my_logger;
};
#endif
