/////////////////////////////////////////////////////////////////////////
///上海直达新一代API系统
///@company 上海期货信息技术有限公司
///@file ShZdFutureUserApiStruct.h
///@brief 定义了客户端接口使用的业务数据结构
///@history 
///20161106	smithxiang		创建该文件
/////////////////////////////////////////////////////////////////////////

#if !defined(TSHZD_TRADESTRUCT_H)
#define TSHZD_TRADESTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Include\ShZdFutureUserApiDataType.h"
#pragma pack(push, 1)
///直达信息分发
struct CTShZdDisseminationField
{
	///序列系列号	
	TShZdSequenceSeriesType	SequenceSeries;
	///序列号
	TShZdSequenceNoType	SequenceNo;
};

///直达用户登录请求
struct CTShZdReqUserLoginField
{
	///交易日
	TShZdDateType	TradingDay;
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///用户代码  直达必须填写
	TShZdUserIDType	UserID;
	///密码  直达必须填写
	TShZdPasswordType	Password;
	///用户端产品信息
	TShZdProductInfoType	UserProductInfo;
	///接口端产品信息
	TShZdProductInfoType	InterfaceProductInfo;
	///协议信息
	TShZdProtocolInfoType	ProtocolInfo;
	///Mac地址
	TShZdMacAddressType	MacAddress;
	///动态密码
	TShZdPasswordType	OneTimePassword;
	///终端IP地址  直达必须填写
	TShZdIPAddressType	ClientIPAddress;
};

///直达用户登录应答
struct CTShZdRspUserLoginField
{
	///交易日 直达
	TShZdDateType	TradingDay;
	///登录成功时间
	TShZdTimeType	LoginTime;	
	///用户代码  直达
	TShZdUserIDType	UserID;
	///交易系统名称  直达
	TShZdSystemNameType	SystemName;	
	///投资者帐号  资金账号  直达
	TShZdAccountIDType	AccountID;
	///币种，账号的币种  直达
	TShZdCurrencyNoType CurrencyNo;	
};

///直达用户登出请求
struct CTShZdUserLogoutField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///用户代码  直达
	TShZdUserIDType	UserID;
};

///直达响应信息
struct CTShZdRspInfoField
{
	///错误代码  直达
	TShZdErrorIDType	ErrorID;
	///错误信息  直达
	TShZdErrorMsgType	ErrorMsg;
};

///直达交易所
struct CTShZdExchangeField
{
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///交易所名称  直达
	TShZdExchangeNameType	ExchangeName;
	///交易所属性
	TShZdExchangePropertyType	ExchangeProperty;
};

///直达合约
struct CTShZdInstrumentField
{
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///合约名称  直达
	TShZdInstrumentNameType	InstrumentName;
	///合约在交易所的代码  直达
	TShZdExchangeInstIDType	ExchangeInstID;
	///交易所名称  直达
	TShZdExchangeNameType ExchangeName;
	///产品代码  直达
	TShZdInstrumentIDType	ProductID;
	///产品名称  直达
	TShZdInstrumentNameType	ProductName;
	///产品类型  F期货 O期权  直达
	TShZdProductClassType	ProductClass;
	///合约货币代码  直达
	TShZdCurrencyNoType  CurrencyNo;
	///货币名称  直达
	TShZdCurrencyNameType  CurrencyName;	
	///行情小数为数 直达
	TShZdVolumeType	MarketDot;
	///行情进阶单位 10进制 32进制  64进制等 直达
	TShZdVolumeType	MarketUnit;
	///调期小时点位数  直达
	TShZdVolumeType	ChangeMarketDot;
	///合约数量乘数  点值（一个最小跳点的价值） 直达
	TShZdPriceType	VolumeMultiple;
	///调期最小变动单位  直达
	TShZdPriceType	ChangeMultiple;
	///最小变动价位  直达
	TShZdPriceType	PriceTick;	
	///交割月日  直达
	TShZdDateType	StartDelivDate;
	///最后更新日  直达
	TShZdDateType	LastUpdateDate;
	///首次通知日 直达
	TShZdDateType	ExpireDate;
	///最后交易日  直达
	TShZdDateType	EndTradeDate;	
	///当前是否交易
	TShZdBoolType	IsTrading;
	///期权类型
	TShZdOptionTypeType	OptionType;
	///期权年月  直达
	TShZdDateType	OptionDate;
	///保证金率  直达
	TShZdRatioType	MarginRatio;
	///固定保证金  直达
	TShZdRatioType	MarginValue;
	///手续费率  直达
	TShZdRatioType	FreeRatio;
	///固定手续费  直达
	TShZdRatioType	FreeValue;
	///现货商品昨结算价  直达
	TShZdPriceType  SpotYesSetPrice;
	///现货商品点值  直达
	TShZdPriceType  SpotMultiple;
	///现货商品最小变动单位  直达
	TShZdPriceType	SpotTick;
	///期权临界价格  直达
	TShZdPriceType	OptionTickPrice;
	///期权临界价格以下最小跳点  直达
	TShZdPriceType	OptionTick;
	///期权执行价  直达
	TShZdPriceType	OptionPrice;
	///期权对应期货的商品代码 直达
	TShZdInstrumentIDType OptionCommodityNo;
	///期权对应期货的合约代码 直达
	TShZdInstrumentIDType OptionContractNo;
};

