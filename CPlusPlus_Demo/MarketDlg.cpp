// MarketDlg.cpp : implementation of the CMarketDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MarketDlg.h"
#include <atlcomtime.h>

extern LONG StrToNum(const ATL::CString& strTemp);

HRESULT CMarketDlg::AdviseMarketCom(int nIndex)
{
	if(NULL != m_spiMarketEvent[nIndex])
		return S_FALSE;
	m_nMarketEventCookie[nIndex] = NULL;
	this->m_spiMarketEvent[nIndex] = new CComObject <CMarketEvent>;
	ATLASSERT(this->m_spiMarketEvent[nIndex]);
	if(NULL == this->m_spiMarketEvent[nIndex])
		return E_POINTER;	
	/// ���ûص������������¼�����ʱƥ����˭������
	m_spiMarketEvent[nIndex]->SetCallPara(this->m_hWnd,nIndex);
	//�����¼�����
	HRESULT hRet = AtlAdvise(m_spiMarket[nIndex],this->m_spiMarketEvent[nIndex]->GetUnknown(),
		DIID__IMarketEvents,&this->m_nMarketEventCookie[nIndex]);
	ATLASSERT(SUCCEEDED(hRet));
	return hRet;
}

void CMarketDlg::UnAdviseMarketCom(int nIndex)
{
	if(m_nMarketEventCookie[nIndex] && NULL != m_spiMarket[nIndex])
	{
		AtlUnadvise(m_spiMarket[nIndex],DIID__IMarketEvents,m_nMarketEventCookie[nIndex]);
		m_nMarketEventCookie[nIndex] = 0;
	}
	if(NULL != m_spiMarketEvent[nIndex])
		m_spiMarketEvent[nIndex] = NULL;
	if(NULL != m_spiMarket[nIndex])
	{
		ULONG nCurConnID = 0;
		m_spiMarket[nIndex]->get_CurConnID(&nCurConnID);
		if(nCurConnID)
			m_spiMarket[nIndex]->Disconnect(nCurConnID);
		m_spiMarket[nIndex] = NULL;
	}
}

LRESULT CMarketDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	m_spiMarket[0] = NULL;
	m_nMarketEventCookie[0] = 0;
	m_spiMarketEvent[0] = NULL;

	if(ZM_ACCOUNT_CONNMAXCOUNT > 1)
	{
		m_spiMarket[1] = NULL;
		m_nMarketEventCookie[1] = 0;
		m_spiMarketEvent[1] = NULL;
	}

	/// ������ͨ��վZ1
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"202.108.253.130");
	/// ������ͨ��վZ2
//	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"������ͨ��վZ1");
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).SetWindowText(L"7709");

	this->GetDlgItem(IDC_EDIT_CODE).SetWindowText(L"000001");

	return TRUE;
}

LRESULT CMarketDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	ReleaseCom();
	EndDialog(wID);
	return 0;
}

