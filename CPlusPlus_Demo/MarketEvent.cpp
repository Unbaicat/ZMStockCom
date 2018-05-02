/////////////////////////////////////////////////////////////////////////
/*@
	文件名称：	MarketEvent.cpp
	文件描述：	实现TDX COM组件行情事件通知处理
*/
/////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarketEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CMarketEvent

STDMETHODIMP CMarketEvent::Invoke( DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,
                DISPPARAMS *pDispParams,VARIANT *pVarResult,EXCEPINFO *pExcepInfo,UINT *puArgErr)
{
	HRESULT hRet(S_OK);
	switch( dispIdMember )
	{
		case 0x00000001:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varConnRet,varTradeConn;
			VariantInit(&varConnRet);
			VariantInit(&varTradeConn);
			hRet = VariantChangeTypeEx( &varTradeConn,&(pDispParams->rgvarg[1]),lcid,0,VT_DISPATCH);
			hRet = VariantChangeTypeEx( &varConnRet,&(pDispParams->rgvarg[0]),lcid,0,VT_BOOL);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = InitEvent(varTradeConn.pdispVal,varConnRet.boolVal);
			VariantClear(&varConnRet);
			VariantClear(&varTradeConn);
			break;
		}

		case 0x00000002:
		{
			if ( pDispParams->cArgs != 5 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varConnRet,varTrade,varHost,varPort,varTradeConn;
			VariantInit(&varConnRet);
			VariantInit(&varTrade);
			VariantInit(&varPort);
			VariantInit(&varHost);
			VariantInit(&varTradeConn);
			hRet = VariantChangeTypeEx( &varTradeConn,&(pDispParams->rgvarg[4]),lcid,0,VT_DISPATCH);
			hRet = VariantChangeTypeEx( &varTrade,&(pDispParams->rgvarg[3]),lcid,0,VT_UI2);
			hRet = VariantChangeTypeEx( &varHost,&(pDispParams->rgvarg[2]),lcid,0,VT_BSTR);
			hRet = VariantChangeTypeEx( &varPort,&(pDispParams->rgvarg[1]),lcid,0,VT_UI2);
			hRet = VariantChangeTypeEx( &varConnRet,&(pDispParams->rgvarg[0]),lcid,0,VT_BOOL);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = ConnectEvent(varTradeConn.pdispVal,varTrade.uiVal,varHost.bstrVal,varPort.uiVal,varConnRet.boolVal);
			VariantClear(&varConnRet);
			VariantClear(&varTrade);
			VariantClear(&varHost);
			VariantClear(&varPort);
			VariantClear(&varTradeConn);
			break;
		}

		case 0x00000003:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varReqID,varRecord;
			VariantInit(&varReqID);
			VariantInit(&varRecord);
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[1]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varRecord,&(pDispParams->rgvarg[0]),lcid,0,VT_DISPATCH);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = StockQuoteEvent(varReqID.ulVal,varRecord.pdispVal);
			VariantClear(&varReqID);
			VariantClear(&varRecord);
			break;
		}

		case 0x00000004:
		{
			if ( pDispParams->cArgs != 3 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varReqID,varCode,varRecord;
			VariantInit(&varReqID);
			VariantInit(&varCode);
			VariantInit(&varRecord);
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[2]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varCode,&(pDispParams->rgvarg[1]),lcid,0,VT_BSTR);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varRecord,&(pDispParams->rgvarg[0]),lcid,0,VT_DISPATCH);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = StockMinuteEvent(varReqID.ulVal,varCode.bstrVal,varRecord.pdispVal);
			VariantClear(&varReqID);
			VariantClear(&varCode);
			VariantClear(&varRecord);
			break;
		}

		case 0x00000005:
		{
			if ( pDispParams->cArgs != 3 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varReqID,varCode,varRecord;
			VariantInit(&varReqID);
			VariantInit(&varCode);
			VariantInit(&varRecord);
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[2]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varCode,&(pDispParams->rgvarg[1]),lcid,0,VT_BSTR);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varRecord,&(pDispParams->rgvarg[0]),lcid,0,VT_DISPATCH);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = StockTransEvent(varReqID.ulVal,varCode.bstrVal,varRecord.pdispVal);
			VariantClear(&varReqID);
			VariantClear(&varCode);
			VariantClear(&varRecord);
			break;
		}

		case 0x00000006:
		{
			if ( pDispParams->cArgs != 3 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varReqID,varCode,varRecord;
			VariantInit(&varReqID);
			VariantInit(&varCode);
			VariantInit(&varRecord);
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[2]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varCode,&(pDispParams->rgvarg[1]),lcid,0,VT_BSTR);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varRecord,&(pDispParams->rgvarg[0]),lcid,0,VT_DISPATCH);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = StockBarsEvent(varReqID.ulVal,varCode.bstrVal,varRecord.pdispVal);
			VariantClear(&varReqID);
			VariantClear(&varCode);
			VariantClear(&varRecord);
			break;
		}

		case 0x00000009:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varTradeID,varReqID;
			VariantInit(&varTradeID);
			VariantInit(&varReqID);
			hRet = VariantChangeTypeEx( &varTradeID,&(pDispParams->rgvarg[1]),lcid,0,VT_UI2);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[0]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = ServerErrEvent(varTradeID.uiVal,varReqID.ulVal);
			VariantClear(&varTradeID);
			VariantClear(&varReqID);
			break;
		}

		default:
			break;
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::InitEvent(IDispatch* piMarket,VARIANT_BOOL bOK)
{
	HRESULT hRet(E_FAIL);
	m_spiMarket = NULL;
	m_spiMarket = piMarket;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		BOOL bInitFlag = FALSE;
		if(VARIANT_TRUE == bOK)
		{
			bInitFlag = TRUE;
		}
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_INITRETURN,(WPARAM)bInitFlag,(LPARAM)m_nConnIndex);
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::ConnectEvent(IDispatch* piMarket,USHORT nConnID,BSTR bstrHost,USHORT nPort,VARIANT_BOOL bOK)
{
	HRESULT hRet(E_FAIL);
	m_spiMarket = NULL;
	m_spiMarket = piMarket;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		CString strLogInfo;
		BOOL bConnected = FALSE;
		USHORT nCurConnID = 0;
		m_spiMarket->get_CurConnID(&nCurConnID);
		if(VARIANT_TRUE == bOK)
		{
			strLogInfo.Format(L"主机 %s 端口 %d 连接成功，对应普通行情ID:%d 主交易ID:%d",\
				CString(bstrHost),nPort,nConnID,nCurConnID);
			bConnected = TRUE;
		}
		else
		{
			ATLASSERT(m_spiMarket);
			CComBSTR bstrErrInfo;
			m_spiMarket->get_LastErrDesc(&bstrErrInfo);
			strLogInfo.Format(L"主机 %s 端口 %d 连接失败，错误原因:%s",CString(bstrHost),nPort,CString(bstrErrInfo.m_str));
			bstrErrInfo.Empty();
		}
		if(nCurConnID == nConnID)
		{
			/// 只处理主服务器登录，采用阻塞方式通知，确保拿到接口前没有被释放
			::PostMessage(m_hParentWnd,WM_TRADEEVENT_LOGINRETURN,(WPARAM)bConnected,(LPARAM)m_nConnIndex);
		}
#ifdef _DEBUG
		/// 测试代码
		else
		{
			/// 其它服务器登录的交易ID，可以用来获取行情等，买卖操作用主主服务器
			/// 如果需要，你可以在这记录都有哪些成功的交易ID以备后用，释放连接时指定交易ID，或者指定0释放所有连接
			ITradeRecordPtr spiRecord = NULL;
			CString strGetCode;
			int nIndex = nConnID%5;
			switch(nIndex)
			{
			case 0:
				strGetCode = L"000001";
				break;
			case 1:
				strGetCode = L"000002";
				break;
			case 2:
				strGetCode = L"000005";
				break;
			case 3:
				strGetCode = L"000006";
				break;
			case 4:
				strGetCode = L"000008";
				break;
			default:
				strGetCode = L"600000";
			}
			/// 记录请求ID，事件通知会携带此请求ID，这样可以匹配请求任务
			ULONG nReqID = 0;
			m_spiMarket->GetQuotes(nConnID,CComBSTR(strGetCode),&nReqID);
		}
#endif
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::ServerErrEvent(USHORT nConnID,ULONG nReqID)
{
	HRESULT hRet(E_FAIL);
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_SERVERERRVER,(WPARAM)nConnID,(LPARAM)m_nConnIndex);
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::StockQuoteEvent(ULONG nReqID,IDispatch* piRecordInfo)
{
	HRESULT hRet(E_FAIL);
	m_spiRecord = NULL;
	if(NULL == piRecordInfo)
		return hRet;
	hRet = piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&m_spiRecord);
	if(NULL == m_spiRecord)
		return hRet;
	m_nRequestID = nReqID;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// 提前缓存了记录对象，所以用PostMessage
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_STOCKQUOTE,nReqID,(LPARAM)m_nConnIndex);
	}
	else
	{
#ifdef _DEBUG
		/// 解析消息
		CComBSTR bstrText;
		m_spiRecord->GetJsonString(&bstrText);
//		::MessageBox(NULL,CString(_T("收到实时行情内容："))+(CString)bstrCode.m_str,ZM_APPLICATION_NAME,MB_OK);
		bstrText.Empty();
#endif
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::StockTransEvent(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo)
{
	HRESULT hRet(E_FAIL);
	m_spiRecord = NULL;
	if(NULL == piRecordInfo)
		return hRet;
	hRet = piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&m_spiRecord);
	if(NULL == m_spiRecord)
		return hRet;
	m_nRequestID = nReqID;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// 提前缓存了记录对象，所以用PostMessage
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_STOCKTRANS,nReqID,(LPARAM)m_nConnIndex);
	}
	else
	{
#ifdef _DEBUG
		/// 解析消息
		CComBSTR bstrText;
		m_spiRecord->GetJsonString(&bstrText);
//		::MessageBox(NULL,CString(_T("收到行情内容："))+(CString)bstrCode.m_str,ZM_APPLICATION_NAME,MB_OK);
		bstrText.Empty();
#endif
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::StockMinuteEvent(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo)
{
	HRESULT hRet(E_FAIL);
	m_spiRecord = NULL;
	if(NULL == piRecordInfo)
		return hRet;
	hRet = piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&m_spiRecord);
	if(NULL == m_spiRecord)
		return hRet;
	m_nRequestID = nReqID;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// 提前缓存了记录对象，所以用PostMessage
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_STOCKMINUTE,nReqID,(LPARAM)m_nConnIndex);
	}
	else
	{
#ifdef _DEBUG
		/// 解析消息
		CComBSTR bstrText;
		m_spiRecord->GetJsonString(&bstrText);
//		::MessageBox(NULL,CString(_T("收到行情内容："))+(CString)bstrCode.m_str,ZM_APPLICATION_NAME,MB_OK);
		bstrText.Empty();
#endif
	}
	return hRet;
}

STDMETHODIMP CMarketEvent::StockBarsEvent(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo)
{
	HRESULT hRet(E_FAIL);
	m_spiRecord = NULL;
	if(NULL == piRecordInfo)
		return hRet;
	hRet = piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&m_spiRecord);
	if(NULL == m_spiRecord)
		return hRet;
	m_nRequestID = nReqID;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// 提前缓存了记录对象，所以用PostMessage
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_STOCKBARS,nReqID,(LPARAM)m_nConnIndex);
	}
	else
	{
#ifdef _DEBUG
		/// 解析消息
		CComBSTR bstrText;
		m_spiRecord->GetJsonString(&bstrText);
//		::MessageBox(NULL,CString(_T("收到行情内容："))+(CString)bstrCode.m_str,ZM_APPLICATION_NAME,MB_OK);
		bstrText.Empty();
#endif
	}
	return hRet;
}