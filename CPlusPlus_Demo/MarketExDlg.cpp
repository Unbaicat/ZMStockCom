// MarketExDlg.cpp : implementation of the CMarketExDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MarketExDlg.h"
#include <atlcomtime.h>

extern LONG StrToNum(const ATL::CString& strTemp);

HRESULT CMarketExDlg::AdviseMarketCom(int nIndex)
{
	if(NULL != m_spiMarketEvent[nIndex])
		return S_FALSE;
	m_nMarketEventCookie[nIndex] = NULL;
	this->m_spiMarketEvent[nIndex] = new CComObject <CMarketExEvent>;
	ATLASSERT(this->m_spiMarketEvent[nIndex]);
	if(NULL == this->m_spiMarketEvent[nIndex])
		return E_POINTER;
	/// ���ûص������������¼�����ʱƥ����˭������
	m_spiMarketEvent[nIndex]->SetCallPara(this->m_hWnd,nIndex);
	//�����¼�����
	HRESULT hRet = AtlAdvise(m_spiMarket[nIndex],this->m_spiMarketEvent[nIndex]->GetUnknown(),
		DIID__IMarketExEvents,&this->m_nMarketEventCookie[nIndex]);
	ATLASSERT(SUCCEEDED(hRet));
	return hRet;
}

void CMarketExDlg::UnAdviseMarketCom(int nIndex)
{
	if(m_nMarketEventCookie[nIndex] && NULL != m_spiMarket[nIndex])
	{
		AtlUnadvise(m_spiMarket[nIndex],DIID__IMarketExEvents,m_nMarketEventCookie[nIndex]);
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

LRESULT CMarketExDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

	m_spiMarket[1] = NULL;
	m_nMarketEventCookie[1] = 0;
	m_spiMarketEvent[1] = NULL;

	this->GetDlgItem(IDC_EDIT_CODE).SetWindowText(L"10001665");

	/// ��Ʊ��Ȩ������
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"114.80.63.20");
	/// ������ͨ��վZ1
//	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"202.108.253.154");
	/// �Ϻ�������վZ1
//	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"180.153.18.176");
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).SetWindowText(L"7721");

	m_wndMarketType.Attach(GetDlgItem(IDC_COMBO_MARKETTYPE).m_hWnd);
	int nItemIndex = m_wndMarketType.AddString(L"�н�����ָ�ڻ�");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_CFFE);
	nItemIndex = m_wndMarketType.AddString(L"֣����Ʒ");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_ZZF);
	nItemIndex = m_wndMarketType.AddString(L"������Ʒ");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_DLF);
	nItemIndex = m_wndMarketType.AddString(L"�����ڻ���Լ");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_MAINFC);
	nItemIndex = m_wndMarketType.AddString(L"�Ϻ��ڻ�");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_SHF);
	nItemIndex = m_wndMarketType.AddString(L"�Ϻ���Ʊ��Ȩ");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_SHSO);
	m_wndMarketType.SetCurSel(5);

	return TRUE;
}

LRESULT CMarketExDlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam > 1)
		return 0;
	IMarketExPtr spiMarket = m_spiMarketEvent[lParam]->GetMarket();
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
			this->MessageBox(bstrErrDesc.m_str);
			bstrErrDesc.Empty();
			this->UnAdviseMarketCom(0);
			return 0;
		}
		this->MessageBox(L"��չ�������ӷ������ɹ���");
#endif