LRESULT CMarketDlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam > 1)
		return 0;
	IMarketPtr spiMarket = m_spiMarketEvent[lParam]->GetMarket();
	if(NULL == spiMarket)
		return 0;/// �����׽ӿ�
	if(wParam)
	{
		/// ����
		VARIANT_BOOL bRet = VARIANT_FALSE;
#ifdef ZM_TDXSERVER_SYNC
		HRESULT hRet = spiMarket->Connect(VARIANT_FALSE,&bRet);
#else
		HRESULT hRet = spiMarket->Connect(VARIANT_TRUE,&bRet);
#endif
		if(VARIANT_FALSE == bRet)
		{
			/// ��ȡ������Ϣ
			USHORT nErrCode = 0;
			CComBSTR bstrErrDesc;
			spiMarket->get_LastErrType(&nErrCode);
			spiMarket->get_LastErrDesc(&bstrErrDesc);
			if(bstrErrDesc.Length())
				this->MessageBox(bstrErrDesc.m_str);
			bstrErrDesc.Empty();
			this->UnAdviseMarketCom(0);
			return 0;
		}
#ifdef ZM_TDXSERVER_SYNC
		USHORT nCurConnID = 0;
		spiMarket->get_CurConnID(&nCurConnID);
		if(!nCurConnID)
		{
			USHORT nErrCode = 0;
			CComBSTR bstrErrDesc;
			spiMarket->get_LastErrType(&nErrCode);
			spiMarket->get_LastErrDesc(&bstrErrDesc);
			if(bstrErrDesc.Length())
				this->MessageBox(bstrErrDesc.m_str);
			bstrErrDesc.Empty();
			this->UnAdviseMarketCom(0);
			return 0;
		}
		this->MessageBox(L"��ͨ�������ӷ������ɹ���");
#endif

#ifdef _DEBUG
		if(m_spiMarket[0] == spiMarket)
		{
#ifdef ZM_MUTI_ACCOUNT
			/// ���˺�ʹ�ã��������COM�������
			HRESULT hRet = m_spiMarket[1].CreateInstance(__uuidof(Market));
			ATLASSERT(m_spiMarket[1]);
			if(NULL != m_spiMarket[1])
			{
				/// �����¼�����
				AdviseMarketCom(1);

				/// ���õ�����־���
				hRet = m_spiMarket[1]->put_EnableLog(VARIANT_TRUE);
				/// ��ʼ��ʹ�û���������ʵ��ֻ��Ҫ����һ�Σ����ݿͻ��˰汾�ţ����������(Ĭ��Ϊ1��ֻ�и߼����֧�ֶ������)
				/// �첽��������Ҫ�����¼�֪ͨ�õ���ʼ�����
				m_spiMarket[1]->Init(1);///������������������ö���������������������ֻ�ü���������
			}
#endif
		}
#endif
	}
	else
	{
		/// ����ʧ�ܣ�������������
		CComBSTR bstrErrInfo;
		spiMarket->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMarketDlg::OnConnReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam > 1)
		return 0;
	IMarketPtr spiMarket = m_spiMarketEvent[lParam]->GetMarket();
	if(NULL == spiMarket)
		return 0;/// �����׽ӿ�
	if(wParam)
	{
		ULONG nConnID = 0;
		HRESULT hRet = spiMarket->get_CurConnID(&nConnID);
		ATLASSERT(nConnID);
		ULONG nSpeed = 0;
		spiMarket->get_ConnSpeed(&nSpeed);/// �������ӷ�����ʱ��
		if(nConnID)
			this->MessageBox(L"�첽���ӷ������ɹ���");
#ifdef ZM_TDXSERVER_SYNC
		/// ͬ������ĳֻ��Ʊ��ʵʱ5������
		ITradeRecordPtr spiRecord = NULL;
		hRet = spiMarket->GetQuote(nConnID,CComBSTR(L"600000"),&spiRecord);
		if(NULL != spiRecord)
		{
			ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
			hRet = spiRecord->get_FieldCount(&nFieldCount);
			hRet = spiRecord->get_RecordCount(&nRecordCount);
			spiRecord = NULL;
		}
		else
		{
			CComBSTR bstrErrInfo;
			spiMarket->get_LastErrDesc(&bstrErrInfo);
			if(bstrErrInfo.Length())
				this->MessageBox(bstrErrInfo.m_str);
			bstrErrInfo.Empty();
		}
#else
		/// �첽�����ֻ��Ʊ��ʵʱ5�����飬��ָ�����ȼ��ͷ��ص��������кš�nReqID�����¼��ص��д���
		ULONG nReqID = 0;
		spiMarket->GetQuotes(nConnID,CComBSTR(L"000001;000002;600001"),&nReqID);
#endif
	}
	else
	{
		/// ����ʧ�ܣ�������������
		CComBSTR bstrErrInfo;
		spiMarket->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMarketDlg::OnServerError(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	USHORT nConnIndex = (USHORT)lParam;
	if(nConnIndex >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// ��Ч����
	if(NULL == m_spiMarket[nConnIndex])
		return 0;
	/// ����������������Ҫ���µ�¼ʹ��
	m_spiMarket[nConnIndex]->Disconnect(0);
	/// �����µ�¼
	return 0;
}

LRESULT CMarketDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// ��Ч����
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// �����Ѿ��ͷ�
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// ��ȡJSON�������������
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	if(bstrJsonVal.Length())
		MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// ���ݽӿڷ�����������
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// ������ؼ�¼���ֶ�����
		EZMDataType eDataType;
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
	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketDlg::OnStockMintue(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// ��Ч����
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// �����Ѿ��ͷ�
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// ��ȡJSON�������������
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	if(bstrJsonVal.Length())
		MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// ���ݽӿڷ�����������
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// ������ؼ�¼���ֶ�����
		EZMDataType eDataType;
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
	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketDlg::OnStockTrans(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// ��Ч����
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// �����Ѿ��ͷ�
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// ��ȡJSON�������������
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	if(bstrJsonVal.Length())
		MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// ���ݽӿڷ�����������
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// ������ؼ�¼���ֶ�����
		EZMDataType eDataType;
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
	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketDlg::OnStockBars(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// ��Ч����
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// �����Ѿ��ͷ�
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// ��ȡJSON�������������
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	if(bstrJsonVal.Length())
		MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// ���ݽӿڷ�����������
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// ������ؼ�¼���ֶ�����
		EZMDataType eDataType;
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
	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReleaseCom();
	EndDialog(wID);
	return 0;
}

void CMarketDlg::ReleaseCom()
{
	/// �ͷ�COM�������
	this->UnAdviseMarketCom(0);
#ifdef ZM_MUTI_ACCOUNT
	this->UnAdviseMarketCom(1);
#endif
}

LRESULT CMarketDlg::OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiMarket[0])
		return 0;
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	this->UnAdviseMarketCom(0);
	if(!nCurConnID)
	{
		this->MessageBox(L"δ���ӣ������ͷţ�");
	}
	else
		this->MessageBox(L"�ͷ����ӳɹ���");
	return 0;
}

LRESULT CMarketDlg::OnBnClickedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiMarket[0])
	{
		ULONG nConnID = 0;
		m_spiMarket[0]->get_CurConnID(&nConnID);
		if(nConnID)
			return 0;/// ��������
		/// �ͷ�����
		this->UnAdviseMarketCom(0);
	}
	/// ��ȡ��ʼ������
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).GetWindowText(&bstrServerPort);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());

	HRESULT hRet = m_spiMarket[0].CreateInstance(__uuidof(Market));
	ATLASSERT(m_spiMarket[0]);
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"������ͨ����ӿ�ʧ�ܣ������Ƿ��Ѿ��ɹ�ע��COM�����");
		return 0;
	}
	/// ���õ�����־���
	hRet = m_spiMarket[0]->put_EnableLog(VARIANT_TRUE);
	/// ���������������ַ
	m_spiMarket[0]->put_CurServerHost(bstrServerAddr);
	/// ��������������˿�
	m_spiMarket[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	hRet = this->AdviseMarketCom(0);

	ATLASSERT(NULL != m_spiMarket[0]);
	if(NULL != m_spiMarket[0])
	{
		/// ��ʼ��ʹ�ò��������������
		hRet = m_spiMarket[0]->Init(1);
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedGetf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ������Ϣ
	m_spiMarket[0]->GetFinanceInfo(nCurConnID,bstrGetCode,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		CComBSTR bstrText;
		spiRecordIndex->GetJsonString(&bstrText);
		CString strText = bstrText.m_str;
		bstrText.Empty();
	}
	else
	{
		CComBSTR bstrErrInfo;
		m_spiMarket[0]->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	spiRecordIndex = NULL;

	return 0;
}

LRESULT CMarketDlg::OnBnClickedKdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ���ʷK��
	USHORT nMaxGet = 800;
	LONG nStart = 5600;

	while(nStart >= 0)
	{
		/// ����ȡ800��¼
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1DAY,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		/// ��Ҫ�жϴ����ǹ�Ʊ����ָ�����ֱ���ò�ͬ�ķ���
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1DAY,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		CComBSTR bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValueString(nIndex,0,&bstrVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftOpen);
			spiRecordIndex->GetValueFloat(nIndex,2,&ftClose);
			spiRecordIndex->GetValueFloat(nIndex,3,&ftHigh);
			spiRecordIndex->GetValueFloat(nIndex,4,&ftLow);
			CString strDay(bstrVal.m_str);
			bstrVal.Empty();
			strDay.Insert(6,L'-');
			strDay.Insert(4,L'-');
			CComVariant varVal(strDay);
			varVal.ChangeType(VT_DATE);
			spiRecordIndex->GetValueString(nIndex,5,&bstrVal);
			CString strVol = bstrVal.m_str;
			bstrVal.Empty();
			spiRecordIndex->GetValueString(nIndex,6,&bstrVal);
			CString strCaptial = bstrVal.m_str;
			bstrVal.Empty();

		}
		spiRecordIndex = NULL;
		if(!nStart)
			break;
		if(nStart >= nMaxGet)
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
		else
			nStart = 0;
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedCodes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nStockCount = 0;
	/// ����Ϻ��г����������
	m_spiMarket[0]->GetTotalCount(nCurConnID,EXCHANGETYPE_SH,&nStockCount);
	if(!nStockCount)
	{
		CComBSTR bstrErrInfo;
		m_spiMarket[0]->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	CComBSTR bstrCode,bstrName;
	USHORT nStart = 0;
	while(nStockCount)
	{
		USHORT nRetCount = 0;
		/// ����Ϻ��г���������Ƶ�
		m_spiMarket[0]->GetList(nCurConnID,EXCHANGETYPE_SH,nStart,&nRetCount,&spiRecordIndex);
		nStart += nRetCount;
		if(NULL != spiRecordIndex)
		{
			FLOAT ftPrice = 0.0;
			LONG nSingleCount = 0;
			VARIANT_BOOL bExistFlag = VARIANT_FALSE;
			HRESULT hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
			hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
			for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
			{
				spiRecordIndex->GetValueString(nIndex,0,&bstrCode);
				spiRecordIndex->GetValueString(nIndex,2,&bstrName);
				spiRecordIndex->GetValueInt(nIndex,1,&nSingleCount);
				spiRecordIndex->GetValueFloat(nIndex,5,&ftPrice);

				ftPrice = 0.0;
				bstrName.Empty();
				bstrCode.Empty();
			}
			spiRecordIndex = NULL;
		}
		if(!nRetCount || nStart >= nStockCount)
			break;/// ��ȡ���
		::Sleep(100);
	}
	nStart = 0;
	nStockCount = 0;
	/// ��������г���������Ƶ�
	m_spiMarket[0]->GetTotalCount(nCurConnID,EXCHANGETYPE_SZ,&nStockCount);
	if(!nStockCount)
	{
		CComBSTR bstrErrInfo;
		m_spiMarket[0]->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	while(nStockCount)
	{
		USHORT nRetCount = 0;
		m_spiMarket[0]->GetList(nCurConnID,EXCHANGETYPE_SZ,nStart,&nRetCount,&spiRecordIndex);
		nStart += nRetCount;
		if(NULL != spiRecordIndex)
		{
			CComBSTR bstrJsonText;
			HRESULT hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
			hRet = spiRecordIndex->get_RecordCount(&nRecordCount);

			FLOAT ftPrice = 0.0;
			LONG nSingleCount = 0;
			VARIANT_BOOL bExistFlag = VARIANT_FALSE;
			hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
			hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
			for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
			{
				spiRecordIndex->GetValueString(nIndex,0,&bstrCode);
				spiRecordIndex->GetValueString(nIndex,2,&bstrName);
				spiRecordIndex->GetValueInt(nIndex,1,&nSingleCount);
				spiRecordIndex->GetValueFloat(nIndex,5,&ftPrice);

				ftPrice = 0.0;
				bstrName.Empty();
				bstrCode.Empty();
			}
			spiRecordIndex = NULL;
		}
		if(!nRetCount || nStart >= nStockCount)
			break;/// ��ȡ���
		::Sleep(100);
	}
	return 0;
}

LRESULT CMarketDlg::OnBnClickedXdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ��Ȩ��Ϣ
	m_spiMarket[0]->GetXdxRInfo(nCurConnID,bstrGetCode,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		CComBSTR bstrText;
		spiRecordIndex->GetJsonString(&bstrText);
		CString strText = bstrText.m_str;
		bstrText.Empty();
	}
	else
	{
		CComBSTR bstrErrInfo;
		m_spiMarket[0]->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	spiRecordIndex = NULL;
	return 0;
}

LRESULT CMarketDlg::OnBnClickedF10(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	m_spiMarket[0]->GetCompanyInfoIndex(nCurConnID,bstrGetCode,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"��ȡָ����Ʊ��˾��Ϣ����ʧ�ܣ�");
		return 0;
	}
	ULONG nIndex = 0;
	HRESULT hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
	hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		CComVariant varVal1,varVal2,varVal3,varVal4;
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			switch(n)
			{
			case 0:
				/// ���
				hRet = spiRecordIndex->GetValue(nIndex,n,&varVal1);
				break;
			case 1:
				hRet = spiRecordIndex->GetValue(nIndex,n,&varVal2);
				break;
			case 2:
				hRet = spiRecordIndex->GetValue(nIndex,n,&varVal3);
				break;
			case 3:
				hRet = spiRecordIndex->GetValue(nIndex,n,&varVal4);
				break;
			}
		}
		varVal3.ChangeType(VT_UI2);
		varVal4.ChangeType(VT_UI2);
		/// ����������Ŀ����
		ITradeRecordPtr spiRecordContent = NULL;
		m_spiMarket[0]->GetCompanyInfoContent(nCurConnID,bstrGetCode,varVal2.bstrVal,\
			varVal3.uiVal,varVal4.uiVal,&spiRecordContent);
		if(NULL == spiRecordContent)
		{
			continue;
		}
		ULONG nChildFieldCount = 0,nChildRecordCount = 0,nIndex = 0;
		hRet = spiRecordContent->get_FieldCount(&nChildFieldCount);
		hRet = spiRecordContent->get_RecordCount(&nChildRecordCount);
		for(ULONG nChildIndex = 0;nChildIndex < nChildRecordCount;nChildIndex++)
		{
			for(ULONG j = 0;j < nChildFieldCount;j++)
			{
				/// ��ȡÿ���ֶ�ֵ
				CComVariant varVal;
				hRet = spiRecordContent->GetValue(nChildIndex,j,&varVal);
				varVal.Clear();
			}
		}
		spiRecordContent->Clear();
		spiRecordContent = NULL;

		/// �������ٻ�ȡ����������������ж�
		::Sleep(200);
	}
	spiRecordIndex->Clear();
	spiRecordIndex = NULL;
	return 0;
}