///直达资金账户
struct CTShZdTradingAccountField
{
	///用户代码  直达
	TShZdUserIDType	UserID;	
	///资金账号  直达
	TShZdAccountIDType	AccountID;
	///昨可用  直达
	TShZdMoneyType	PreMortgage;
	///昨权益 直达
	TShZdMoneyType	PreCredit;
	///昨结存 直达
	TShZdMoneyType	PreDeposit;
	///今权益  直达
	TShZdMoneyType	CurrBalance;
	///今可用 直达
	TShZdMoneyType	CurrUse;
	///今结存 直达
	TShZdMoneyType	CurrDeposit;	
	///入金金额   直达
	TShZdMoneyType	Deposit;
	///出金金额   直达
	TShZdMoneyType	Withdraw;
	///冻结的保证金  直达
	TShZdMoneyType	FrozenMargin;	
	///当前保证金总额  直达
	TShZdMoneyType	CurrMargin;	
	///手续费  直达
	TShZdMoneyType	Commission;
	///平仓盈亏  直达
	TShZdMoneyType	CloseProfit;
	///净盈利（总盈亏） 直达
	TShZdMoneyType	NetProfit;
	///未到期平盈   直达
	TShZdMoneyType	UnCloseProfit;
	///未冻结平盈  直达
	TShZdMoneyType	UnFrozenCloseProfit;	
	///交易日
	TShZdDateType	TradingDay;	
	///信用额度  直达
	TShZdMoneyType	Credit;
	///配资资金  直达
	TShZdMoneyType	Mortgage;
	///维持保证金  直达
	TShZdMoneyType	KeepMargin;
	///期权利金  直达
	TShZdMoneyType	RoyaltyMargin;
	///初始资金  直达
	TShZdMoneyType	FirstInitMargin;
	///盈利率  直达
	TShZdMoneyType	ProfitRatio;
	///风险率  直达
	TShZdMoneyType	RiskRatio;
	///币种，账号的币种  直达
	TShZdCurrencyNoType CurrencyNo;
	///货币与基币的汇率  直达
	TShZdMoneyType	CurrencyRatio;
};

///直达投资者持仓
struct CTShZdInvestorPositionField
{
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;	
	///资金代码  直达
	TShZdInvestorIDType	InvestorID;	
	///持买量  直达
	TShZdVolumeType	HoldBuyVolume;
	///持买开均价  直达
	TShZdMoneyType	HoldBuyOpenPrice;
	///持买均价 直达
	TShZdMoneyType	HoldBuyPrice;
	///持卖量  直达
	TShZdVolumeType	HoldSaleVolume;
	//持卖开均价  直达
	TShZdMoneyType	HoldSaleOpenPrice;
	///持卖均价  直达
	TShZdMoneyType	HoldSalePrice;
	///持买保证金  直达
	TShZdMoneyType	HoldBuyAmount;
	///持卖保证金  直达
	TShZdMoneyType	HoldSaleAmount;
	///开仓量  直达
	TShZdVolumeType	OpenVolume;
	///成交量  直达
	TShZdVolumeType	FilledVolume;	
	///成交均价  直达
	TShZdMoneyType	FilledAmount;	
	///手续费  直达
	TShZdMoneyType	Commission;	
	///持仓盈亏  直达
	TShZdMoneyType	PositionProfit;	
	///交易日  直达
	TShZdDateType	TradingDay;	
};

///上海直达行情成交数据  
struct CTShZdFilledDataField
{
	///交易日  直达  
	TShZdDateType	TradingDay;
	///合约代码   直达
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///合约在交易所的代码  直达
	TShZdExchangeInstIDType	ExchangeInstID;
	///成交价  直达
	TShZdPriceType	LastPrice;
	///成交数量  直达
	TShZdVolumeType	Volume;
	///成交总数量  直达
	TShZdVolumeType	FilledVolume;
	///最后修改时间  直达
	TShZdTimeType	UpdateTime;
	///最后修改毫秒  直达
	TShZdMillisecType	UpdateMillisec;	
};

