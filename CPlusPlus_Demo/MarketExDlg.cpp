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
	/// 设置回调参数，用于事件发生时匹配是谁侦听的
	m_spiMarketEvent[nIndex]->SetCallPara(this->m_hWnd,nIndex);
	//建立事件连接
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
		USHORT nCurConnID = 0;
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

	this->GetDlgItem(IDC_EDIT_CODE).SetWindowText(L"IH12");

	/// 北京联通主站Z1
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"202.108.253.154");
	/// 上海电信主站Z1
//	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"180.153.18.176");
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).SetWindowText(L"7721");

	m_wndMarketType.Attach(GetDlgItem(IDC_COMBO_MARKETTYPE).m_hWnd);
	int nItemIndex = m_wndMarketType.AddString(L"中金所股指期货");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_CFFE);
	nItemIndex = m_wndMarketType.AddString(L"郑州商品");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_ZZF);
	nItemIndex = m_wndMarketType.AddString(L"大连商品");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_DLF);
	nItemIndex = m_wndMarketType.AddString(L"主力期货合约");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_MAINFC);
	nItemIndex = m_wndMarketType.AddString(L"上海期货");
	m_wndMarketType.SetItemData(nItemIndex,MARKETTYPE_SHF);
	m_wndMarketType.SetCurSel(0);

	return TRUE;
}