LRESULT CMarketDlg::OnBnClickedMk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///���ɻ�ȡ�ķ�������
	LONG nStart = 5600;
	HRESULT hRet(E_FAIL);

	while(nStart >= 0)
	{
		/// ����ȡ800��¼
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		/// �����ǹ�Ʊ����ָ���ֱ���ò�ͬ�ķ���
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		CComVariant varVal;
		CComBSTR	bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValue(nIndex,0,&varVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftOpen);
			spiRecordIndex->GetValueFloat(nIndex,2,&ftClose);
			spiRecordIndex->GetValueFloat(nIndex,3,&ftHigh);
			spiRecordIndex->GetValueFloat(nIndex,4,&ftLow);
			varVal.ChangeType(VT_DATE);
			spiRecordIndex->GetValueString(nIndex,5,&bstrVal);
			CString strVol = bstrVal.m_str;
			bstrVal.Empty();
			spiRecordIndex->GetValueString(nIndex,6,&bstrVal);
			CString strCaptial = bstrVal.m_str;
			bstrVal.Empty();

		}
		spiRecordIndex = NULL;
		if(!nStart)
			break;
		if(nStart >= nMaxGet)
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
		else
			nStart = 0;
	}

	return 1;
}

LRESULT CMarketDlg::OnBnClickedHmk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}

	return 0;
}

