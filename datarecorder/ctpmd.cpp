#include"ctpmd.h"
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif
Ctpmd::Ctpmd(EventEngine *eventengine, std::string gatewayname) :connectStatus(false), loginStatus(false), req_ID(0)
{
	this->gatewayname = gatewayname;
	this->eventengine = eventengine;

	std::vector<std::string>ninetoeleven = { "bu", "rb", "hc", "ru" };
	this->ninetoeleven.insert(ninetoeleven.begin(), ninetoeleven.end()); //9点到11点的合约列表
	std::vector<std::string>ninetohalfeleven = { "p", "j", "m", "y", "a", "b", "jm", "i", "SR", "CF", "RM", "MA", "ZC", "FG", "OI" };
	this->ninetohalfeleven.insert(ninetohalfeleven.begin(), ninetohalfeleven.end()); //9点到11点半的合约
	std::vector<std::string>ninetoone = { "cu", "al", "zn", "pb", "sn", "ni" };
	this->ninetoone.insert(ninetoone.begin(), ninetoone.end()); //9点到1点的合约列表
	std::vector<std::string>ninetohalftwo = { "ag", "au" };
	this->ninetohalftwo.insert(ninetohalftwo.begin(), ninetohalftwo.end()); //9点到2点半的合约
}

Ctpmd::~Ctpmd()
{

}

inline void Ctpmd::writeLog(const std::string& msg) const
{
	std::shared_ptr<Event_Log>e = std::make_shared<Event_Log>();
	e->msg = msg;
	e->gatewayname = this->gatewayname;
	e->logTime = Utils::getCurrentDateTime();
	this->eventengine->put(e);
}

void Ctpmd::OnFrontConnected()
{
	this->connectStatus = true;
	this->writeLog("行情线程连接成功");
	this->login();
}

void Ctpmd::OnFrontDisconnected(int nReason)
{
	this->connectStatus = false;
	this->loginStatus = false;
	this->writeLog("行情线程连接断开，nReason:" + std::to_string(nReason));
}

void Ctpmd::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	this->writeLog(pRspInfo->ErrorMsg);
}

void Ctpmd::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//登录回报
	if (!IsErrorRspInfo(pRspInfo)) 
	{
		//登录成功
		this->loginStatus = true;
		this->writeLog("行情服务器登录完成");
		if (!this->subscribedSymbols.empty())
		{
			for (std::set<std::string>::const_iterator iter = this->subscribedSymbols.cbegin(); iter != this->subscribedSymbols.cend(); ++iter)
			{
				this->subscribe((*iter));
				this->writeLog("重新订阅合约:" + (*iter));
			}
		}
	}
	else 
	{
		this->loginStatus = false;
		this->writeLog(pRspInfo->ErrorMsg);
	}
}

void Ctpmd::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//登出回报
	if (!IsErrorRspInfo(pRspInfo))
	{
		this->loginStatus = false;
		this->writeLog("行情服务器登出");
	}
	else 
	{
		this->loginStatus = false;
		this->writeLog(pRspInfo->ErrorMsg);
	}
}