LRESULT CMarketExDlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam > 1)
		return 0;
	IMarketExPtr spiMarket = m_spiMarketEvent[lParam]->GetMarket();
	if(NULL == spiMarket)
		return 0;/// 错误交易接口
	if(wParam)
	{
		/// 连接
		VARIANT_BOOL bRet = VARIANT_FALSE;
#ifdef ZM_TDXSERVER_SYNC
		HRESULT hRet = spiMarket->Connect(VARIANT_FALSE,&bRet);
#else
		HRESULT hRet = spiMarket->Connect(VARIANT_TRUE,&bRet);
#endif
		if(VARIANT_FALSE == bRet)
		{
			/// 获取错误信息
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
		this->MessageBox(L"扩展行情连接服务器成功！");
#endif

#ifdef _DEBUG
		if(m_spiMarket[0] == spiMarket)
		{
#ifdef ZM_MUTI_ACCOUNT
			/// 多账号使用，创建多个COM组件对象
			HRESULT hRet = m_spiMarket[1].CreateInstance(__uuidof(MarketEx));
			ATLASSERT(m_spiMarket[1]);
			if(NULL != m_spiMarket[1])
			{
				/// 建立事件连接
				AdviseMarketCom(1);

				/// 启用调试日志输出
				hRet = m_spiMarket[1]->put_EnableLog(VARIANT_TRUE);
				/// 初始化使用环境，单个实例只需要调用一次，根据客户端版本号，最大连接数(默认为1，只有高级版才支持多个连接)
				/// 异步操作，需要接收事件通知得到初始化结果
#ifdef ZM_TDXSERVER_DEMO
				m_spiMarket[1]->Init(1);
#else
#ifndef ZM_VRSION_ADVANCED
				m_spiMarket[1]->Init(1);
#else
				m_spiMarket[1]->Init(1);///最大连接数，可以设置多个服务器，这儿可以现在只用几个服务器
#endif
#endif
			}
#endif
		}
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
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
		return 0;/// 错误交易接口
	if(wParam)
	{
		USHORT nConnID = 0;
		HRESULT hRet = spiMarket->get_CurConnID(&nConnID);
		ATLASSERT(nConnID);
		ULONG nSpeed = 0;
		spiMarket->get_ConnSpeed(&nSpeed);/// 返回连接服务器时间

		this->MessageBox(L"异步连接服务器成功！");
	}
	else
	{
		/// 连接失败，弹出错误描述
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
		return 0;/// 无效索引
	if(NULL == m_spiMarketEvent[nConnIndex])
		return 0;/// 对象已经释放
	this->UnAdviseMarketCom(nConnIndex);
	return 0;
}

LRESULT CMarketExDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// 无效索引
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

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

LRESULT CMarketExDlg::OnStockMintue(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// 无效索引
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

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

LRESULT CMarketExDlg::OnStockTrans(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// 无效索引
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

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

LRESULT CMarketExDlg::OnStockBars(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(lParam >= ZM_ACCOUNT_CONNMAXCOUNT)
		return 0;/// 无效索引
	if(NULL == m_spiMarketEvent[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiMarketEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
//#ifdef _DEBUG
	/// 获取JSON编码的数据内容
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
//#endif
	/// 根据接口方法遍历数据
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

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
	/// 释放COM组件对象
	if(NULL != m_spiMarket[0])
	{
		/// 释放所有连接
		this->UnAdviseMarketCom(0);
	}
#ifdef ZM_MUTI_ACCOUNT
	if(NULL != m_spiMarket[1])
	{
		/// 断开事件连接
		UnAdviseMarketCom(1);
	}
#endif
}

LRESULT CMarketExDlg::OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiMarket[0])
		return 0;
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	this->UnAdviseMarketCom(0);
	if(!nCurConnID)
		this->MessageBox(L"未连接，无需释放！");
	else
		this->MessageBox(L"释放连接成功！");
	return 0;
}

LRESULT CMarketExDlg::OnBnClickedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiMarket[0])
	{
		USHORT nConnID = 0;
		m_spiMarket[0]->get_CurConnID(&nConnID);
		if(nConnID)
			return 0;/// 正常连接
		this->UnAdviseMarketCom(0);
	}
	/// 获取初始化参数
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).GetWindowText(&bstrServerPort);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());

	HRESULT hRet = m_spiMarket[0].CreateInstance(__uuidof(MarketEx));
	ATLASSERT(m_spiMarket[0]);
	if(NULL == m_spiMarket)
	{
		this->MessageBox(L"创建扩展行情接口失败，请检查是否已经成功注册COM组件！");
		return 0;
	}

	/// 启用调试日志输出
	hRet = m_spiMarket[0]->put_EnableLog(VARIANT_TRUE);
	/// 设置行情服务器地址
	m_spiMarket[0]->put_CurServerHost(bstrServerAddr);
	/// 设置行情服务器端口
	m_spiMarket[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	this->AdviseMarketCom(0);
	/// 初始化
	m_spiMarket[0]->Init(1);
	return 1;
}

LRESULT CMarketExDlg::OnBnClickedCodes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	CComBSTR bstrTitle;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 获取支持的交易市场列表
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	ITradeRecordPtr spiRecord = NULL;
	/// 获取中金所IF 17年9月合约日K线数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1DAY,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	/// 获得行情报价
	m_spiMarket[0]->GetBreedQuote(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	/// 获取郑州交易所棉花17年9月合约周K线数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1WEEK,(EZMMarketType )dwType,CComBSTR(L"CF1801"),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// 获取中金所IF 17年9月合约日K线数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1MONTH,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// 获取中金所IF 17年9月合约小时K线数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1HOUR,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// 获取中金所IF 17年9月合约小时K线数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_5MINUTE,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// 获取中金所IF 17年9月合约小时K线数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetBreedBars(nCurConnID,ZMBARTYPE_1MINUTE,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());

	ITradeRecordPtr spiRecord = NULL;
	/// 获取中金所IF 17年9月合约分时图数据
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	hRet = m_spiMarket[0]->GetMinuteTimeData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	COleDateTime BeginTime = COleDateTime::GetCurrentTime();
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	/// 获取中金所IF合约分时图数据
	/// 指定一个日期
	hRet = m_spiMarket[0]->GetHisMinuteTimeData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),20170930,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	/// 获取中金所IF合约分时成交数据
	hRet = m_spiMarket[0]->GetTransData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还未成功连接！");
		return 0;
	}
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	ITradeRecordPtr spiRecord = NULL;
	USHORT nCount = 800;///单次最多获取800条记录
	HRESULT hRet(E_FAIL);
	/// 获取中金所合约分时成交数据
	/// 指定一个日期
	hRet = m_spiMarket[0]->GetHisTransData(nCurConnID,(EZMMarketType )dwType,CComBSTR(bstrGetCode),20170930,0,nCount,&spiRecord);
	if(FAILED(hRet) || NULL == spiRecord)
	{
		this->MessageBox(L"获取所有品种代码错误！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	DWORD dwType = (DWORD )m_wndMarketType.GetItemData(m_wndMarketType.GetCurSel());
	CString strStockCode(bstrStockCode.m_str);
	USHORT nConnID = 0;
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