LRESULT CMarketDlg::OnBnClickedWk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ���ʷK��
	USHORT nMaxGet = 800;
	LONG nStart = 800;

	while(nStart >= 0)
	{
		/// ����ȡ800��¼
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1WEEK,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1WEEK,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		CComBSTR bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValueString(nIndex,0,&bstrVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftOpen);
			spiRecordIndex->GetValueFloat(nIndex,2,&ftClose);
			spiRecordIndex->GetValueFloat(nIndex,3,&ftHigh);
			spiRecordIndex->GetValueFloat(nIndex,4,&ftLow);
			CString strDay(bstrVal.m_str);
			bstrVal.Empty();
			strDay.Insert(6,L'-');
			strDay.Insert(4,L'-');
			CComVariant varVal(strDay);
			varVal.ChangeType(VT_DATE);
			spiRecordIndex->GetValueString(nIndex,5,&bstrVal);
			CString strVol = bstrVal.m_str;
			bstrVal.Empty();
			spiRecordIndex->GetValueString(nIndex,6,&bstrVal);
			CString strCaptial = bstrVal.m_str;
			bstrVal.Empty();

		}
		spiRecordIndex = NULL;
		if(!nStart)
			break;
		if(nStart >= nMaxGet)
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
		else
			nStart = 0;
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedFst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	COleDateTime curTime = COleDateTime::GetCurrentTime();
	COleDateTime BeginTime(curTime.GetYear(),curTime.GetMonth(),curTime.GetDay(),9,29,0);
	COleDateTime EndTime(curTime.GetYear(),curTime.GetMonth(),curTime.GetDay(),15,01,0);

	hRet = m_spiMarket[0]->GetMinuteTimeData(nCurConnID,bstrGetCode,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"��ȡ��ʱ����ʧ�ܣ�");
		return 0;
	}
	hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
	hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
	if(!nRecordCount)
	{
		spiRecordIndex = NULL;
		this->MessageBox(L"��ȡ��ʱ�����޼�¼��");
		return 0;
	}
	CComBSTR bstrVal;
	LONG nKeep = 0;
	FLOAT ftPrice = 0.0;
	for(LONG nIndex = 0;nIndex < (LONG)nRecordCount ;nIndex++)
	{
		LONG nMintue = nIndex+1;
		LONG nHoure = (LONG)(nMintue/60);
		nMintue = nMintue%60;
		COleDateTime TTime = BeginTime + COleDateTimeSpan(0,nHoure,nMintue,0);
		spiRecordIndex->GetValueFloat(nIndex,0,&ftPrice);
		spiRecordIndex->GetValueInt(nIndex,2,&nKeep);
		spiRecordIndex->GetValueString(nIndex,1,&bstrVal);
		CString strVol = bstrVal.m_str;
		bstrVal.Empty();

	}
	spiRecordIndex = NULL;
	return 1;
}