///深度行情
struct CTShZdDepthMarketDataField
{
	///交易日  直达
	TShZdDateType	TradingDay;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码   直达
	TShZdExchangeIDType	ExchangeID;
	///合约在交易所的代码  
	TShZdExchangeInstIDType	ExchangeInstID;
	///最新价  直达
	TShZdPriceType	LastPrice;
	///上次结算价  直达
	TShZdPriceType	PreSettlementPrice;
	///昨收盘  直达
	TShZdPriceType	PreClosePrice;
	///昨持仓量
	TShZdLargeVolumeType	PreOpenInterest;
	///今开盘  直达
	TShZdPriceType	OpenPrice;
	///最高价  直达
	TShZdPriceType	HighestPrice;
	///最低价  直达
	TShZdPriceType	LowestPrice;
	///数量  直达
	TShZdVolumeType	Volume;
	///成交金额
	TShZdMoneyType	Turnover;
	///持仓量  直达
	TShZdLargeVolumeType	OpenInterest;
	///今收盘  直达
	TShZdPriceType	ClosePrice;
	///本次结算价
	TShZdPriceType	SettlementPrice;
	///涨停板价
	TShZdPriceType	UpperLimitPrice;
	///跌停板价
	TShZdPriceType	LowerLimitPrice;
	///昨虚实度
	TShZdRatioType	PreDelta;
	///今虚实度
	TShZdRatioType	CurrDelta;
	///最后修改时间  直达
	TShZdTimeType	UpdateTime;
	///最后修改毫秒  直达
	TShZdMillisecType	UpdateMillisec;
	///申买价一  直达
	TShZdPriceType	BidPrice1;
	///申买量一  直达
	TShZdVolumeType	BidVolume1;
	///申卖价一  直达
	TShZdPriceType	AskPrice1;
	///申卖量一  直达
	TShZdVolumeType	AskVolume1;
	///申买价二  直达
	TShZdPriceType	BidPrice2;
	///申买量二  直达
	TShZdVolumeType	BidVolume2;
	///申卖价二  直达
	TShZdPriceType	AskPrice2;
	///申卖量二  直达
	TShZdVolumeType	AskVolume2;
	///申买价三  直达
	TShZdPriceType	BidPrice3;
	///申买量三  直达
	TShZdVolumeType	BidVolume3;
	///申卖价三  直达
	TShZdPriceType	AskPrice3;
	///申卖量三  直达
	TShZdVolumeType	AskVolume3;
	///申买价四  直达
	TShZdPriceType	BidPrice4;
	///申买量四  直达
	TShZdVolumeType	BidVolume4;
	///申卖价四  直达
	TShZdPriceType	AskPrice4;
	///申卖量四  直达
	TShZdVolumeType	AskVolume4;
	///申买价五  直达
	TShZdPriceType	BidPrice5;
	///申买量五  直达
	TShZdVolumeType	BidVolume5;
	///申卖价五  直达
	TShZdPriceType	AskPrice5;
	///申卖量五  直达
	TShZdVolumeType	AskVolume5;
	///当日均价  直达
	TShZdPriceType	AveragePrice;
};

///直达用户口令变更
struct CTShZdUserPasswordUpdateField
{	
	///用户代码  直达
	TShZdUserIDType	UserID;
	///原来的口令  直达
	TShZdPasswordType	OldPassword;
	///新的口令  直达
	TShZdPasswordType	NewPassword;
};

///直达输入报单
struct CTShZdInputOrderField
{
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///直达的资金账号  直达
	TShZdInvestorIDType	InvestorID;
	///合约代码 直达
	TShZdInstrumentIDType	InstrumentID;
	///系统编号  直达
	TShZdOrderRefType	OrderRef;
	///本地报单编号  直达
	TShZdOrderLocalIDType	OrderLocalID;
	///用户代码  直达
	TShZdUserIDType	UserID;
	///报单价格条件   1限价单 2市价单 3限价止损（stop to limit），4止损（stop to market） 直达
	TShZdOrderPriceTypeType	OrderPriceType;
	///买卖方向   1买 2卖  直达
	TShZdDirectionType	Direction;
	///组合开平标志
	TShZdCombOffsetFlagType	CombOffsetFlag;
	///组合投机套保标志
	TShZdCombHedgeFlagType	CombHedgeFlag;
	///价格  直达
	TShZdPriceType	LimitPrice;
	///数量  直达
	TShZdVolumeType	VolumeTotalOriginal;
	///有效期类型  （1=当日有效, 2=永久有效（GTC），3=OPG，4=IOC，5=FOK，6=GTD，7=ATC，8=FAK） 直达
	TShZdTimeConditionType	TimeCondition;
	///强平编号  直达
	TShZdDateType	GTDDate;
	///成交量类型  1=regular 2=FOK 3=IOC
	TShZdVolumeConditionType	VolumeCondition;
	///最小成交量  必须小于等于委托量；有效期=4时，ShowVolume>=1小于委托量时是FOK，等于委托量时是FAK  直达
	TShZdVolumeType	MinVolume;
	///触发条件
	TShZdContingentConditionType	ContingentCondition;
	///止损价  触发价  直达
	TShZdPriceType	StopPrice;
	///强平原因
	TShZdForceCloseReasonType	ForceCloseReason;
	/// 如果是冰山单，ShowVolume的值1到orderNumber，不是冰山单，ShowVolume的值为0  直达
	TShZdVolumeType	ShowVolume;	
};

