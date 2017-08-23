#ifndef JSGATEWAY_H
#define JSGATEWAY_H
//抽象类，给所有接口类继承
#include<string>
#include"structs.hpp"
#include"eventengine.h"
class JSGateway
{
public:
    explicit JSGateway::JSGateway(EventEngine *eventengine)
    {
        this->eventengine = eventengine;
    }

    void JSGateway::onTick(std::shared_ptr<Event_Tick>e)
    {
        this->eventengine->put(e);
    }
    void JSGateway::onTrade(std::shared_ptr<Event_Trade>e)
    {
        this->eventengine->put(e);
    }
    void JSGateway::onOrder(std::shared_ptr<Event_Order>e)
    {
        this->eventengine->put(e);
    }
    void JSGateway::onPosition(std::shared_ptr<Event_Position>e)
    {
        this->eventengine->put(e);
    }
    void JSGateway::onAccount(std::shared_ptr<Event_Account>e)
    {
        this->eventengine->put(e);
    }
    void JSGateway::onError(std::shared_ptr<Event_Error>e)
    {
        this->eventengine->put(e);
    }
    void JSGateway::onLog(std::shared_ptr<Event_Log>e)
    {
        this->eventengine->put(e);
    }

    void JSGateway::onContract(std::shared_ptr<Event_Contract>e)
    {
        this->eventengine->put(e);
    }
    virtual void connect() = 0;																					//连接
	virtual void subscribe(const jsstructs::SubscribeReq& subscribeReq) = 0;									//订阅
	virtual std::string sendOrder(const jsstructs::OrderReq & req) = 0;											//发单
	virtual void cancelOrder(const jsstructs::CancelOrderReq & req) = 0;										//撤单
    virtual void qryAccount() = 0;																				//查询账户资金
    virtual void qryPosition() = 0;																				//查询持仓
    virtual void close() = 0;																					//断开API
private:
    EventEngine *eventengine;
};
#endif