LRESULT CMarketDlg::OnBnClickedHmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	SHORT nGetDay = 30;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ָ��һ������
	hRet = m_spiMarket[0]->GetHisMinuteTimeData(nCurConnID,bstrGetCode,20170930,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"��ȡ��ʱ����ʧ�ܣ�");
		return 0;
	}
	hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
	hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
	if(!nRecordCount)
	{
		spiRecordIndex = NULL;
		this->MessageBox(L"��ȡ��ʱ�����޼�¼��");
		return 0;
	}
	CComBSTR bstrVal;
	LONG nKeep = 0;
	FLOAT ftPrice = 0.0;
	for(LONG nIndex = 0;nIndex < (LONG)nRecordCount ;nIndex++)
	{
		spiRecordIndex->GetValueFloat(nIndex,0,&ftPrice);
		spiRecordIndex->GetValueInt(nIndex,2,&nKeep);
		spiRecordIndex->GetValueString(nIndex,1,&bstrVal);
		CString strVol = bstrVal.m_str;
		bstrVal.Empty();
	}

	spiRecordIndex = NULL;
	return 1;
}

LRESULT CMarketDlg::OnBnClickedFscj(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	LONG nStart = 0;
	USHORT nMaxGet = 2000;
	while(TRUE)
	{
		hRet = m_spiMarket[0]->GetTransData(nCurConnID,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			break;
		}
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			spiRecordIndex = NULL;
			break;
		}
		CComBSTR bstrVal;
		FLOAT ftPrice = 0.0;
		LONG nVol = 0,nHand = 0,nTrade = 0;
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValueString(nIndex,0,&bstrVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftPrice);
			spiRecordIndex->GetValueInt(nIndex,2,&nVol);
			spiRecordIndex->GetValueInt(nIndex,3,&nHand);
			spiRecordIndex->GetValueInt(nIndex,4,&nTrade);
		}
		spiRecordIndex = NULL;
		nStart += nMaxGet;/// ��ȡ��һ��800����¼
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedHfst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	SHORT nGetDay = 30;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 2000;
	LONG nStart = 0;
	while(TRUE)
	{
		/// ָ��һ������
		hRet = m_spiMarket[0]->GetHisTransData(nCurConnID,bstrGetCode,20170930,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			break;
		}
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			spiRecordIndex = NULL;
			break;
		}
		CComBSTR bstrVal;
		FLOAT ftPrice = 0.0;
		LONG nVol = 0,nHand = 0,nTrade = 0;
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValueString(nIndex,0,&bstrVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftPrice);
			spiRecordIndex->GetValueInt(nIndex,2,&nVol);
			spiRecordIndex->GetValueInt(nIndex,3,&nHand);
			spiRecordIndex->GetValueInt(nIndex,4,&nTrade);
		}
		spiRecordIndex = NULL;
		nStart += nMaxGet;/// ��ȡ��һ��800����¼
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedHkd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///���ɻ�ȡ��СʱK������
	LONG nStart = 1600;
	HRESULT hRet(E_FAIL);

	while(nStart >= 0)
	{
		/// ����ȡ800��¼���ж��ǻ�ȡ��Ʊ�Ļ���ָ����
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1HOUR,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1HOUR,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		CComVariant varVal;
		CComBSTR	bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValue(nIndex,0,&varVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftOpen);
			spiRecordIndex->GetValueFloat(nIndex,2,&ftClose);
			spiRecordIndex->GetValueFloat(nIndex,3,&ftHigh);
			spiRecordIndex->GetValueFloat(nIndex,4,&ftLow);
			varVal.ChangeType(VT_DATE);
			spiRecordIndex->GetValueString(nIndex,5,&bstrVal);
			CString strVol = bstrVal.m_str;
			bstrVal.Empty();
			spiRecordIndex->GetValueString(nIndex,6,&bstrVal);
			CString strCaptial = bstrVal.m_str;
			bstrVal.Empty();

		}
		spiRecordIndex = NULL;
		if(!nStart)
			break;
		if(nStart >= nMaxGet)
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
		else
			nStart = 0;
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedFivemk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///���ɻ�ȡ��5��������
	LONG nStart = 23200;

	HRESULT hRet(E_FAIL);
	while(nStart >= 0)
	{
		/// ����ȡ800��¼
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_5MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_5MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		CComVariant varVal;
		CComBSTR	bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValue(nIndex,0,&varVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftOpen);
			spiRecordIndex->GetValueFloat(nIndex,2,&ftClose);
			spiRecordIndex->GetValueFloat(nIndex,3,&ftHigh);
			spiRecordIndex->GetValueFloat(nIndex,4,&ftLow);
			varVal.ChangeType(VT_DATE);
			spiRecordIndex->GetValueString(nIndex,5,&bstrVal);
			CString strVol = bstrVal.m_str;
			bstrVal.Empty();
			spiRecordIndex->GetValueString(nIndex,6,&bstrVal);
			CString strCaptial = bstrVal.m_str;
			bstrVal.Empty();
		}
		spiRecordIndex = NULL;
		if(!nStart)
			break;
		if(nStart >= nMaxGet)
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
		else
			nStart = 0;
	}

	return 1;
}

