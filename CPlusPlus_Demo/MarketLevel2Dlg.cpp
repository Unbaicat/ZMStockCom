// MarketLevel2Dlg.cpp : implementation of the CMarketLevel2Dlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MarketLevel2Dlg.h"
#include <atlcomtime.h>

extern LONG StrToNum(const ATL::CString& strTemp);

HRESULT CMarketLevel2Dlg::AdviseMarketCom()
{
	if(NULL != m_spiMarketEvent)
		return S_FALSE;
	m_nMarketEventCookie = NULL;
	this->m_spiMarketEvent = new CComObject <CMarketL2Event>;
	ATLASSERT(this->m_spiMarketEvent);
	if(NULL == this->m_spiMarketEvent)
		return E_POINTER;
	/// 设置回调参数，用于事件发生时匹配是谁侦听的
	m_spiMarketEvent->SetCallPara(this->m_hWnd);
	//建立事件连接
	HRESULT hRet = AtlAdvise(m_spiMarket,this->m_spiMarketEvent->GetUnknown(),
		DIID__IMarketLevel2Events,&this->m_nMarketEventCookie);
	ATLASSERT(SUCCEEDED(hRet));
	return hRet;
}

void CMarketLevel2Dlg::UnAdviseMarketCom()
{
	if(m_nMarketEventCookie && NULL != m_spiMarket)
	{
		AtlUnadvise(m_spiMarket,DIID__IMarketLevel2Events,m_nMarketEventCookie);
		m_nMarketEventCookie = 0;
	}
	if(NULL != m_spiMarketEvent)
		m_spiMarketEvent = NULL;
	if(NULL != m_spiMarket)
	{
		VARIANT_BOOL bConnected = VARIANT_FALSE;
		m_spiMarket->get_IsConnected(&bConnected);
		if(VARIANT_TRUE	== bConnected)
			m_spiMarket->Disconnect();
		m_spiMarket = NULL;
	}
}

LRESULT CMarketLevel2Dlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	m_bSort =  FALSE;

	m_spiMarket = NULL;
	m_nMarketEventCookie = 0;
	m_spiMarketEvent = NULL;

	this->GetDlgItem(IDC_EDIT_CODE).SetWindowText(L"000001");

	this->GetDlgItem(IDC_EDIT_LEVEL2SERVERPORT).SetWindowText(L"7727");
	/// 市场上海主站1
	this->GetDlgItem(IDC_EDIT_LEVEL2SERVER).SetWindowText(L"61.152.107.141");
	/// 市场上海主站2
//	this->GetDlgItem(IDC_EDIT_LEVEL2SERVERPORT).SetWindowText(L"61.152.107.171");

	DoDataExchange(false,IDC_CHECK_SORT);
	return TRUE;
}

LRESULT CMarketLevel2Dlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_spiMarketEvent != (CMarketL2Event *)lParam)
		return 0;/// 错误行情接口
	if(wParam)
	{
		/// 连接服务器
		VARIANT_BOOL bRet = VARIANT_FALSE;
#ifndef _DEBUG
#ifdef ZM_TDXSERVER_SYNC
		HRESULT hRet = m_spiMarket->Connect(VARIANT_FALSE,&bRet);
#else
		HRESULT hRet = m_spiMarket->Connect(VARIANT_TRUE,&bRet);
#endif
#else
		HRESULT hRet = m_spiMarket->Connect(VARIANT_TRUE,&bRet);
#endif
		if(VARIANT_FALSE == bRet)
		{
			/// 获取错误信息
			USHORT nErrCode = 0;
			CComBSTR bstrErrDesc;
			m_spiMarket->get_LastErrType(&nErrCode);
			m_spiMarket->get_LastErrDesc(&bstrErrDesc);
			this->MessageBox(bstrErrDesc.m_str);
			this->UnAdviseMarketCom();
			bstrErrDesc.Empty();
			return 0;
		}
#ifndef _DEBUG
#ifdef ZM_TDXSERVER_SYNC
		VARIANT_BOOL bConnected = VARIANT_FALSE;
		m_spiMarket->get_IsConnected(&bConnected);
		if(VARIANT_FALSE == bConnected)
		{
			USHORT nErrCode = 0;
			CComBSTR bstrErrDesc;
			m_spiMarket->get_LastErrType(&nErrCode);
			m_spiMarket->get_LastErrDesc(&bstrErrDesc);
			this->MessageBox(bstrErrDesc.m_str);
			bstrErrDesc.Empty();
			this->UnAdviseMarketCom();
			return 0;
		}
		this->MessageBox(L"L2行情连接服务器成功！");
#endif
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMarketLevel2Dlg::OnLoginReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_spiMarketEvent != (CMarketL2Event *)lParam)
		return 0;/// 错误行情接口
	if(wParam)
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		HRESULT hRet = m_spiMarket->get_IsConnected(&bConnectValid);
		if(VARIANT_TRUE == bConnectValid)
			this->MessageBox(L"异步登录L2行情服务器成功！");
	}
	else
	{
		/// 连接失败，弹出错误描述
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnServerError(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarket)
		return 0;
	/// 服务器产生错误，需要重新登录使用，先断开连接
	m_spiMarket->Disconnect();
	return 0;
}

