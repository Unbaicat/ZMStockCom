#!/usr/bin/env python
# -*- coding: utf-8 -*-
from win32com import client
import time

def OnLoginEvent(piTrade, nTradeID, bstrHost, nPort, bOK):
    IDispatch = getattr(piTrade, "_oleobj_", piTrade)
    print('IDispatch = %s' %IDispatch)
    print('------OnLoginEvent ： piTrade=%s, nTradeID=%s, bstrHost=%s, nPort=%s, bOK=%s' %(piTrade, nTradeID, bstrHost, nPort, bOK))

def OnStockQuoteEvent(nReqID, bstrCode, piRecord):
    print('------OnStockQuoteEvent : nReqID=%s, bstrCode=%s, piRecord=%s' %(nReqID, bstrCode, piRecord))

def OnServerChangedEvent(nPreTradeID, nCurTradeID):
    print('------OnServerChangedEvent : nPreTradeID=%s, nCurTradeID=%s' %(nPreTradeID, nCurTradeID))

def OnInitEvent(piTrade, bOK):
    print('------OnInitEvent : piTrade=%s, bOK=%s' %(piTrade, bOK))
    pass

def OnOrderSuccessEvent(bstrOrderID, bstrJson):
    print('------OnOrderSuccessEvent : bstrOrderID=%s, bstrJson=%s' %(bstrOrderID, bstrJson))
    pass

def OnOrderOKEvent(nReqID, eExchangeType, piRecord):
    print('------OnOrderOKEvent : nReqID=%s, eExchangeType=%s, piRecord=%s' %(nReqID, eExchangeType, piRecord))
    pass

def OnOrderErrEvent(nReqID, bstrErrDesc):
    print('------OnOrderErrEvent : nReqID=%s, bstrErrDesc=%s' %(nReqID, bstrErrDesc))

def OnServerErrEvent(nTradeID, nReqID):
    print('------OnServerErrEvent : nTradeID=%s, nReqID=%s' %(nTradeID, nReqID))

dll = client.gencache.EnsureModule(typelibCLSID='{9F3A40BA-CEAF-4DB4-BD2F-EFFCBBC8BF59}', lcid=0x0, major=1, minor=0)
StockTrade = dll.StockTrade()
event = dll._ITradeEvents(oobj = StockTrade)

event.OnLoginEvent = OnLoginEvent
event.OnStockQuoteEvent = OnStockQuoteEvent
event.OnServerChangedEvent = OnServerChangedEvent
event.OnInitEvent = OnInitEvent
event.OnOrderSuccessEvent = OnOrderSuccessEvent
event.OnOrderOKEvent = OnOrderOKEvent
event.OnServerErrEvent = OnServerErrEvent

#设置使用内核0
StockTrade.TradeType = 0
#设置运行输出详细日志
StockTrade.EnableLog = 1
#设置自动保持连接
StockTrade.AutoKeepConn = 1

#指定您的券商的通达信客户端版本号进行初始化
ret = StockTrade.Init("8.36", 1)
print(ret)
time.sleep(100)

#设置您的券商ID
StockTrade.BrokerType = 32
/// 账号登录类型，一般是8资金账号或9客户号
StockTrade.AccountType = 8
/// 你的券商交易服务器IP及端口
StockTrade.CurServerHost = "180.153.18.180"
StockTrade.CurServerPort = 7708
/// 营业部ID
StockTrade.DepartmentID = 1
#登录ID和密码必须填
StockTrade.LoginID = ""
StockTrade.TradePassword = ""
#交易账号和通讯密码，一般不需要填，个别券商需要填写
StockTrade.TradeAccount = ""
StockTrade.CommPassword = ""

#登录
boolLogin = StockTrade.LogIn(True)
print('StockTrade LogIn : %s' % boolLogin)

HolderCode = StockTrade.ShareHolderCode
print('股东代码 = %s' %(HolderCode.GetJsonString()))

CapitalInfo  = StockTrade.CapitalInfo
print('资金信息 = %s' %(CapitalInfo.GetJsonString()))

StockRecord = StockTrade.GetStockQuote(StockTrade.CurTradeID, "000001")
print('5档行情 : %s' %(StockRecord.GetJsonString()))

#查询资金数据
StockRecord1 = StockTrade.QueryTradeData(nTradeID=StockTrade.CurTradeID, eOrderType=1)
print('1 查询资金数据 QueryTradeData  eOrderType=1 : %s' %(StockRecord1.GetJsonString()))

StockRecord2 = StockTrade.QueryTradeData(nTradeID=StockTrade.CurTradeID, eOrderType=2)
print('2 查询当前持仓 QueryTradeData  eOrderType=2 : %s' %(StockRecord2.GetJsonString()))

StockRecord3 = StockTrade.QueryTradeData(nTradeID=StockTrade.CurTradeID, eOrderType=3)
print('3 查询当日委托 QueryTradeData  eOrderType=3 : %s' %(StockRecord3.GetJsonString()))

StockRecord4 = StockTrade.QueryTradeData(nTradeID=StockTrade.CurTradeID, eOrderType=4)
print('4 查询当日成交 QueryTradeData  eOrderType=4 : %s' %(StockRecord4.GetJsonString()))

StockRecord5 = StockTrade.QueryTradeData(nTradeID=StockTrade.CurTradeID, eOrderType=5)
print('5 查询当日可撤单委托 QueryTradeData  eOrderType=5 : %s' %(StockRecord5.GetJsonString()))

# 批量委托，可多次调用AddOrder后统一提交委托
AddOrder = StockTrade.AddOrder(eOrderType=1, ePriceType=1, bstrZqdm='000333', ftPrice=38.0, nCount=100, eExchangeType=1)
print('AddOrder : %s' %(AddOrder))
CommitOrder = StockTrade.CommitOrder(nTradeID=StockTrade.CurTradeID, bAutoCheckConn=True, ePriType=0)
print('批量委托返回 : %s' %(CommitOrder))

CancelOrder = StockTrade.CancelOrder(eExchangeType = 1 , bstrOrderID = '208723')
print('撤单返回 = %s' %(CommitOrder))

time.sleep(5)

StockTrade.LogOut(nTradeID="0")