LRESULT CMarketDlg::OnBnClickedMkx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	ULONG nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ���ʷK��
	USHORT nMaxGet = 800;
	LONG nStart = 0;

	while(nStart >= 0)
	{
		/// ����ȡ800��¼
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1MONTH,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1MONTH,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		CComBSTR bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
			spiRecordIndex = NULL;
			continue;
		}
		for(ULONG nIndex = 0;nIndex < nRecordCount ;nIndex++)
		{
			spiRecordIndex->GetValueString(nIndex,0,&bstrVal);
			spiRecordIndex->GetValueFloat(nIndex,1,&ftOpen);
			spiRecordIndex->GetValueFloat(nIndex,2,&ftClose);
			spiRecordIndex->GetValueFloat(nIndex,3,&ftHigh);
			spiRecordIndex->GetValueFloat(nIndex,4,&ftLow);
			CString strDay(bstrVal.m_str);
			bstrVal.Empty();
			strDay.Insert(6,L'-');
			strDay.Insert(4,L'-');
			CComVariant varVal(strDay);
			varVal.ChangeType(VT_DATE);
			spiRecordIndex->GetValueString(nIndex,5,&bstrVal);
			CString strVol = bstrVal.m_str;
			bstrVal.Empty();
			spiRecordIndex->GetValueString(nIndex,6,&bstrVal);
			CString strCaptial = bstrVal.m_str;
			bstrVal.Empty();

		}
		spiRecordIndex = NULL;
		if(!nStart)
			break;
		if(nStart >= nMaxGet)
			nStart -= nMaxGet;/// ��ȡ��һ��800����¼
		else
			nStart = 0;
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedTotalf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"��û�г�ʼ�����ӣ�");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��û�����ӳɹ���");
		return 0;
	}
	COleDateTime GetTime = COleDateTime::GetCurrentTime()-COleDateTimeSpan(90,0,0,0);

	CComBSTR bstrVal;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	/// ������Ϣ
	m_spiMarket[0]->GetFinanceInfo(nCurConnID,bstrGetCode,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"��ȡʧ�ܣ�");
		return 0;
	}
	spiRecordIndex->get_FieldCount(&nFieldCount);
	spiRecordIndex->get_RecordCount(&nRecordCount);
	if(!nRecordCount || nFieldCount < 37)
	{
		spiRecordIndex = NULL;
		this->MessageBox(L"��ȡ�����쳣��");
		return 0;
	}
	CComVariant varVal;
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�����������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strDay(varVal.bstrVal);
	varVal.Clear();
	DATE dtFTime = 0.0;
	if(strDay.GetLength())
	{
		strDay.Insert(6,L'-');
		strDay.Insert(4,L'-');
		CComVariant varTmpVal(strDay);
		varTmpVal.ChangeType(VT_DATE);
		dtFTime = varTmpVal.date;
		varTmpVal.Clear();
	}
	strDay.Empty();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"��������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	strDay = varVal.bstrVal;
	varVal.Clear();
	DATE dtLDay = 0.0;
	if(strDay.GetLength())
	{
		strDay.Insert(6,L'-');
		strDay.Insert(4,L'-');
		CComVariant varTmpVal(strDay);
		varTmpVal.ChangeType(VT_DATE);
		dtLDay = varTmpVal.date;
		varTmpVal.Clear();
	}
	strDay.Empty();

	spiRecordIndex->GetValueByName(0,CComBSTR(L"�г�"),&varVal);
	varVal.ChangeType(VT_UI2);
	USHORT nMarket = varVal.uiVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"����ʡ��"),&varVal);
	varVal.ChangeType(VT_UI2);
	USHORT nProvince = varVal.uiVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"������ҵ"),&varVal);
	varVal.ChangeType(VT_UI2);
	USHORT nIndustry = varVal.uiVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"��ͨ�ɱ�"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nCCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"֤ȯ����"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strFCode = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�ܹɱ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTCount = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���ҹ�"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nSCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�����˷��˹�"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nICount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���˹�"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nLCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"B��"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nBCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"H��"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nHCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"ְ����"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strECount = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���ʲ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�����ʲ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strCCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�̶��ʲ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strFCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�����ʲ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strICapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�ʱ�������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strRCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�ɶ�����"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nSNumber = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"������ծ"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strCLiability = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���ڸ�ծ"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strLLiability = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���ʲ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strNetAssets = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"��Ӫ����"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strMainSale = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"��Ӫ����"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strMainProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"Ӧ���ʿ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strReceivables = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"Ӫҵ����"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTradeProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"Ͷ������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strInvestIn = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"��Ӫ�ֽ���"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strCashFlow = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���ֽ���"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTCash = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"���"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strInventory = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"�����ܶ�"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTotalProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"˰������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strATProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strNetProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"δ������"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strUProfit = varVal.bstrVal;
	varVal.Clear();

	return 1;
}

LRESULT CMarketDlg::OnBnClickedQuote(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	CString strStockCode(bstrStockCode.m_str);
	ULONG nReqID = 0;
	ULONG nConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nConnID);
	m_spiMarket[0]->GetQuotes(nConnID,bstrStockCode,&nReqID);

	return 0;
}