#ifdef _DEBUG
		if(m_spiMarket[0] == spiMarket)
		{
#ifdef ZM_MUTI_ACCOUNT
			/// ���˺�ʹ�ã��������COM�������
			HRESULT hRet = m_spiMarket[1].CreateInstance(__uuidof(MarketEx));
			ATLASSERT(m_spiMarket[1]);
			if(NULL != m_spiMarket[1])
			{
				/// �����¼�����
				AdviseMarketCom(1);

				/// ���õ�����־���
				hRet = m_spiMarket[1]->put_EnableLog(VARIANT_TRUE);
				/// ��ʼ��ʹ�û���������ʵ��ֻ��Ҫ����һ�Σ����ݿͻ��˰汾�ţ����������(Ĭ��Ϊ1��ֻ�и߼����֧�ֶ������)
				/// �첽��������Ҫ�����¼�֪ͨ�õ���ʼ�����
#ifdef ZM_TDXSERVER_DEMO
				m_spiMarket[1]->Init(1);
#else
#ifndef ZM_VRSION_ADVANCED
				m_spiMarket[1]->Init(1);
#else
				m_spiMarket[1]->Init(1);///������������������ö���������������������ֻ�ü���������
#endif
#endif
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
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMarketExDlg::OnConnReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam > 1)
		return 0;
	IMarketExPtr spiMarket = m_spiMarketEvent[lParam]->GetMarket();
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
	}
	else
	{
		/// ����ʧ�ܣ�������������
		CComBSTR bstrErrInfo;
		spiMarket->get_LastErrDesc(&bstrErrInfo);
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	spiMarket = NULL;
	return 0;
}

LRESULT CMarketExDlg::OnServerError(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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

LRESULT CMarketExDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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

LRESULT CMarketExDlg::OnStockMintue(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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

LRESULT CMarketExDlg::OnStockTrans(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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

LRESULT CMarketExDlg::OnStockBars(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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

LRESULT CMarketExDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	ReleaseCom();

	EndDialog(wID);
	return 0;
}

LRESULT CMarketExDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReleaseCom();
	EndDialog(wID);
	return 0;
}

void CMarketExDlg::ReleaseCom()
{
	/// �ͷ�COM�������
	if(NULL != m_spiMarket[0])
	{
		/// �ͷ���������
		this->UnAdviseMarketCom(0);
	}
#ifdef ZM_MUTI_ACCOUNT
	if(NULL != m_spiMarket[1])
	{
		/// �Ͽ��¼�����
		UnAdviseMarketCom(1);
	}
#endif
}

LRESULT CMarketExDlg::OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiMarket[0])
		return 0;
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	this->UnAdviseMarketCom(0);
	if(!nCurConnID)
		this->MessageBox(L"δ���ӣ������ͷţ�");
	else
		this->MessageBox(L"�ͷ����ӳɹ���");
	return 0;
}

LRESULT CMarketExDlg::OnBnClickedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiMarket[0])
	{
		ULONG nConnID = 0;
		m_spiMarket[0]->get_CurConnID(&nConnID);
		if(nConnID)
			return 0;/// ��������
		this->UnAdviseMarketCom(0);
	}
	/// ��ȡ��ʼ������
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).GetWindowText(&bstrServerPort);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());

	HRESULT hRet = m_spiMarket[0].CreateInstance(__uuidof(MarketEx));
	ATLASSERT(m_spiMarket[0]);
	if(NULL == m_spiMarket)
	{
		this->MessageBox(L"������չ����ӿ�ʧ�ܣ������Ƿ��Ѿ��ɹ�ע��COM�����");
		return 0;
	}

	/// ���õ�����־���
	hRet = m_spiMarket[0]->put_EnableLog(VARIANT_TRUE);
	/// ���������������ַ
	m_spiMarket[0]->put_CurServerHost(bstrServerAddr);
	/// ��������������˿�
	m_spiMarket[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	this->AdviseMarketCom(0);
	/// ��ʼ��
	m_spiMarket[0]->Init(1);
	return 1;
}