///直达报单
struct CTShZdOrderField
{	
	///经纪公司代码  直达
	TShZdBrokerIDType	BrokerID;
	///资金账号 直达
	TShZdInvestorIDType	InvestorID;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
	///订单号  直达
	TShZdOrderRefType	OrderRef;
	///用户代码   直达
	TShZdUserIDType	UserID;
	///报单价格类型   1限价单 2市价单 3限价止损（stop to limit），4止损（stop to market） 直达
	TShZdOrderPriceTypeType	OrderPriceType;
	///有效期类型  （1=当日有效, 2=永久有效（GTC），3=OPG，4=IOC，5=FOK，6=GTD，7=ATC，8=FAK） 直达
	TShZdTimeConditionType	TimeCondition;
	///买卖方向  直达
	TShZdDirectionType	Direction;
	///组合开平标志  
	TShZdCombOffsetFlagType	CombOffsetFlag;
	///组合投机套保标志
	TShZdCombHedgeFlagType	CombHedgeFlag;
	///价格  直达
	TShZdPriceType	LimitPrice;
	///数量   直达
	TShZdVolumeType	VolumeTotalOriginal;	
	///最小成交量  直达
	TShZdVolumeType	MinVolume;	
	///止损价、触发价  直达
	TShZdPriceType	StopPrice;	
	///请求编号  直达
	TShZdRequestIDType	RequestID;
	///本地编号  直达
	TShZdOrderLocalIDType	OrderLocalID;
	///交易所代码   直达
	TShZdExchangeIDType	ExchangeID;	
	///合约在交易所的代码  直达
	TShZdExchangeInstIDType	ExchangeInstID;	
	///订单状态 1、已请求 2、已排队 3、部分成交 4、完全成交 5、已撤余单 6、已撤单 7、指令失败  直达
	TShZdOrderSubmitStatusType	OrderSubmitStatus;
	/// 如果是冰山单，ShowVolume的值1到orderNumber，不是冰山单，ShowVolume的值为0  直达
	TShZdVolumeType	ShowVolume;	
	///交易日  直达
	TShZdDateType	TradingDay;	
	///系统号  直达
	TShZdOrderSysIDType	OrderSysID;	
	///报单类型  下单人类别 C客户下单  D是dealor下单 R 是强平（风控）F条件单 O第3方软件报单  直达
	TShZdOrderTypeType	OrderType;
	///今成交数量  直达
	TShZdVolumeType	VolumeTraded;
	///成交价格  直达
	TShZdPriceType  PriceTraded;	
	///报单日期  直达
	TShZdDateType	InsertDate;
	///委托时间  直达
	TShZdTimeType	InsertTime;	
	///撤单日期 直达
	TShZdDateType  CancelDate;
	///撤销时间    直达
	TShZdTimeType	CancelTime;	
	///用户强评标志  直达
	TShZdBoolType	UserForceClose;	
	///撤单号  直达
	TShZdOrderSysIDType	RelativeOrderSysID;
};

///直达报单操作（撤单、改单）回报
struct CTShZdInputOrderActionField
{	
	///资金账号  直达
	TShZdInvestorIDType	InvestorID;
	///报单操作引用  直达
	TShZdOrderActionRefType	OrderActionRef;
	///系统号 直达
	TShZdOrderRefType	OrderRef;
	///请求编号  直达
	TShZdRequestIDType	RequestID;	
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///订单号  直达
	TShZdOrderSysIDType	OrderSysID;
	///操作标志  0 撤单 3 改单  直达
	TShZdActionFlagType	ActionFlag;
	///价格变化  改单后的价格  直达
	TShZdPriceType	LimitPrice;
	///数量变化  改单后的数量 撤单数量  直达
	TShZdVolumeType	VolumeChange;
	///已成交数量
	TShZdVolumeType    VolumeFilled;
	///报单价格
	TShZdPriceType  OrderPrice;
	///报单数量   撤单直达
	TShZdVolumeType  OrderVolume; 
	///用户代码  直达
	TShZdUserIDType	UserID;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
	///有效期类型  （1=当日有效, 2=永久有效（GTC），3=OPG，4=IOC，5=FOK，6=GTD，7=ATC，8=FAK）
	TShZdTimeConditionType	TimeCondition;
	///买卖方向   1买 2卖  直达
	TShZdDirectionType	Direction;
	///报单价格条件   1限价单 2市价单 3限价止损（stop to limit），4止损（stop to market）
	TShZdOrderPriceTypeType	OrderPriceType;	
	///改单触发价格
	TShZdPriceType  ModifyTriggerPrice;
	///操作日期(改单日期、撤单日期)
	TShZdDateType	ActionDate;
	///操作时间(改单时间、撤单时间)
	TShZdTimeType	ActionTime;
};

///直达报单操作  撤单 、改单 请求 
struct CTShZdOrderActionField
{	
	///系统编号
	TShZdOrderRefType	OrderRef;	
	///报单编号
	TShZdOrderSysIDType	OrderSysID;
	///操作标志
	TShZdActionFlagType	ActionFlag;
	///修改的价格 （改单填写）
	TShZdPriceType	LimitPrice;
	///数量变化(改单填写)
	TShZdVolumeType	VolumeChange;	
	///用户代码
	TShZdUserIDType	UserID;	
};