void Ctpmd::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	std::shared_ptr<Event_Tick>e = std::make_shared<Event_Tick>();
	e->gatewayname = this->gatewayname;
	e->symbol = pDepthMarketData->InstrumentID;
	e->exchange = pDepthMarketData->ExchangeID;
	e->lastprice = pDepthMarketData->LastPrice;
	e->volume = pDepthMarketData->Volume;
	e->openInterest = pDepthMarketData->OpenInterest;
	e->time = std::string(pDepthMarketData->UpdateTime) + "." + std::to_string(pDepthMarketData->UpdateMillisec / 100);
	e->date = Utils::getCurrentDate();
	e->openPrice = pDepthMarketData->OpenPrice;
	e->highPrice = pDepthMarketData->HighestPrice;
	e->lowPrice = pDepthMarketData->LowestPrice;
	e->preClosePrice = pDepthMarketData->PreClosePrice;
	e->upperLimit = pDepthMarketData->UpperLimitPrice;
	e->lowerLimit = pDepthMarketData->LowerLimitPrice;
	e->bidprice1 = pDepthMarketData->BidPrice1;
	e->askprice1 = pDepthMarketData->AskPrice1;
	e->bidvolume1 = pDepthMarketData->BidVolume1;
	e->askvolume1 = pDepthMarketData->AskVolume1;
	e->setUnixDatetime();
	//过滤CTP的时间***************************************************************************************************************************
	auto ticktime_t=e->getTime_t();
	auto systemtime_t=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获取当前的系统时间
	std::string symbolHead=Utils::regexSymbol(e->symbol);
	if (this->ninetoeleven.find(symbolHead) != this->ninetoeleven.end()) {
	    if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(23, 0, 0))))) {
	        //在9点到11点里头的对立事件则返回
	        return;
	    } else {
	        //数据包的时间是在交易时间段的在判断一下真实时间是否也在交易时间段
	        if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(23, 0, 0))))) {
	            return;
	        }
	    }
	} else if (this->ninetohalfeleven.find(symbolHead) != this->ninetohalfeleven.end()) {
	    if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(23, 30, 0))))) {
	        return;
	    } else {
	        if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(23, 30, 0))))) {
	            return;
	        }
	    }
	} else if (this->ninetoone.find(symbolHead) != this->ninetoone.end()) {
	    if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(24, 0, 0))) || (ticktime_t <= Utils::timeTotime_t(1, 0, 0)))) {
	        return;
	    } else {
	        if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(24, 0, 0))) || (systemtime_t <= Utils::timeTotime_t(1, 0, 0)))) {
	            return;
	        }
	    }
	} else if (this->ninetohalftwo.find(symbolHead) != this->ninetohalftwo.end()) {
	    if (!(((ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) || ((ticktime_t >= Utils::timeTotime_t(21, 0, 0)) && (ticktime_t <= Utils::timeTotime_t(24, 0, 0))) || (ticktime_t <= Utils::timeTotime_t(2, 30, 0)))) {
	        return;
	    } else {
	        if (!(((systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) || ((systemtime_t >= Utils::timeTotime_t(21, 0, 0)) && (systemtime_t <= Utils::timeTotime_t(24, 0, 0))) || (systemtime_t <= Utils::timeTotime_t(2, 30, 0)))) {
	            return;
	        }
	    }
	} else {
	    //如果只是日盘没有夜盘的合约，就判断一下发过来的时间是否是在日盘交易时间之内
	    if (!(ticktime_t >= Utils::timeTotime_t(9, 0, 0) && ticktime_t <= Utils::timeTotime_t(15, 0, 0))) {
	        return;
	    } else {
	        if (!(systemtime_t >= Utils::timeTotime_t(9, 0, 0) && systemtime_t <= Utils::timeTotime_t(15, 0, 0))) {
	            return;
	        }
	    }
	}
	this->eventengine->put(e);
}

bool Ctpmd::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo) const
{
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	return bResult;
}

void Ctpmd::connect()
{
	if (Utils::checkExist("./CTPGateway")) 
	{
		std::fstream file("./CTPGateway/ctp_connect.txt", std::ios::in);
		if (file.is_open()) 
		{
			std::string line;
			std::map<std::string, std::string>configmap;
			while (getline(file, line)) 
			{
				if (line.find("userid=") != std::string::npos) {
					configmap["userid"] = line.substr(line.find("=") + 1);
				}
				if (line.find("password=") != std::string::npos) {
					configmap["password"] = line.substr(line.find("=") + 1);
				}
				if (line.find("brokerid=") != std::string::npos) {
					configmap["brokerid"] = line.substr(line.find("=") + 1);
				}
				if (line.find("mdaddress=") != std::string::npos) {
					configmap["mdaddress"] = line.substr(line.find("=") + 1);
				}
			}
			if (configmap.size() == 4) 
			{
				this->connect_md(configmap["userid"], configmap["password"], configmap["brokerid"], configmap["mdaddress"]);
			}
			else
			{
				this->writeLog("Ctp_connect.txt userid password brokerid mdaddress 缺少字段");
			}
		}
		else 
		{
			this->writeLog("./CtpGateway/Ctp_connect.txt 读取失败");
		}
	}
	else 
	{
		this->writeLog("不存在CTPGateway目录");
	}
}

