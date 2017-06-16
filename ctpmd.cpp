#include"ctpmd.h"
#if _MSC_VER >= 1600

#pragma execution_character_set("utf-8")

#endif
Ctpmd::Ctpmd(EventEngine *eventengine, std::string gatewayname):connectStatus(false),loginStatus(false),req_ID(0)
{
	my_logger = spdlog::basic_logger_mt("Ctpmd", "logs/Ctpmd.txt");
	try {
		this->gatewayname = gatewayname;
		this->eventengine=eventengine;
	} catch(...) {
		my_logger->error("Ctpmd()");
	}
}

Ctpmd::~Ctpmd()
{
}

inline void Ctpmd::writeLog(const std::string& msg) const
{
	try {
		std::shared_ptr<Event_Log>e=std::make_shared<Event_Log>();
		e->msg = msg;
		e->gatewayname = this->gatewayname;
		e->logTime=Utils::getCurrentDateTime();
		this->eventengine->put(e);
	} catch(...) {
		my_logger->error("writeLog()");
	}
}

void Ctpmd::OnFrontConnected()
{
	try {
		this->connectStatus = true;
		this->writeLog("行情线程连接成功");
		this->login();
	} catch(...) {
		my_logger->error("OnFrontConnected()");
	}
}

void Ctpmd::OnFrontDisconnected(int nReason)
{
	try {
		this->connectStatus = false;
		this->loginStatus = false;
		this->writeLog("行情线程连接断开，nReason:"+std::to_string(nReason));
	} catch(...) {
		my_logger->error("OnFrontDisconnected()");
	}
}

void Ctpmd::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	try {
		this->writeLog(pRspInfo->ErrorMsg);
	} catch(...) {
		my_logger->error("OnRspError()");
	}
}

void Ctpmd::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	try {
		//登录回报
		if (!IsErrorRspInfo(pRspInfo)) {
			//登录成功
			this->loginStatus = true;
			this->writeLog("行情服务器登录完成");
		} else {
			this->loginStatus = false;
			this->writeLog(pRspInfo->ErrorMsg);
		}
	} catch(...) {
		my_logger->error("OnRspUserLogin()");
	}
}

void Ctpmd::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	try {
		//登出回报
		if (!IsErrorRspInfo(pRspInfo)) {
			this->loginStatus = false;
			this->writeLog("行情服务器登出");
		} else {
			this->loginStatus = false;
			this->writeLog(pRspInfo->ErrorMsg);
		}
	} catch(...) {
		my_logger->error("OnRspUserLogout()");
	}
}

void Ctpmd::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	try {
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
		this->eventengine->put(e);
	} catch(...) {
		my_logger->error("OnRtnDepthMarketData()");
	}
}

bool Ctpmd::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo) const
{
	try {
		bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
		return bResult;
	} catch(...) {
		my_logger->error("IsErrorRspInfo()");
	}
}