///直达成交
struct CTShZdTradeField
{	
	///资金账号  直达
	TShZdInvestorIDType	InvestorID;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
	///报单引用 系统编号  直达
	TShZdOrderRefType	OrderRef;
	///用户代码  直达
	TShZdUserIDType	UserID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///成交编号   直达
	TShZdTradeIDType	TradeID;
	///买卖方向  直达
	TShZdDirectionType	Direction;
	///订单编号  直达
	TShZdOrderSysIDType	OrderSysID;	
	///开平标志  直达 
	TShZdOffsetFlagType	OffsetFlag;
	///投机套保标志
	TShZdHedgeFlagType	HedgeFlag;
	///价格  直达
	TShZdPriceType	Price;
	///数量  直达
	TShZdVolumeType	Volume;
	///成交时期  直达
	TShZdDateType	TradeDate;
	///成交时间   直达
	TShZdTimeType	TradeTime;
	///成交类型
	TShZdTradeTypeType	TradeType;	
	///本地报单编号   直达
	TShZdOrderLocalIDType	OrderLocalID;	
	///调期后的交割日期 
	TShZdDateType	ChangeTradingDay;	
	///成交手续费
	TShZdPriceType	PriceFree;
};

///直达查询报单
struct CTShZdQryOrderField
{
	///用户代码  直达
	TShZdUserIDType	UserID;	
	///资金代码 直达
	TShZdInvestorIDType	InvestorID;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///系统号  直达
	TShZdOrderRefType	OrderRef;
	///开始时间  直达
	TShZdTimeType	InsertTimeStart;
	///结束时间  直达
	TShZdTimeType	InsertTimeEnd;
};

///直达查询成交
struct CTShZdQryTradeField
{
	///用户代码   直达
	TShZdUserIDType	UserID;	
	///资金账号   直达
	TShZdInvestorIDType	InvestorID;
	///合约代码   直达
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///系统号   直达
	TShZdOrderRefType	OrderRef;
	///成交编号  直达
	TShZdTradeIDType	TradeID;
	///开始时间  直达
	TShZdTimeType	TradeTimeStart;
	///结束时间  直达
	TShZdTimeType	TradeTimeEnd;
};

///直达查询投资者持仓
struct CTShZdQryInvestorPositionField
{
	///用户代码
	TShZdUserIDType	UserID;
	///客户资金账号 
	TShZdInvestorIDType	InvestorID;
	///合约代码
	TShZdInstrumentIDType	InstrumentID;
};

///直达查询资金账户
struct CTShZdQryTradingAccountField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码  资金账号
	TShZdInvestorIDType	InvestorID;
	///用户代码
	TShZdUserIDType	UserID;
};

///直达查询交易所
struct CTShZdQryExchangeField
{
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
};

///直达查询合约
struct CTShZdQryInstrumentField
{
	///合约代码。查询单个合约
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码，如果填写值，查询一个交易所的合约
	TShZdExchangeIDType	ExchangeID;
	///合约在交易所的代码
	TShZdExchangeInstIDType	ExchangeInstID;
	///产品代码 ，如果填写值，查询一个产品的合约
	TShZdInstrumentIDType	ProductID;
	///开始时间,如果填写，是这个时间以后新增的
	TShZdTimeType	InsertTimeStart;
	///查询到多少条,返回这个顺序号以后，每次固定返回1千条
	TShZdVolumeType	Index;
};

///指定的合约  
struct CTShZdSpecificInstrumentField
{
	///合约代码 直达
	TShZdInstrumentIDType	InstrumentID;
};

///合约状态 未使用
struct CTShZdInstrumentStatusField
{
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
	///合约在交易所的代码
	TShZdExchangeInstIDType	ExchangeInstID;
	///结算组代码
	TShZdSettlementGroupIDType	SettlementGroupID;
	///合约代码
	TShZdInstrumentIDType	InstrumentID;
	///合约交易状态
	TShZdInstrumentStatusType	InstrumentStatus;
	///交易阶段编号
	TShZdTradingSegmentSNType	TradingSegmentSN;
	///进入本状态时间
	TShZdTimeType	EnterTime;
	///进入本状态原因
	TShZdInstStatusEnterReasonType	EnterReason;
};

///直达查询投资者持仓明细
struct CTShZdQryInvestorPositionDetailField
{
	///用户代码  直达
	TShZdUserIDType	UserID;	
	///资金账号  直达
	TShZdInvestorIDType	InvestorID;
	///合约代码  直达
	TShZdInstrumentIDType	InstrumentID;
};