LRESULT CMarketLevel2Dlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnStockMintue(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnStockTrans(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnStockBars(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnDetailTrans(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnDetailOrder(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnOrderList(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnStockQuote10(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiMarketEvent)
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent->GetReqData();
	if(NULL == spiRecord)
		return 0;
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType;
		spiRecord->GetDataType(nIndex,&eDataType);
		/// 输出返回记录的字段名称
		hRet = spiRecord->GetFieldName(nIndex,&bstrName);
		bstrName.Empty();
	}
	///  输出所有记录
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		for(ULONG n = 0;n < nFieldCount;n++)
		{
			/// 获取每个字段值
			CComVariant varVal;
			hRet = spiRecord->GetValue(nIndex,n,&varVal);
			varVal.Clear();
		}
	}
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	ReleaseCom();

	EndDialog(wID);
	return 0;
}

LRESULT CMarketLevel2Dlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReleaseCom();

	EndDialog(wID);
	return 0;
}

void CMarketLevel2Dlg::ReleaseCom()
{
	/// 释放COM组件对象
	if(NULL == m_spiMarket)
		return;
	this->UnAdviseMarketCom();
}

LRESULT CMarketLevel2Dlg::OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnectValid = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnectValid);
	this->UnAdviseMarketCom();

	if(VARIANT_FALSE == bConnectValid)
		this->MessageBox(L"未连接，无需释放！");
	else
		this->MessageBox(L"释放连接成功！");
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiMarket)
	{
		VARIANT_BOOL bConnected = VARIANT_FALSE;
		m_spiMarket->get_IsConnected(&bConnected);
		if(VARIANT_TRUE == bConnected)
			return 0;
		this->UnAdviseMarketCom();
	}
	/// 获取初始化参数
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_LEVEL2SERVER).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_LEVEL2SERVERPORT).GetWindowText(&bstrServerPort);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());

	HRESULT hRet = m_spiMarket.CreateInstance(__uuidof(MarketLevel2));
	ATLASSERT(m_spiMarket);
	if(NULL == m_spiMarket)
	{
		this->MessageBox(L"创建L2行情接口失败，请检查是否已经成功注册COM组件！");
		return 0;
	}
	/// 启用调试日志输出
	hRet = m_spiMarket->put_EnableLog(VARIANT_TRUE);
	/// 设置行情服务器地址
	m_spiMarket->put_CurServerHost(bstrServerAddr);
	/// 设置行情服务器端口
	m_spiMarket->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	this->AdviseMarketCom();
	ATLASSERT(m_spiMarket);
	if(NULL == m_spiMarket)
		return 0;
	/// 必须设置L2登录账号和密码
	CComBSTR bstrLoginID,bstrLoginPW;
	this->GetDlgItem(IDC_EDIT_LOGINID).GetWindowText(&bstrLoginID);
	this->GetDlgItem(IDC_EDIT_LOGINPW).GetWindowText(&bstrLoginPW);
	ATLASSERT(bstrLoginID.Length());
	ATLASSERT(bstrLoginPW.Length());
	m_spiMarket->put_Level2UserName(bstrLoginID);
	m_spiMarket->put_Level2UserPW(bstrLoginPW);
	/// 初始化使用参数，最大线程数
	m_spiMarket->Init(1);
	return 1;
}

