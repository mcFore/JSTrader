/////////////////////////////////////////////////////////////////////////
///@system 新一代交易所系统
///@company 上海期货信息技术有限公司
///@file ShZdFutureTraderApi.h
///@brief 定义了交易客户端接口
///@history 
///20161106	smithxiang	创建该文件
/////////////////////////////////////////////////////////////////////////

#if !defined(THOST_FTDCTRADERAPI_H)
#define THOST_FTDCTRADERAPI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Include\ShZdFutureUserApiStruct.h"

#ifdef WIN32 
#ifdef LIB_TRADER_API_EXPORT
#define TRADER_API_EXPORT __declspec(dllexport)
#else
#define TRADER_API_EXPORT __declspec(dllimport)
#endif
#else
#define TRADER_API_EXPORT
#endif
class CSHZdTraderSpi
{
public:
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected(){};
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason){};
		
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse){};
	
	///登录请求响应
	virtual void OnRspUserLogin(CTShZdRspUserLoginField *pRspUserLogin, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///登出请求响应
	virtual void OnRspUserLogout(CTShZdUserLogoutField *pUserLogout, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///用户口令更新请求响应
	virtual void OnRspUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单录入请求响应
	virtual void OnRspOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///预埋单录入请求响应（直达暂时不支持）
	virtual void OnRspParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///预埋撤单录入请求响应（直达暂时不支持）
	virtual void OnRspParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单操作请求响应			  
	virtual void OnRspOrderAction(CTShZdInputOrderActionField *pInputOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///删除预埋单响应（直达暂时不支持）
	virtual void OnRspRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///删除预埋撤单响应（直达暂时不支持）
	virtual void OnRspRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询报单响应
	virtual void OnRspQryOrder(CTShZdOrderField *pOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询成交响应
	virtual void OnRspQryTrade(CTShZdTradeField *pTrade, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CTShZdInvestorPositionField *pInvestorPosition, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CTShZdTradingAccountField *pTradingAccount, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	
	///请求查询交易所响应
	virtual void OnRspQryExchange(CTShZdExchangeField *pExchange, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询合约响应
	virtual void OnRspQryInstrument(CTShZdInstrumentField *pInstrument, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询投资者持仓明细响应
	virtual void OnRspQryInvestorPositionDetail(CTShZdInvestorPositionDetailField *pInvestorPositionDetail, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
		
	///请求查询转帐流水响应(出入金)
	virtual void OnRspQryTransferSerial(CTShZdTransferSerialField *pTransferSerial, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
		
	///错误应答
	virtual void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///报单通知
	virtual void OnRtnOrder(CTShZdOrderField *pOrder) {};

	///成交通知
	virtual void OnRtnTrade(CTShZdTradeField *pTrade) {};

	///交易资金通知（报单、成交、撤单、改单）
	virtual void OnRtnTradeMoney(CTShZdTradeMoneyField *pTradeMoney){};

	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CTShZdInputOrderField *pInputOrder, CTShZdRspInfoField *pRspInfo) {};
	
	///提示条件单校验错误（直达暂时不支持）
	virtual void OnRtnErrorConditionalOrder(CTShZdErrorConditionalOrderField *pErrorConditionalOrder) {};

	///请求查询预埋单响应（直达暂时不支持）
	virtual void OnRspQryParkedOrder(CTShZdParkedOrderField *pParkedOrder, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///请求查询预埋撤单响应（直达暂时不支持）
	virtual void OnRspQryParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};

	///直达开收盘时间
	virtual void OnRtnOpenCloseTime(CTShZdOpenCloseTimeField *openCloseTime,int nRequestID,bool bIsLast){};
	
	///直达支持市场的开收盘时间
	virtual void OnRtnMarketOpenCloseTime(CTShZdMarketOpenCloseTimeField *openCloseTime,int nRequestID,bool bIsLast){};
	
	///直达支持产品的开收盘时间
	virtual void OnRtnCommonOpenCloseTime(CTShZdCommonOpenCloseTimeField *commonTime,int nRequestID,bool bIsLast){};
	
	///直达汇率查询返回
	virtual void OnRspMoneyRatio(CTShZdMoneyRatioField * rspRation,int nRequestID,bool bIsLast){};
};

class TRADER_API_EXPORT CSHZdTraderApi
{
public:
	///创建TraderApi
	///@param pszFlowPath 存贮订阅信息文件的目录，默认为当前目录
	///@return 创建出的UserApi
	//modify for udp marketdata
	static CSHZdTraderApi *CreateSHZdTraderApi(const char *pszFlowPath = "", const bool bIsUsingUdp=false);
	
	///删除接口对象本身
	///@remark 不再使用本接口对象时,调用该函数删除接口对象
	virtual void Release() = 0;	
	///初始化
	///@remark 初始化运行环境,只有调用后,接口才开始工作
	virtual void Init() = 0;
		
	///获取当前交易日
	///@retrun 获取到的交易日
	///@remark 只有登录成功后,才能得到正确的交易日
	virtual const char *GetTradingDay() = 0;
	
	///注册前置机网络地址
	///@param pszFrontAddress：前置机网络地址。
	///@remark 网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:17001”。 
	///@remark “tcp”代表传输协议，“127.0.0.1”代表服务器地址。”17001”代表服务器端口号。
	virtual void RegisterFront(char *pszFrontAddress) = 0;	
	///注册回调接口
	///@param pSpi 派生自回调接口类的实例
	virtual void RegisterSpi(CSHZdTraderSpi *pSpi) = 0;			
	///用户登录请求
	virtual int ReqUserLogin(CTShZdReqUserLoginField *pReqUserLoginField, int nRequestID) = 0;
	
	///登出请求
	virtual int ReqUserLogout(CTShZdUserLogoutField *pUserLogout, int nRequestID) = 0;

	///用户口令更新请求
	virtual int ReqUserPasswordUpdate(CTShZdUserPasswordUpdateField *pUserPasswordUpdate, int nRequestID) = 0;

	///报单录入请求
	virtual int ReqOrderInsert(CTShZdInputOrderField *pInputOrder, int nRequestID) = 0;

	///预埋单录入请求（直达暂时不支持）
	virtual int ReqParkedOrderInsert(CTShZdParkedOrderField *pParkedOrder, int nRequestID) = 0;

	///预埋撤单录入请求（直达暂时不支持）
	virtual int ReqParkedOrderAction(CTShZdParkedOrderActionField *pParkedOrderAction, int nRequestID) = 0;

	///报单操作请求
	virtual int ReqOrderAction(CTShZdOrderActionField *pInputOrderAction, int nRequestID) = 0;

	///请求删除预埋单（直达暂时不支持）
	virtual int ReqRemoveParkedOrder(CTShZdRemoveParkedOrderField *pRemoveParkedOrder, int nRequestID) = 0;

	///请求删除预埋撤单（直达暂时不支持）
	virtual int ReqRemoveParkedOrderAction(CTShZdRemoveParkedOrderActionField *pRemoveParkedOrderAction, int nRequestID) = 0;

	///请求查询报单
	virtual int ReqQryOrder(CTShZdQryOrderField *pQryOrder, int nRequestID) = 0;

	///请求查询成交
	virtual int ReqQryTrade(CTShZdQryTradeField *pQryTrade, int nRequestID) = 0;

	///请求查询投资者持仓汇总
	virtual int ReqQryInvestorPosition(CTShZdQryInvestorPositionField *pQryInvestorPosition, int nRequestID) = 0;

	///请求查询资金账户
	virtual int ReqQryTradingAccount(CTShZdQryTradingAccountField *pQryTradingAccount, int nRequestID) = 0;

	///请求查询交易所
	virtual int ReqQryExchange(CTShZdQryExchangeField *pQryExchange, int nRequestID) = 0;

	///请求查询合约
	virtual int ReqQryInstrument(CTShZdQryInstrumentField *pQryInstrument, int nRequestID) = 0;

	///请求查询投资者持仓明细
	virtual int ReqQryInvestorPositionDetail(CTShZdQryInvestorPositionDetailField *pQryInvestorPositionDetail, int nRequestID) = 0;

	///请求查询出入金
	virtual int ReqQryTransferSerial(CTShZdQryTransferSerialField *pQryTransferSerial, int nRequestID) = 0;
		
	///请求查询预埋单（直达暂时不支持）
	virtual int ReqQryParkedOrder(CTShZdQryParkedOrderField *pQryParkedOrder, int nRequestID) = 0;

	///请求查询预埋撤单（直达暂时不支持）
	virtual int ReqQryParkedOrderAction(CTShZdQryParkedOrderActionField *pQryParkedOrderAction, int nRequestID) = 0;
	
	///查询直达开收盘时间
	virtual int ReqQueryOpenCloseTime(CTShZdReqOpenCloseTimeField *OpenCloseTime,int nRequestID)=0;
	
	///查询直达支持的市场开始盘时间
	virtual int ReqQueryMarketOpenCloseTime(CTShZdReqMarketOpenCloseTimeField *OpenCloseTime,int nRequestID)=0;
	
	///查询直达支持的产品的开收盘时间
	virtual int ReqQueryCommonOpenCloseTime(CTShZdReqCommonOpenCloseTimeField *comTime,int nRequestID)=0;

	///查询直达汇率
	virtual int ReqQueryMoneyRatio(CTShZdReqMoneyRatioField* reqRation,int nRequestID)=0;

protected:
	~CSHZdTraderApi(){};
};


#endif