///直达投资者持仓明细
struct CTShZdInvestorPositionDetailField
{
	///用户代码  直达
	TShZdUserIDType	UserID;
	///合约代码   直达
	TShZdInstrumentIDType	InstrumentID;	
	///  资金账号  直达
	TShZdInvestorIDType	InvestorID;
	///投机套保标志  直达
	TShZdHedgeFlagType	HedgeFlag;
	///买卖   直达
	TShZdDirectionType	Direction;
	///调整后的交割日期(yyyy-MM-dd)  直达
	TShZdDateType	OpenDate;
	///成交编号  直达
	TShZdTradeIDType	TradeID;
	///平仓数量  直达
	TShZdVolumeType	Volume;
	///开仓价  直达
	TShZdPriceType	OpenPrice;
	///交易日   直达
	TShZdDateType	TradingDay;	
	///交易所代码  直达 
	TShZdExchangeIDType	ExchangeID;	
	///结算价  直达
	TShZdPriceType	SettlementPrice;	
	///持仓状态  直达
	TShZdHoldSatusType HoldStatus;
	///货币编码  直达
	TShZdCurrencyNoType  CurrencyNo;
	///商品代码  直达
	TShZdInstrumentIDType	CommonID;
};

///直达预埋单
struct CTShZdParkedOrderField
{
	///经纪公司代码  止损止盈编号
	TShZdBrokerIDType	BrokerID;
	///投资者代码 用户资金账号
	TShZdInvestorIDType	InvestorID;
	///合约代码 
	TShZdInstrumentIDType	InstrumentID;
	///报单引用  本地编号
	TShZdOrderRefType	OrderRef;
	///用户代码
	TShZdUserIDType	UserID;
	///报单价格条件
	TShZdOrderPriceTypeType	OrderPriceType;
	///买卖方向
	TShZdDirectionType	Direction;
	///组合开平标志
	TShZdCombOffsetFlagType	CombOffsetFlag;
	///组合投机套保标志
	TShZdCombHedgeFlagType	CombHedgeFlag;
	///价格  止损止盈报单价
	TShZdPriceType	LimitPrice;
	///数量
	TShZdVolumeType	VolumeTotalOriginal;
	///有效期类型
	TShZdTimeConditionType	TimeCondition;
	///GTD日期
	TShZdDateType	GTDDate;
	///成交量类型
	TShZdVolumeConditionType	VolumeCondition;
	///最小成交量
	TShZdVolumeType	MinVolume;
	///触发条件 1：按最新价触发；2：按买价触发；3：按卖价触发
	TShZdContingentConditionType	ContingentCondition;
	///止损价   止损止盈触发价
	TShZdPriceType	StopPrice;
	///强平原因
	TShZdForceCloseReasonType	ForceCloseReason;
	///自动挂起标志
	TShZdBoolType	IsAutoSuspend;
	///业务单元
	TShZdBusinessUnitType	BusinessUnit;
	///请求编号
	TShZdRequestIDType	RequestID;
	///用户强评标志
	TShZdBoolType	UserForceClose;
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
	///预埋报单编号
	TShZdParkedOrderIDType	ParkedOrderID;
	///用户类型
	TShZdUserTypeType	UserType;
	///预埋单状态
	TShZdParkedOrderStatusType	Status;
	///错误代码
	TShZdErrorIDType	ErrorID;
	///错误信息
	TShZdErrorMsgType	ErrorMsg;
	///互换单标志
	TShZdBoolType	IsSwapOrder;
};

///直达输入预埋单操作
struct CTShZdParkedOrderActionField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码
	TShZdInvestorIDType	InvestorID;
	///报单操作引用
	TShZdOrderActionRefType	OrderActionRef;
	///报单引用
	TShZdOrderRefType	OrderRef;
	///请求编号
	TShZdRequestIDType	RequestID;
	///前置编号
	TShZdFrontIDType	FrontID;
	///会话编号
	TShZdSessionIDType	SessionID;
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
	///报单编号
	TShZdOrderSysIDType	OrderSysID;
	///操作标志
	TShZdActionFlagType	ActionFlag;
	///价格
	TShZdPriceType	LimitPrice;
	///数量变化
	TShZdVolumeType	VolumeChange;
	///用户代码
	TShZdUserIDType	UserID;
	///合约代码
	TShZdInstrumentIDType	InstrumentID;
	///预埋撤单单编号
	TShZdParkedOrderActionIDType	ParkedOrderActionID;
	///用户类型
	TShZdUserTypeType	UserType;
	///预埋撤单状态
	TShZdParkedOrderStatusType	Status;
	///错误代码
	TShZdErrorIDType	ErrorID;
	///错误信息
	TShZdErrorMsgType	ErrorMsg;
};

///直达查询预埋单
struct CTShZdQryParkedOrderField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码
	TShZdInvestorIDType	InvestorID;
	///合约代码
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
};

///直达查询预埋撤单
struct CTShZdQryParkedOrderActionField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码
	TShZdInvestorIDType	InvestorID;
	///合约代码
	TShZdInstrumentIDType	InstrumentID;
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
};

///直达删除预埋单
struct CTShZdRemoveParkedOrderField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码
	TShZdInvestorIDType	InvestorID;
	///预埋报单编号
	TShZdParkedOrderIDType	ParkedOrderID;
};

///直达删除预埋撤单 
struct CTShZdRemoveParkedOrderActionField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码
	TShZdInvestorIDType	InvestorID;
	///预埋撤单编号
	TShZdParkedOrderActionIDType	ParkedOrderActionID;
};