LRESULT CMarketExDlg::OnBnClickedCodes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	CComBSTR bstrTitle;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// ��ȡ֧�ֵĽ����г��б�
	m_spiMarket[0]->GetMarkets(nCurConnID,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		spiRecordIndex->GetTitleJson(&bstrTitle);
		spiRecordIndex->get_RecordCount(&nRecordCount);
		for(ULONG nIndex = 0;nIndex < nRecordCount;nIndex++)
		{
			CComBSTR bstrChildJson;
			spiRecordIndex->GetChildJson(nIndex,&bstrChildJson);
#ifdef _DEBUG
//			this->MessageBox(bstrChildJson.m_str);
#endif
			bstrChildJson.Empty();
		}
		spiRecordIndex = NULL;
	}
	USHORT nCount = 0;
	m_spiMarket[0]->GetBreedCount(nCurConnID,&nCount);
	USHORT nRetCount = 511,nStart = 0;
	while(TRUE)
	{
		m_spiMarket[0]->GetBreedInfo(nCurConnID,nStart,nRetCount,&spiRecordIndex);
		if(NULL == spiRecordIndex)
			break;
		bstrTitle.Empty();
		spiRecordIndex->GetTitleJson(&bstrTitle);
		spiRecordIndex->get_RecordCount(&nRecordCount);
		if(nRecordCount < 2)
			break;
		nStart += (USHORT)nRecordCount;
		for(ULONG nIndex = 0;nIndex < nRecordCount;nIndex++)
		{
			CComBSTR bstrChildJson;
			spiRecordIndex->GetChildJson(nIndex,&bstrChildJson);
#ifdef _DEBUG
//			this->MessageBox(bstrChildJson.m_str);
#endif
			bstrChildJson.Empty();
		}
		spiRecordIndex = NULL;
		::Sleep(100);
	}
	return 1;
}

LRESULT CMarketExDlg::OnBnClickedKdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"���벻�Ϸ���");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ULONG nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ�н���IF 17��9�º�Լ��K������
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1DAY,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;
	/// ������鱨��
	m_spiMarket[0]->GetBreedQuote(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();
	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedWk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ֣�ݽ������޻�17��9�º�Լ��K������
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1WEEK,(EZMMarketType )dwType,CComBSTR(L"CF1801"),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;
	return 1;
}

LRESULT CMarketExDlg::OnBnClickedMkx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ�н���IF 17��9�º�Լ��K������
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1MONTH,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedHkd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ�н���IF 17��9�º�ԼСʱK������
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1HOUR,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedFivemk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ�н���IF 17��9�º�ԼСʱK������
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_5MINUTE,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedMk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ�н���IF 17��9�º�ԼСʱK������
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1MINUTE,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedFst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// ��ȡ�н���IF 17��9�º�Լ��ʱͼ����
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetMinuteTimeData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedHmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	COleDateTime BeginTime = COleDateTime::GetCurrentTime();
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	/// ��ȡ�н���IF��Լ��ʱͼ����
	/// ָ��һ������
	hRet = m_spiMarket[0]->GetHisMinuteTimeData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),20170930,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedFscj(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	/// ��ȡ�н���IF��Լ��ʱ�ɽ�����
	hRet = m_spiMarket[0]->GetTransData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedHfst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
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
		this->MessageBox(L"��δ�ɹ����ӣ�");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	USHORT nCount = 800;///��������ȡ800����¼
	HRESULT hRet(E_FAIL);
	/// ��ȡ�н�����Լ��ʱ�ɽ�����
	/// ָ��һ������
	hRet = m_spiMarket[0]->GetHisTransData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),20170930,0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"��ȡ����Ʒ�ִ������");
		return 0;
	}
	CComBSTR bstrName;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	spiRecord->GetJsonString(&bstrName);
	this->MessageBox(bstrName.m_str);
	bstrName.Empty();

	spiRecord->Clear();
	spiRecord = NULL;

	return 0;
}

LRESULT CMarketExDlg::OnBnClickedQuote(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	CString strStockCode(bstrStockCode.m_str);
	ULONG nConnID = 0;
	ULONG nReqID = 0;
	m_spiMarket[0]->get_CurConnID(&nConnID);
	ITradeRecordPtr spiRecord = NULL;
	m_spiMarket[0]->GetBreedQuote(nConnID,(EZMMarketType )dwType,bstrStockCode,&spiRecord);
	if(NULL == spiRecord)
	{
		CComBSTR bstrErr;
		m_spiMarket[0]->get_LastErrDesc(&bstrErr);
		this->MessageBox(bstrErr.m_str);
		bstrErr.Empty();
		return 0;
	}
	CComBSTR bstrJsonInfo;
	spiRecord->GetJsonString(&bstrJsonInfo);
	this->MessageBox(bstrJsonInfo.m_str);
	bstrJsonInfo.Empty();
	spiRecord = NULL;
	return 0;
}