LRESULT CMarketLevel2Dlg::OnBnClickedF10(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	m_spiMarket->GetCompanyInfoIndex(bstrStockCode,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"获取指定股票公司信息索引失败！");
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
				/// 类别
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
		/// 遍历所有项目内容
		ITradeRecordPtr spiRecordContent = NULL;
		m_spiMarket->GetCompanyInfoContent(bstrStockCode,varVal2.bstrVal,\
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
				/// 获取每个字段值
				CComVariant varVal;
				hRet = spiRecordContent->GetValue(nChildIndex,j,&varVal);
				varVal.Clear();
			}
		}
		spiRecordContent->Clear();
		spiRecordContent = NULL;

		/// 休眠下再获取，避免服务器连接中断
		::Sleep(200);
	}
	spiRecordIndex->Clear();
	spiRecordIndex = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedGetf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 财务信息
	m_spiMarket->GetFinanceInfo(bstrStockCode,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		CComBSTR bstrText;
		spiRecordIndex->GetJsonString(&bstrText);
		CString strText = bstrText.m_str;
		bstrText.Empty();
		this->MessageBox(strText);
		strText.Empty();
	}
	else
	{
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	spiRecordIndex = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedXdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 除权信息
	m_spiMarket->GetXdxRInfo(bstrStockCode,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		CComBSTR bstrText;
		spiRecordIndex->GetJsonString(&bstrText);
		this->MessageBox(bstrText.m_str);
		bstrText.Empty();
	}
	else
	{
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	spiRecordIndex = NULL;

	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedKdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 最长历史K线
	USHORT nMaxGet = 800;
	LONG nStart = 5600;

	COleDateTime curTime = COleDateTime::GetCurrentTime();
	COleDateTime MarketDay = curTime;
	/// 通过获取财务数据的上市日期来获取日K线
	m_spiMarket->GetFinanceInfo(bstrStockCode,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		ULONG nCount = 0;
		spiRecordIndex->get_RecordCount(&nCount);
		ATLASSERT(nCount);
		if(nCount)
		{
			CString strMarketDay;
			CComVariant varVal;
			spiRecordIndex->GetValueByName(0,CComBSTR(L"上市日期"),&varVal);
			varVal.ChangeType(VT_BSTR);
			strMarketDay = varVal.bstrVal;
			varVal.Clear();
			if(strMarketDay.GetLength() > 5)
			{
				strMarketDay.Insert(6,L'-');
				strMarketDay.Insert(4,L'-');
				CComVariant varDay(strMarketDay);
				varDay.ChangeType(VT_DATE);
				MarketDay = COleDateTime(varDay);
			}
		}
		spiRecordIndex = NULL;
	}
	COleDateTimeSpan spanTime = curTime - MarketDay;
	int nTotalDay = (int)spanTime.GetTotalDays();
	if(nTotalDay)
	{
		/// 再扣除节假日
		nStart = (int)5*nTotalDay/7;
	}

	HRESULT hRet(E_FAIL);
	while(nStart >= 0)
	{
		/// 最大获取800记录，区分股票还是指数
		hRet = m_spiMarket->GetBars(ZMBARTYPE_1DAY,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket->GetIndexBars(ZMBARTYPE_1DAY,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
			spiRecordIndex = NULL;
			continue;
		}
		CComBSTR bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
		else
			nStart = 0;
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedWk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 最长历史K线
	USHORT nMaxGet = 800;
	LONG nStart = 800;

	HRESULT hRet(E_FAIL);
	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket->GetBars(ZMBARTYPE_1WEEK,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket->GetIndexBars(ZMBARTYPE_1WEEK,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
			spiRecordIndex = NULL;
			continue;
		}
		CComBSTR bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
		else
			nStart = 0;
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedMkx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 最长历史K线
	USHORT nMaxGet = 800;
	LONG nStart = 0;

	HRESULT hRet(E_FAIL);
	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket->GetBars(ZMBARTYPE_1MONTH,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket->GetIndexBars(ZMBARTYPE_1MONTH,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
			spiRecordIndex = NULL;
			continue;
		}
		CComBSTR bstrVal;
		FLOAT ftOpen = 0.0,ftHigh = 0.0,ftLow = 0.0,ftClose = 0.0;
		hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
		hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
		if(!nRecordCount)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
		else
			nStart = 0;
	}

	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedHkd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///最多可获取的小时K线数量
	LONG nStart = 1600;

	while(nStart >= 0)
	{
		/// 最大获取800记录
		HRESULT hRet = m_spiMarket->GetBars(ZMBARTYPE_1HOUR,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket->GetIndexBars(ZMBARTYPE_1HOUR,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
		else
			nStart = 0;
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedFivemk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///最多可获取的5分钟数量
	LONG nStart = 23200;

	while(nStart >= 0)
	{
		/// 最大获取800记录
		HRESULT hRet = m_spiMarket->GetBars(ZMBARTYPE_5MINUTE,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		hRet = m_spiMarket->GetIndexBars(ZMBARTYPE_5MINUTE,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
		else
			nStart = 0;
	}

	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedQuote(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nReqID = 0;
	m_spiMarket->GetQuotes(bstrStockCode,&nReqID);
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedMk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///最多可获取的分钟数量
	LONG nStart = 5600;

	while(nStart >= 0)
	{
		/// 最大获取800记录
		HRESULT hRet = m_spiMarket->GetBars(ZMBARTYPE_1MINUTE,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		hRet = m_spiMarket->GetIndexBars(ZMBARTYPE_1MINUTE,bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
		if(NULL == spiRecordIndex)
		{
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
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
			nStart -= nMaxGet;/// 获取下一个800条记录
		else
			nStart = 0;
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedFst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	HRESULT hRet = m_spiMarket->GetMinuteTimeData(bstrStockCode,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"获取分时数据失败！");
		return 0;
	}
	hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
	hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
	if(!nRecordCount)
	{
		spiRecordIndex = NULL;
		this->MessageBox(L"获取分时数据无记录！");
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
		spiRecordIndex->GetValueFloat(nIndex,0,&ftPrice);
		spiRecordIndex->GetValueInt(nIndex,2,&nKeep);
		spiRecordIndex->GetValueString(nIndex,1,&bstrVal);
		CString strVol = bstrVal.m_str;
		bstrVal.Empty();

	}
	spiRecordIndex = NULL;

	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedHmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	SHORT nGetDay = 30;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 指定一个日期
	HRESULT hRet = m_spiMarket->GetHisMinuteTimeData(bstrStockCode,20170930,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"获取分时数据失败！");
		return 0;
	}
	hRet = spiRecordIndex->get_FieldCount(&nFieldCount);
	hRet = spiRecordIndex->get_RecordCount(&nRecordCount);
	if(!nRecordCount)
	{
		spiRecordIndex = NULL;
		this->MessageBox(L"获取分时数据无记录！");
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
		spiRecordIndex->GetValueFloat(nIndex,0,&ftPrice);
		spiRecordIndex->GetValueInt(nIndex,2,&nKeep);
		spiRecordIndex->GetValueString(nIndex,1,&bstrVal);
		CString strVol = bstrVal.m_str;
		bstrVal.Empty();

	}

	spiRecordIndex = NULL;
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedQuote10(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nReqID = 0;
	m_spiMarket->GetQuotes10(bstrStockCode,&nReqID);
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedFscj(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	LONG nStart = 0;
	USHORT nMaxGet = 2000;
	while(TRUE)
	{
		HRESULT hRet = m_spiMarket->GetTransData(bstrStockCode,nStart,nMaxGet,&spiRecordIndex);
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
		nStart += nMaxGet;/// 获取下一个800条记录
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedHfst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	SHORT nGetDay = 30;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 2000;
	LONG nStart = 0;
	while(TRUE)
	{
		/// 指定一个日期
		HRESULT hRet = m_spiMarket->GetHisTransData(bstrStockCode,20170930,nStart,nMaxGet,&spiRecordIndex);
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
		nStart += nMaxGet;/// 获取下一个800条记录
	}

	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedDetailTrade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	DoDataExchange(true,IDC_CHECK_SORT);

	HRESULT hRet(E_FAIL);
	LONG nStart = 0;
	USHORT nMaxGet = 2000;
	USHORT nGetCount = 2000;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	while(TRUE)
	{
		if(!m_bSort)
			hRet = m_spiMarket->GetDetailTransData(VARIANT_TRUE,bstrStockCode,nStart,nGetCount,&spiRecordIndex);
		else
			hRet = m_spiMarket->GetDetailTransData(VARIANT_FALSE,bstrStockCode,nStart,nGetCount,&spiRecordIndex);
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
		nStart += nMaxGet;/// 获取下一个800条记录

		::Sleep(200);
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedDetailTask(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	DoDataExchange(true,IDC_CHECK_SORT);
	HRESULT hRet(E_FAIL);
	LONG nStart = 0;
	USHORT nGetCount = 2000;
	USHORT nMaxGet = 2000;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	while(TRUE)
	{
		if(!m_bSort)
			hRet = m_spiMarket->GetDetailOrderData(VARIANT_TRUE,bstrStockCode,nStart,nGetCount,&spiRecordIndex);
		else
			hRet = m_spiMarket->GetDetailOrderData(VARIANT_FALSE,bstrStockCode,nStart,nGetCount,&spiRecordIndex);
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
		nStart += nMaxGet;/// 获取下一个800条记录
	
		::Sleep(200);
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedDuiLi(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
		return 0;
	VARIANT_BOOL bConnected = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnected);
	if(VARIANT_FALSE == bConnected)
	{
		this->MessageBox(L"连接已经失效，请重新登录！");
		return 0;
	}
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	m_spiMarket->GetBuySellList(bstrStockCode,&spiRecordIndex);
	if(NULL != spiRecordIndex)
	{
		CComBSTR bstrJsonInfo;
		spiRecordIndex->GetJsonString(&bstrJsonInfo);
		this->MessageBox(bstrJsonInfo.m_str);
		bstrJsonInfo.Empty();
		spiRecordIndex = NULL;
	}
	else
	{
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	return 0;
}

LRESULT CMarketLevel2Dlg::OnBnClickedCodes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket)
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	VARIANT_BOOL bConnFlag = VARIANT_FALSE;
	m_spiMarket->get_IsConnected(&bConnFlag);
	if(VARIANT_FALSE == bConnFlag)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nStockCount = 0;
	/// 获得上海市场代码和名称
	m_spiMarket->GetTotalCount(EXCHANGETYPE_SH,&nStockCount);
	if(!nStockCount)
	{
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	CComBSTR bstrCode,bstrName;
	USHORT nStart = 0;
	while(nStockCount)
	{
		USHORT nRetCount = 0;
		/// 获得上海市场代码和名称等
		m_spiMarket->GetList(EXCHANGETYPE_SH,nStart,&nRetCount,&spiRecordIndex);
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
			break;/// 获取完成
		::Sleep(100);
	}
	nStart = 0;
	nStockCount = 0;
	/// 获得深圳市场代码和名称等
	m_spiMarket->GetTotalCount(EXCHANGETYPE_SZ,&nStockCount);
	if(!nStockCount)
	{
		CComBSTR bstrErrInfo;
		m_spiMarket->get_LastErrDesc(&bstrErrInfo);
		if(bstrErrInfo.Length())
			this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}
	while(nStockCount)
	{
		USHORT nRetCount = 0;
		m_spiMarket->GetList(EXCHANGETYPE_SZ,nStart,&nRetCount,&spiRecordIndex);
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
			break;/// 获取完成
		::Sleep(100);
	}

	return 0;
}