///查询错误报单操作  未使用
struct CTShZdErrorConditionalOrderField
{
	///经纪公司代码
	TShZdBrokerIDType	BrokerID;
	///投资者代码
	TShZdInvestorIDType	InvestorID;
	///合约代码
	TShZdInstrumentIDType	InstrumentID;
	///报单引用
	TShZdOrderRefType	OrderRef;
	///用户代码
	TShZdUserIDType	UserID;
	///报单价格条件
	TShZdOrderPriceTypeType	OrderPriceType;
	///买卖方向
	TShZdDirectionType	Direction;
	///组合开平标志
	TShZdCombOffsetFlagType	CombOffsetFlag;
	///组合投机套保标志
	TShZdCombHedgeFlagType	CombHedgeFlag;
	///价格
	TShZdPriceType	LimitPrice;
	///数量
	TShZdVolumeType	VolumeTotalOriginal;
	///有效期类型
	TShZdTimeConditionType	TimeCondition;
	///GTD日期
	TShZdDateType	GTDDate;
	///成交量类型
	TShZdVolumeConditionType	VolumeCondition;
	///最小成交量
	TShZdVolumeType	MinVolume;
	///触发条件
	TShZdContingentConditionType	ContingentCondition;
	///止损价
	TShZdPriceType	StopPrice;
	///强平原因
	TShZdForceCloseReasonType	ForceCloseReason;
	///自动挂起标志
	TShZdBoolType	IsAutoSuspend;
	///业务单元
	TShZdBusinessUnitType	BusinessUnit;
	///请求编号
	TShZdRequestIDType	RequestID;
	///本地报单编号
	TShZdOrderLocalIDType	OrderLocalID;
	///交易所代码
	TShZdExchangeIDType	ExchangeID;
	///会员代码
	TShZdParticipantIDType	ParticipantID;
	///客户代码
	TShZdClientIDType	ClientID;
	///合约在交易所的代码
	TShZdExchangeInstIDType	ExchangeInstID;
	///交易所交易员代码
	TShZdTraderIDType	TraderID;
	///安装编号
	TShZdInstallIDType	InstallID;
	///报单提交状态
	TShZdOrderSubmitStatusType	OrderSubmitStatus;
	///报单提示序号
	TShZdSequenceNoType	NotifySequence;
	///交易日
	TShZdDateType	TradingDay;
	///结算编号
	TShZdSettlementIDType	SettlementID;
	///报单编号
	TShZdOrderSysIDType	OrderSysID;
	///报单来源
	TShZdOrderSourceType	OrderSource;
	///报单状态
	TShZdOrderStatusType	OrderStatus;
	///报单类型
	TShZdOrderTypeType	OrderType;
	///今成交数量
	TShZdVolumeType	VolumeTraded;
	///剩余数量
	TShZdVolumeType	VolumeTotal;
	///报单日期
	TShZdDateType	InsertDate;
	///委托时间
	TShZdTimeType	InsertTime;
	///激活时间
	TShZdTimeType	ActiveTime;
	///挂起时间
	TShZdTimeType	SuspendTime;
	///最后修改时间
	TShZdTimeType	UpdateTime;
	///撤销时间
	TShZdTimeType	CancelTime;
	///最后修改交易所交易员代码
	TShZdTraderIDType	ActiveTraderID;
	///结算会员编号
	TShZdParticipantIDType	ClearingPartID;
	///序号
	TShZdSequenceNoType	SequenceNo;
	///前置编号
	TShZdFrontIDType	FrontID;
	///会话编号
	TShZdSessionIDType	SessionID;
	///用户端产品信息
	TShZdProductInfoType	UserProductInfo;
	///状态信息
	TShZdErrorMsgType	StatusMsg;
	///用户强评标志
	TShZdBoolType	UserForceClose;
	///操作用户代码
	TShZdUserIDType	ActiveUserID;
	///经纪公司报单编号
	TShZdSequenceNoType	BrokerOrderSeq;
	///相关报单
	TShZdOrderSysIDType	RelativeOrderSysID;
	///郑商所成交数量
	TShZdVolumeType	ZCETotalTradedVolume;
	///错误代码
	TShZdErrorIDType	ErrorID;
	///错误信息
	TShZdErrorMsgType	ErrorMsg;
	///互换单标志
	TShZdBoolType	IsSwapOrder;
};

///直达出入金
struct CTShZdTransferSerialField
{		
	///交易日期  直达
	TShZdDateType	TradingDay;
	///交易时间  直达
	TShZdTradeTimeType	TradeTime;	
	///入金银行名称  直达
	TShZdBankNameType BankInName;
	///入金银行帐号  直达
	TShZdBankAccountType	BankInAccount;
	///出金银行名称 直达
	TShZdBankNameType BankOutName;
	///出金银行帐号 直达
	TShZdBankAccountType	BankOutAccount;
	///银行流水号  直达
	TShZdBankSerialType	BankSerial;	
	///币种代码  直达
	TShZdCurrencyIDType	CurrencyID;	
	///出入金申请状态  直达
	TShZdInOutMoneyStatusType	InOutStatus;
	///用户标识  直达
	TShZdUserIDType	UserID;
	///出入金类型 直达
	TShZdInOutMoneyType InOutMoneyType;
	///出入金方式 直达
	TShZdInOutMoneyMedthType  InOutMedth;
	///资金用途 直达
	TShZdInOutMoneyUsingType  InOutMoneyUsing;
	///金额  直达
	TShZdTradeAmountType	Amount;
	///备注  直达
	TShZdDescribingType  Describing;
};

