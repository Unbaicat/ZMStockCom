#pragma once

// 交易L2行情处理辅助类
class CMarketL2Event : 
	public CComObjectRoot,
	//实现L2行情事件通知接口
	public _IMarketLevel2Events
{
protected:

	/// 父窗口
	HWND			m_hParentWnd;
	/// 请求ID
	ULONG			m_nRequestID;

	/// 返回数据缓存
	ITradeRecordPtr	m_spiRecord;

	/// 错误描述
	CString			m_strErrInfo;

public:
	CMarketL2Event()
		:m_hParentWnd(NULL)
		,m_spiRecord(NULL)
		,m_nRequestID(0)
	{
	}

	~CMarketL2Event()
	{
		m_spiRecord = NULL;
	}

	void SetCallPara(HWND hParentWnd)
	{
		if(NULL != hParentWnd)
		{
			if(!::IsWindow(hParentWnd))
				return;
		}
		m_hParentWnd = hParentWnd;
	}

	ULONG GetReqID()
	{
		return m_nRequestID;
	}

	ITradeRecordPtr GetReqData()
	{
		return m_spiRecord;
	}

BEGIN_COM_MAP(CMarketL2Event)
	COM_INTERFACE_ENTRY(_IMarketLevel2Events)
END_COM_MAP()

public:

	STDMETHODIMP GetTypeInfoCount(UINT*)
    {
       return E_NOTIMPL;
    }

    STDMETHODIMP GetTypeInfo( UINT iTInfo,LCID lcid,ITypeInfo **ppTInfo)
    { 
       return E_NOTIMPL;
    }

    STDMETHODIMP GetIDsOfNames( REFIID riid,LPOLESTR *rgszNames,UINT cNames,
                    LCID lcid,DISPID *rgDispId)
    { 
      return E_NOTIMPL;
    }

	STDMETHOD(Invoke)(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS *pDispParams,
               VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr);
	
	//  _IMarketLevel2Events
	/// 初始化接口结果通知
	STDMETHOD(InitEvent)(IDispatch* piMarket,VARIANT_BOOL bOK);
	/// 连接服务器结果通知
	STDMETHOD(ConnectEvent)(IDispatch* piMarket, ULONG nConnID,BSTR bstrHost,USHORT nPort,VARIANT_BOOL bOK);
	/// 查询指定股票实时5档行情回报
	STDMETHOD(StockQuoteEvent)(ULONG nReqID,VARIANT_BOOL bFive,IDispatch* piRecordInfo);
	/// 查询指定股票分时行情回报
	STDMETHOD(StockMinuteEvent)(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo);
	/// 查询指定股票分时成交回报
	STDMETHOD(StockTransEvent)(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo);
	/// 查询指定股票K线回报
	STDMETHOD(StockBarsEvent)(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo);
	/// 查询指定股票逐笔成交回报
	STDMETHOD(StockDetailTransEvent)(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo);
	/// 查询指定股票逐笔委托回报
	STDMETHOD(StockDetailOrderEvent)(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo);
	/// 查询指定股票买卖队列回报
	STDMETHOD(StockOrderListEvent)(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo);

	/// 服务器错误通知
	STDMETHOD(ServerErrEvent)(ULONG nConnID,ULONG nReqID);
};