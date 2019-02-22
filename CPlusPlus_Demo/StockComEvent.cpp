/////////////////////////////////////////////////////////////////////////
/*@
	�ļ����ƣ�	StockComEvent.cpp
	�ļ�������	ʵ��TDX COM����¼�֪ͨ����
*/
/////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StockComEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CStockComEvent

STDMETHODIMP CStockComEvent::Invoke( DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,
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
			hRet = VariantChangeTypeEx( &varTrade,&(pDispParams->rgvarg[3]),lcid,0,VT_UI4);
			hRet = VariantChangeTypeEx( &varHost,&(pDispParams->rgvarg[2]),lcid,0,VT_BSTR);
			hRet = VariantChangeTypeEx( &varPort,&(pDispParams->rgvarg[1]),lcid,0,VT_UI2);
			hRet = VariantChangeTypeEx( &varConnRet,&(pDispParams->rgvarg[0]),lcid,0,VT_BOOL);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = LoginEvent(varTradeConn.pdispVal,varTrade.ulVal,varHost.bstrVal,varPort.uiVal,varConnRet.boolVal);
			VariantClear(&varConnRet);
			VariantClear(&varTrade);
			VariantClear(&varHost);
			VariantClear(&varPort);
			VariantClear(&varTradeConn);
			break;
		}

		case 0x00000003:
		{
			if ( pDispParams->cArgs != 3 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varRecord,varExchangeID,varReqID;
			VariantInit(&varRecord);
			VariantInit(&varExchangeID);
			VariantInit(&varReqID);
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[2]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varExchangeID,&(pDispParams->rgvarg[1]),lcid,0,VT_UI2);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varRecord,&(pDispParams->rgvarg[0]),lcid,0,VT_DISPATCH);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet=OrderOKEvent(varReqID.ulVal,(EZMExchangeType)(varExchangeID.uiVal),varRecord.pdispVal);
			VariantClear(&varRecord);
			VariantClear(&varExchangeID);
			VariantClear(&varReqID);
			break;
		}
		case 0x00000004:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varErrInfo,varReqID;
			VariantInit(&varErrInfo);
			VariantInit(&varReqID);
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[1]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varErrInfo,&(pDispParams->rgvarg[0]),lcid,0,VT_BSTR);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet=OrderErrEvent(varReqID.ulVal,varErrInfo.bstrVal);
			VariantClear(&varErrInfo);
			VariantClear(&varReqID);
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
			hRet=StockQuoteEvent(varReqID.ulVal,varCode.bstrVal,varRecord.pdispVal);
			VariantClear(&varReqID);
			VariantClear(&varCode);
			VariantClear(&varRecord);
			break;
		}
		case 0x00000006:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varOrderID,varJson;
			VariantInit(&varJson);
			VariantInit(&varOrderID);
			hRet = VariantChangeTypeEx( &varOrderID,&(pDispParams->rgvarg[1]),lcid,0,VT_BSTR);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varJson,&(pDispParams->rgvarg[0]),lcid,0,VT_BSTR);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = OrderSuccessEvent(varOrderID.bstrVal,varJson.bstrVal);
			VariantClear(&varOrderID);
			VariantClear(&varOrderID);
			break;
		}

		case 0x00000008:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varTradeID,varReqID;
			VariantInit(&varTradeID);
			VariantInit(&varReqID);
			hRet = VariantChangeTypeEx( &varTradeID,&(pDispParams->rgvarg[1]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = VariantChangeTypeEx( &varReqID,&(pDispParams->rgvarg[0]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = ServerErrEvent(varTradeID.ulVal,varReqID.ulVal);
			VariantClear(&varTradeID);
			VariantClear(&varReqID);
			break;
		}
		case 0x00000009:
		{
			if ( pDispParams->cArgs != 2 )
				return DISP_E_BADPARAMCOUNT;
			if ( pDispParams->cNamedArgs )
				return DISP_E_NONAMEDARGS;

			CComVariant varPreTradeID,varCurTradeID;
			VariantInit(&varPreTradeID);
			VariantInit(&varCurTradeID);
			hRet = VariantChangeTypeEx( &varPreTradeID,&(pDispParams->rgvarg[1]),lcid,0,VT_UI4);
			hRet = VariantChangeTypeEx( &varCurTradeID,&(pDispParams->rgvarg[0]),lcid,0,VT_UI4);
			if FAILED(hRet)
				return DISP_E_BADVARTYPE;
			hRet = ServerChangedEvent(varPreTradeID.ulVal,varCurTradeID.ulVal);
			VariantClear(&varPreTradeID);
			VariantClear(&varCurTradeID);
			break;
		}
		default:
			break;
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::InitEvent(IDispatch* piTrade,VARIANT_BOOL bOK)
{
	HRESULT hRet(E_FAIL);
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		BOOL bInitFlag = FALSE;
		if(VARIANT_TRUE == bOK)
			bInitFlag = TRUE;
		/// ����������ʽ֪ͨ��ȷ���õ����׽ӿ�ǰû�б��ͷ�
		::SendMessage(m_hParentWnd,WM_TRADEEVENT_INITRETURN,(WPARAM)bInitFlag,(LPARAM)piTrade);
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::LoginEvent(IDispatch* piTrade, ULONG nTradeID,BSTR bstrHost,USHORT nPort,VARIANT_BOOL bOK)
{
	HRESULT hRet(E_FAIL);
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		BOOL bConnected = FALSE;
		ULONG nCurTradeID = 0;
		ATLASSERT(piTrade);
		IStockTradePtr spiTrade = NULL;
		if(NULL != piTrade)
			piTrade->QueryInterface(IID_IStockTrade,(LPVOID *)&spiTrade);
		ATLASSERT(spiTrade);
		if(VARIANT_TRUE == bOK)
		{
			bConnected = TRUE;
			spiTrade->get_CurTradeID(&nCurTradeID);
		}
		else
		{
			/// ��¼ʧ�ܣ�ȡ����ԭ��
			if(NULL != spiTrade)
			{
				CComBSTR bstrErrInfo;
				spiTrade->get_LastErrDesc(&bstrErrInfo);
				bstrErrInfo.Empty();
			}
		}
		if(nCurTradeID == nTradeID)
		{
			/// ֻ��������������¼������������ʽ֪ͨ��ȷ���õ����׽ӿ�ǰû�б��ͷ�
			::SendMessage(m_hParentWnd,WM_TRADEEVENT_LOGINRETURN,(WPARAM)bConnected,(LPARAM)piTrade);
		}
		else
		{
#ifdef _DEBUG
			/// ������������¼�Ľ���ID������������ȡ����ȣ���������������������
			/// �����Ҫ������������¼������Щ�ɹ��Ľ���ID�Ա����ã��ͷ�����ʱָ������ID������ָ��0�ͷ���������
			ITradeRecordPtr spiRecord = NULL;
			CString strGetCode;
			int nIndex = nTradeID%5;
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
			/// ͬ������5�����飬����֧�������ˣ��������ֺż��
			spiTrade->GetStockQuote(nTradeID,CComBSTR(strGetCode),&spiRecord);
			if(NULL != spiRecord)
			{
				ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
				HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
				hRet = spiRecord->get_RecordCount(&nRecordCount);

				CComBSTR bstrName;
				for(nIndex = 0;nIndex < nFieldCount;nIndex++)
				{
					/// ������ؼ�¼���ֶ�����
					EZMDataType eDataType = ZMDATATYPE_UNKNOW;
					spiRecord->GetDataType(nIndex,&eDataType);
					/// ������ؼ�¼���ֶ�����
					hRet = spiRecord->GetFieldName(nIndex,&bstrName);
					bstrName.Empty();
				}
				///  ������м�¼
				for(nIndex = 0;nIndex < nRecordCount;nIndex++)
				{
					for(ULONG n = 0;n < nFieldCount;n++)
					{
						/// ��ȡÿ���ֶ�ֵ
						CComVariant varVal;
						hRet = spiRecord->GetValue(nIndex,n,&varVal);
						varVal.Clear();
					}
				}
				spiRecord = NULL;
			}
#endif
		}
		spiTrade = NULL;
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::ServerErrEvent(ULONG nTradeID,ULONG nReqID)
{
	HRESULT hRet(E_FAIL);
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_SERVERERRVER,(WPARAM)nReqID,(LPARAM)nTradeID);
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::ServerChangedEvent(ULONG nPreTradeID, ULONG nCurTradeID)
{
	HRESULT hRet(E_FAIL);
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		::PostMessage(m_hParentWnd,WM_TRADEEVENT_CHANGESERVER,(WPARAM)nCurTradeID,(LPARAM)nPreTradeID);
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::OrderOKEvent(ULONG nRequestID,EZMExchangeType eExchangeType,IDispatch* piRecordInfo)
{
	HRESULT hRet(E_FAIL);
	if(NULL == piRecordInfo)
		return hRet;
	//hRet = piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&m_spiTradeRecord);
	//if(NULL == m_spiTradeRecord)
	//	return hRet;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// SendMessageȷ����¼����û���ͷ�
		::SendMessage(m_hParentWnd,WM_TRADEEVENT_SENDORDER, eExchangeType,(LPARAM)piRecordInfo);
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::OrderErrEvent(ULONG nReqID,BSTR bstrErrInfo)
{
	HRESULT hRet(E_FAIL);
	if(0 == nReqID)
		return hRet;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// SendMessageȷ���ַ���û�б��ͷ�
		::SendMessage(m_hParentWnd,WM_TRADEEVENT_ORDERERROR,nReqID,(LPARAM)bstrErrInfo);
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::OrderSuccessEvent(BSTR bstrOrderID,BSTR bstrJson)
{
	HRESULT hRet(E_FAIL);
	ATLASSERT(bstrOrderID);
	ATLASSERT(bstrJson);
	if(NULL == bstrOrderID)
		return hRet;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// ֱ�Ӵ���bstrJson����ҪSendMessage
		::SendMessage(m_hParentWnd,WM_TRADEEVENT_ORDERSUCCESS,(WPARAM)bstrOrderID,(LPARAM)bstrJson);
	}
	return hRet;
}

STDMETHODIMP CStockComEvent::StockQuoteEvent(ULONG nReqID,BSTR bstrCode,IDispatch* piRecordInfo)
{
	HRESULT hRet(E_FAIL);
	if(NULL == piRecordInfo)
		return hRet;
	if(NULL != m_hParentWnd && ::IsWindow(m_hParentWnd))
	{
		/// SendMessageȷ����¼����û���ͷ�
		::SendMessage(m_hParentWnd,WM_TRADEEVENT_STOCKQUOTE,nReqID,(LPARAM)piRecordInfo);
	}
	return hRet;
}