///请求直达出入金
struct CTShZdQryTransferSerialField
{	
	///资金账号  直达
	TShZdAccountIDType	AccountID;	
	///用户标识  直达
	TShZdUserIDType	UserID;
	///出入金类型  直达
	TShZdInOutMoneyType InOutMoneyType;
	///出入金方式  直达
	TShZdInOutMoneyMedthType  InOutMedth;
	///资金用途 直达
	TShZdInOutMoneyUsingType  InOutMoneyUsing;
	///币种代码  直达
	TShZdCurrencyIDType	CurrencyID;
	///金额  直达
	TShZdTradeAmountType	Amount;
	///备注  直达
	TShZdDescribingType  Describing;
};

//直达市场开收盘时间请求
struct CTShZdReqOpenCloseTimeField
{
	///用户代码  直达
	TShZdUserIDType	UserID;
};

///直达交易开市时间和闭市时间
struct CTShZdOpenCloseTimeField
{
	///用户标识  直达
	TShZdUserIDType	UserID;
	///交易日期  直达
	TShZdTradeDateType TradeDate;
	///开盘时间  直达
	TShZdTradeTimeType OpenTime;
	///收盘时间  直达
	TShZdTradeTimeType CloseTime;
};

///查询直达支持的市场开收盘时间
struct CTShZdReqMarketOpenCloseTimeField
{
	///用户标识  直达
	TShZdUserIDType	UserID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;	
};

///直达市场的开收盘时间
struct CTShZdMarketOpenCloseTimeField
{
	///年份  直达  
	TShZdTradeTimeType CurrYear;
	///夏令开始时间  直达
	TShZdTradeTimeType SummeyBeginTime;
	///冬令开始时间  直达
	TShZdTradeTimeType WinterBeginTime;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;		
	///交易所名称  直达
	TShZdExchangeNameType ExchangeName;
};

///查询直达产品的开收盘时间
struct CTShZdReqCommonOpenCloseTimeField
{
	///用户标识  直达
	TShZdUserIDType	UserID;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///产品代码  直达
	TShZdInstrumentIDType	ProductID;
};

///直达产品开收盘时间的返回
struct CTShZdCommonOpenCloseTimeField
{
	///产品是否跨日交易  直达
	TShZdTradeCrossDayType  TradeDay;
	///状态  直达
	TShZdCommonStageType  Stage;
	///普通时间、冬夏令时  直达
	TShZdTNswTimeType  NomalSummerWinter;
	///开盘时间  直达
	TShZdTradeTimeType OpenTime;
	///收盘时间  直达
	TShZdTradeTimeType CloseTime;
	///产品代码  直达
	TShZdInstrumentIDType	ProductID;
	///产品名称  直达
	TShZdProductNameType  ProductName;
	///交易所代码  直达
	TShZdExchangeIDType	ExchangeID;
	///交易所名称  直达
	TShZdExchangeNameType ExchangeName;		
};

///直达交易资金变化
struct CTShZdTradeMoneyField
{
	///用户标识  直达
	TShZdUserIDType	UserID;
	///资金账号  直达
	TShZdAccountIDType	AccountID;
	///币种代码  直达
	TShZdCurrencyIDType	CurrencyID;
	///昨可用 直达
	TShZdMoneyType	PreUse;
	///今结存 直达
	TShZdMoneyType	CurrDeposit;
	///平仓盈亏  直达
	TShZdMoneyType	CloseProfit;
	///冻结的资金  直达
	TShZdMoneyType	FrozenDeposit;	
	///手续费  直达
	TShZdMoneyType	Commission;
	///当前保证金总额  直达
	TShZdMoneyType	CurrMargin;	
	///维持保证金  直达
	TShZdMoneyType	KeepMargin;		
	///入金金额   直达
	TShZdMoneyType	Deposit;
	///出金金额   直达
	TShZdMoneyType	Withdraw;
	///未到期平盈   直达
	TShZdMoneyType	UnCloseProfit;
	///未冻结平盈  直达
	TShZdMoneyType	UnFrozenCloseProfit;
	///期权利金  直达
	TShZdMoneyType	RoyaltyMargin;	
};

///直达汇率查询
struct CTShZdReqMoneyRatioField
{
	///用户标识  直达
	TShZdUserIDType	UserID;
};

struct CTShZdMoneyRatioField
{
	///币种代码  直达
	TShZdCurrencyIDType	CurrencyID;
	///是否基础货币 
	TShZdBoolType IsBaseCurrency;
	///汇率
	TShZdMoneyType Ratio;
};

#pragma pack(pop)
#endif