void Ctpmd::connect()
{
	try {
		if (Utils::checkExist("./CTPGateway")) {
			std::fstream file("./CTPGateway/ctp_connect.txt",std::ios::in);
			if (file.is_open()) {
				std::string line;
				std::map<std::string,std::string>configmap;
				while(getline(file,line)) {
					if(line.find("userid=")!=std::string::npos) {
						configmap["userid"]=line.substr(line.find("=")+1);
					}
					if(line.find("password=")!=std::string::npos) {
						configmap["password"]=line.substr(line.find("=")+1);
					}
					if(line.find("brokerid=")!=std::string::npos) {
						configmap["brokerid"]=line.substr(line.find("=")+1);
					}
					if(line.find("mdaddress=")!=std::string::npos) {
						configmap["mdaddress"]=line.substr(line.find("=")+1);
					}
				}
				if(configmap.size()==4) {
					this->connect_md(configmap["userid"],configmap["password"],configmap["brokerid"],configmap["mdaddress"]);
				} else {
					this->writeLog("Ctp_connect.txt userid password brokerid mdaddress 缺少字段");
				}
			} else {
				this->writeLog("./CtpGateway/Ctp_connect.txt 读取失败");
			}
		} else {
			this->writeLog("不存在CTPGateway目录");
		}
	} catch(...) {
		my_logger->error("connect()");
	}
}
void Ctpmd::connect_md(const std::string &userID, const std::string &password, const std::string &brokerID, const std::string &address)
{
	try {
		//连接
		if(this->connectStatus==false) {
                        this->ctpData.userID=userID;
                        this->ctpData.password=password;
                        this->ctpData.brokerID=brokerID;
                        this->ctpData.md_address=address;
			if(Utils::checkExist("./temp")) {
				this->mdApi = CThostFtdcMdApi::CreateFtdcMdApi("./temp/CTPmd");
				this->mdApi->RegisterSpi(this);
                                this->mdApi->RegisterFront((char*)(this->ctpData.md_address.c_str()));
				this->mdApi->Init();
			} else {
				if(Utils::createDirectory("./temp")) {
					this->mdApi = CThostFtdcMdApi::CreateFtdcMdApi("./temp/CTPmd");
					this->mdApi->RegisterSpi(this);
                                        this->mdApi->RegisterFront((char*)(this->ctpData.md_address.c_str()));
					this->mdApi->Init();
				} else {
					this->writeLog("创建temp目录失败!");
				}
			}
		}
	} catch(...) {
		my_logger->error("connect_md()");
	}
}

void Ctpmd::login()
{
	try {
		//登录
		if(this->loginStatus==false) {
                        if (this->ctpData.userID.empty() == false && this->ctpData.password.empty() == false && this->ctpData.brokerID.empty() == false) {
				CThostFtdcReqUserLoginField myreq;
                                strncpy(myreq.BrokerID, this->ctpData.brokerID.c_str(), sizeof(myreq.BrokerID) - 1);
                                strncpy(myreq.UserID, this->ctpData.userID.c_str(), sizeof(myreq.UserID) - 1);
                                strncpy(myreq.Password, this->ctpData.password.c_str(), sizeof(myreq.Password) - 1);
				this->req_ID++;
				this->mdApi->ReqUserLogin(&myreq, this->req_ID);//登录账号
			}
		}
	} catch(...) {
		my_logger->error("login()");
	}
}

void Ctpmd::subscribe(const std::string &symbol)
{
	try {
		if (this->loginStatus == true) {
			if(this->subscribedSymbols.find(symbol)==this->subscribedSymbols.end()) {
				this->subscribedSymbols.insert(symbol);
			}
			char* buffer = (char*)symbol.c_str();
			char* myreq[1] = { buffer };
			int iResult=this->mdApi->SubscribeMarketData(myreq, 1);
			if (iResult == 0)
			{
				this->writeLog("订阅" + symbol + "成功");
			}
			else
			{
				this->writeLog("订阅" + symbol + "失败");
			}
		}
	} catch(...) {
		my_logger->error("subscribe()");
	}
}

void Ctpmd::logout()
{
	try {
		CThostFtdcUserLogoutField myreq;
                strncpy(myreq.BrokerID, this->ctpData.brokerID.c_str(), sizeof(myreq.BrokerID) - 1);
                strncpy(myreq.UserID, this->ctpData.userID.c_str(), sizeof(myreq.UserID) - 1);
		this->req_ID++;
		int iResult=this->mdApi->ReqUserLogout(&myreq,this->req_ID);
		if(iResult==0) {
			this->writeLog("登出请求发送成功");
		} else {
			this->writeLog("登出请求发送失败");
		}
	} catch(...) {
		my_logger->error("logout()");
	}
}

void Ctpmd::close()
{
	try {
		if (this->mdApi != nullptr) {
			this->connectStatus = false;
			this->loginStatus = false;
			this->mdApi->RegisterSpi(nullptr);
			this->mdApi->Release();
			this->mdApi = nullptr;
		}
	} catch(...) {
		my_logger->error("close()");
	}
}
