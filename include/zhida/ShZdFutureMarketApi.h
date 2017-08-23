/////////////////////////////////////////////////////////////////////////
///@system 新一代交易接口系统
///@company 上海期货信息技术有限公司
///@file ShZdFutureMarketApi.h
///@brief 定义了行情客户端接口
///@history 
///20161106	smithxiang	创建该文件
/////////////////////////////////////////////////////////////////////////

#if !defined(SHZD_MARKETAPI_H)
#define SHZD_MARKETAPI_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Include\ShZdFutureUserApiStruct.h"

#ifdef WIN32 
#ifdef LIB_MARKET_API_EXPORT
#define MARKET_API_EXPORT __declspec(dllexport)
#else
#define MARKET_API_EXPORT __declspec(dllimport)
#endif
#else
#define MARKET_API_EXPORT
#endif

class  CSHZdMarketSpi
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
	///错误应答
	virtual void OnRspError(CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	///订阅行情应答  
	virtual void OnRspSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CTShZdSpecificInstrumentField *pSpecificInstrument, CTShZdRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {};
	///深度行情通知
	virtual void OnRtnDepthMarketData(CTShZdDepthMarketDataField *pDepthMarketData) {};
	///成交数据通知,外盘新增方法
	virtual void OnRtnFilledMarketData(CTShZdFilledDataField* pFilledMarketData){};
};

class MARKET_API_EXPORT CSHZdMarketApi
{
public:
	///创建MdApi
	///@param pszFlowPath 存贮订阅信息文件的目录，默认为当前目录
	///@return 创建出的UserApi
	///modify for udp marketdata
	static CSHZdMarketApi *CreateSHZdMarketApi(const char *pszFlowPath = "", const bool bIsUsingUdp=false);	
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
	///注册行情服务器网络地址
	///@param pszFrontAddress：行情服务器网络地址。
	///@remark 网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:17001”。 
	///@remark “tcp”代表传输协议，“127.0.0.1”代表服务器地址。”17001”代表服务器端口号。
	virtual void RegisterFront(char *pszFrontAddress) = 0;	
	///注册名前置机网络地址
	///@param pszNsAddress：前置机地址。
	///@remark 网络地址的格式为：“protocol://ipaddress:port”，如：”tcp://127.0.0.1:12001”。 
	///@remark “tcp”代表传输协议，“127.0.0.1”代表服务器地址。”12001”代表服务器端口号。
	///@remark RegisterNameServer优先于RegisterFront
	virtual void RegisterLoginFront(char *pszNsAddress) = 0;	
	///注册回调接口
	///@param pSpi 派生自回调接口类的实例
	virtual void RegisterSpi(CSHZdMarketSpi *pSpi) = 0;	
	///订阅行情。
	///@param ppInstrumentID 合约ID  
	///@param nCount 要订阅/退订行情的合约个数
	///@remark 
	virtual int SubscribeMarketData(char *ppInstrumentID[], int nCount) = 0;
	///退订行情。
	///@param ppInstrumentID 合约ID  
	///@param nCount 要订阅/退订行情的合约个数
	///@remark 
	virtual int UnSubscribeMarketData(char *ppInstrumentID[], int nCount) = 0;
	///用户登录请求
	virtual int ReqUserLogin(CTShZdReqUserLoginField *pReqUserLoginField, int nRequestID) = 0;
	///登出请求
	virtual int ReqUserLogout(CTShZdUserLogoutField *pUserLogout, int nRequestID) = 0;

protected:
	~CSHZdMarketApi(){};
};

#endif