void Ctpmd::connect_md(const std::string &userID, const std::string &password, const std::string &brokerID, const std::string &address)
{
	//连接
	if (this->connectStatus == false)
	{
		this->ctpData.userID = userID;
		this->ctpData.password = password;
		this->ctpData.brokerID = brokerID;
		this->ctpData.md_address = address;
		if (Utils::checkExist("./temp")) 
		{
			this->mdApi = CThostFtdcMdApi::CreateFtdcMdApi("./temp/CTPmd");
			this->mdApi->RegisterSpi(this);
			this->mdApi->RegisterFront((char*)(this->ctpData.md_address.c_str()));
			this->mdApi->Init();
		}
		else 
		{
			if (Utils::createDirectory("./temp"))
			{
				this->mdApi = CThostFtdcMdApi::CreateFtdcMdApi("./temp/CTPmd");
				this->mdApi->RegisterSpi(this);
				this->mdApi->RegisterFront((char*)(this->ctpData.md_address.c_str()));
				this->mdApi->Init();
			}
			else 
			{
				this->writeLog("创建temp目录失败!");
			}
		}
	}
}

void Ctpmd::login()
{
	//登录
	if (this->loginStatus == false)
	{
		if (this->ctpData.userID.empty() == false && this->ctpData.password.empty() == false && this->ctpData.brokerID.empty() == false) 
		{
			CThostFtdcReqUserLoginField myreq;
			strncpy(myreq.BrokerID, this->ctpData.brokerID.c_str(), sizeof(myreq.BrokerID) - 1);
			strncpy(myreq.UserID, this->ctpData.userID.c_str(), sizeof(myreq.UserID) - 1);
			strncpy(myreq.Password, this->ctpData.password.c_str(), sizeof(myreq.Password) - 1);
			this->req_ID++;
			this->mdApi->ReqUserLogin(&myreq, this->req_ID);//登录账号
		}
	}
}

void Ctpmd::subscribe(const std::string &symbol)
{
	if (this->loginStatus == true) 
	{
		if (this->subscribedSymbols.find(symbol) == this->subscribedSymbols.end()) 
		{
			this->subscribedSymbols.insert(symbol);
		}
		char* buffer = (char*)symbol.c_str();
		char* myreq[1] = { buffer };
		int iResult = this->mdApi->SubscribeMarketData(myreq, 1);
		if (iResult == 0)
		{
			this->writeLog("订阅" + symbol + "成功");
		}
		else
		{
			this->writeLog("订阅" + symbol + "失败");
		}
	}
}

void Ctpmd::logout()
{
	CThostFtdcUserLogoutField myreq;
	strncpy(myreq.BrokerID, this->ctpData.brokerID.c_str(), sizeof(myreq.BrokerID) - 1);
	strncpy(myreq.UserID, this->ctpData.userID.c_str(), sizeof(myreq.UserID) - 1);
	this->req_ID++;
	int iResult = this->mdApi->ReqUserLogout(&myreq, this->req_ID);
	if (iResult == 0)
	{
		this->writeLog("登出请求发送成功");
	}
	else 
	{
		this->writeLog("登出请求发送失败");
	}
}

void Ctpmd::close()
{
	if (this->mdApi != nullptr)
	{
		this->connectStatus = false;
		this->loginStatus = false;
		this->mdApi->RegisterSpi(nullptr);
		this->mdApi->Release();
		this->mdApi = nullptr;
	}
}
