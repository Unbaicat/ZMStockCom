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
	/// 设置回调参数，用于事件发生时匹配是谁侦听的
	m_spiMarketEvent[nIndex]->SetCallPara(this->m_hWnd,nIndex);
	//建立事件连接
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
		USHORT nCurConnID = 0;
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

	m_spiMarket[1] = NULL;
	m_nMarketEventCookie[1] = 0;
	m_spiMarketEvent[1] = NULL;

	/// 北京联通主站Z1
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"202.108.253.130");
	/// 北京联通主站Z2
//	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).SetWindowText(L"北京联通主站Z1");
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
		this->MessageBox(L"普通行情连接服务器成功！");
#endif

#ifdef _DEBUG
		if(m_spiMarket[0] == spiMarket)
		{
#ifdef ZM_MUTI_ACCOUNT
			/// 多账号使用，创建多个COM组件对象
			HRESULT hRet = m_spiMarket[1].CreateInstance(__uuidof(Market));
			ATLASSERT(m_spiMarket[1]);
			if(NULL != m_spiMarket[1])
			{
				/// 建立事件连接
				AdviseMarketCom(1);

				/// 启用调试日志输出
				hRet = m_spiMarket[1]->put_EnableLog(VARIANT_TRUE);
				/// 初始化使用环境，单个实例只需要调用一次，根据客户端版本号，最大连接数(默认为1，只有高级版才支持多个连接)
				/// 异步操作，需要接收事件通知得到初始化结果
				m_spiMarket[1]->Init(1);///最大连接数，可以设置多个服务器，这儿可以现在只用几个服务器
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
		return 0;/// 错误交易接口
	if(wParam)
	{
		USHORT nConnID = 0;
		HRESULT hRet = spiMarket->get_CurConnID(&nConnID);
		ATLASSERT(nConnID);
		ULONG nSpeed = 0;
		spiMarket->get_ConnSpeed(&nSpeed);/// 返回连接服务器时间
		if(nConnID)
			this->MessageBox(L"异步连接服务器成功！");
#ifdef ZM_TDXSERVER_SYNC
		/// 同步请求某只股票的实时5档行情
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
		/// 异步请求多只股票的实时5档行情，可指定优先级和返回的请求序列号。nReqID会在事件回调中传回
		ULONG nReqID = 0;
		spiMarket->GetQuotes(nConnID,CComBSTR(L"000001;000002;600001"),&nReqID);
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
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
		return 0;/// 无效索引
	if(NULL == m_spiMarket[nConnIndex])
		return 0;
	/// 服务器产生错误，需要重新登录使用
	m_spiMarket[nConnIndex]->Disconnect(0);
	/// 请重新登录
	return 0;
}

LRESULT CMarketDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
	if(bstrJsonVal.Length())
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

LRESULT CMarketDlg::OnStockMintue(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
	if(bstrJsonVal.Length())
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

LRESULT CMarketDlg::OnStockTrans(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
	if(bstrJsonVal.Length())
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

LRESULT CMarketDlg::OnStockBars(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
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
	if(bstrJsonVal.Length())
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

LRESULT CMarketDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReleaseCom();
	EndDialog(wID);
	return 0;
}

void CMarketDlg::ReleaseCom()
{
	/// 释放COM组件对象
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
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	this->UnAdviseMarketCom(0);
	if(!nCurConnID)
	{
		this->MessageBox(L"未连接，无需释放！");
	}
	else
		this->MessageBox(L"释放连接成功！");
	return 0;
}

LRESULT CMarketDlg::OnBnClickedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiMarket[0])
	{
		USHORT nConnID = 0;
		m_spiMarket[0]->get_CurConnID(&nConnID);
		if(nConnID)
			return 0;/// 正常连接
		/// 释放连接
		this->UnAdviseMarketCom(0);
	}
	/// 获取初始化参数
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_MARKETSERVERADDR).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_MARKETSERVERPORT).GetWindowText(&bstrServerPort);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());

	HRESULT hRet = m_spiMarket[0].CreateInstance(__uuidof(Market));
	ATLASSERT(m_spiMarket[0]);
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"创建普通行情接口失败，请检查是否已经成功注册COM组件！");
		return 0;
	}
	/// 启用调试日志输出
	hRet = m_spiMarket[0]->put_EnableLog(VARIANT_TRUE);
	/// 设置行情服务器地址
	m_spiMarket[0]->put_CurServerHost(bstrServerAddr);
	/// 设置行情服务器端口
	m_spiMarket[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	hRet = this->AdviseMarketCom(0);

	ATLASSERT(NULL != m_spiMarket[0]);
	if(NULL != m_spiMarket[0])
	{
		/// 初始化使用参数，最大连接数
		hRet = m_spiMarket[0]->Init(1);
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedGetf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 财务信息
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 最长历史K线
	USHORT nMaxGet = 800;
	LONG nStart = 5600;

	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1DAY,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		/// 需要判断代码是股票还是指数，分别调用不同的方法
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1DAY,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
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
	return 1;
}

LRESULT CMarketDlg::OnBnClickedCodes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nStockCount = 0;
	/// 获得上海市场代码和名称
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
		/// 获得上海市场代码和名称等
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
			break;/// 获取完成
		::Sleep(100);
	}
	nStart = 0;
	nStockCount = 0;
	/// 获得深圳市场代码和名称等
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
			break;/// 获取完成
		::Sleep(100);
	}
	return 0;
}

LRESULT CMarketDlg::OnBnClickedXdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 除权信息
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}

	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	m_spiMarket[0]->GetCompanyInfoIndex(nCurConnID,bstrGetCode,&spiRecordIndex);
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

LRESULT CMarketDlg::OnBnClickedMk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///最多可获取的分钟数量
	LONG nStart = 5600;
	HRESULT hRet(E_FAIL);

	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
		/// 区分是股票还是指数分别调用不同的方法
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
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

	return 1;
}

LRESULT CMarketDlg::OnBnClickedHmk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}

	return 0;
}

LRESULT CMarketDlg::OnBnClickedWk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 最长历史K线
	USHORT nMaxGet = 800;
	LONG nStart = 800;

	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1WEEK,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1WEEK,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
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
	return 1;
}

LRESULT CMarketDlg::OnBnClickedFst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	SHORT nGetDay = 30;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 指定一个日期
	hRet = m_spiMarket[0]->GetHisMinuteTimeData(nCurConnID,bstrGetCode,20170930,&spiRecordIndex);
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
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
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
		nStart += nMaxGet;/// 获取下一个800条记录
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedHfst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	SHORT nGetDay = 30;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 2000;
	LONG nStart = 0;
	while(TRUE)
	{
		/// 指定一个日期
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
		nStart += nMaxGet;/// 获取下一个800条记录
	}
	return 1;
}

LRESULT CMarketDlg::OnBnClickedHkd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///最多可获取的小时K线数量
	LONG nStart = 1600;
	HRESULT hRet(E_FAIL);

	while(nStart >= 0)
	{
		/// 最大获取800记录，判断是获取股票的还是指数的
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1HOUR,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1HOUR,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
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
	return 1;
}

LRESULT CMarketDlg::OnBnClickedFivemk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	USHORT nMaxGet = 800;
	///最多可获取的5分钟数量
	LONG nStart = 23200;

	HRESULT hRet(E_FAIL);
	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_5MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_5MINUTE,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
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

	return 1;
}

LRESULT CMarketDlg::OnBnClickedMkx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
	USHORT nCurConnID = 0;
	HRESULT hRet =m_spiMarket[0]->get_CurConnID(&nCurConnID);
	if(!nCurConnID)
	{
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}

	CComBSTR bstrGetCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrGetCode);
	if(6 != bstrGetCode.Length())
	{
		this->MessageBox(L"代码不合法！");
		return 0;
	}
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;
	/// 最长历史K线
	USHORT nMaxGet = 800;
	LONG nStart = 0;

	while(nStart >= 0)
	{
		/// 最大获取800记录
		hRet = m_spiMarket[0]->GetBars(nCurConnID,ZMBARTYPE_1MONTH,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
//		hRet = m_spiMarket[0]->GetIndexBars(nCurConnID,ZMBARTYPE_1MONTH,bstrGetCode,nStart,nMaxGet,&spiRecordIndex);
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
	return 1;
}

LRESULT CMarketDlg::OnBnClickedTotalf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
	{
		this->MessageBox(L"还没有初始化连接！");
		return 0;
	}
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
		this->MessageBox(L"还没有连接成功！");
		return 0;
	}
	COleDateTime GetTime = COleDateTime::GetCurrentTime()-COleDateTimeSpan(90,0,0,0);

	CComBSTR bstrVal;
	ULONG nFieldCount = 0,nRecordCount = 0;
	ITradeRecordPtr spiRecordIndex = NULL;

	/// 财务信息
	m_spiMarket[0]->GetFinanceInfo(nCurConnID,bstrGetCode,&spiRecordIndex);
	if(NULL == spiRecordIndex)
	{
		this->MessageBox(L"获取失败！");
		return 0;
	}
	spiRecordIndex->get_FieldCount(&nFieldCount);
	spiRecordIndex->get_RecordCount(&nRecordCount);
	if(!nRecordCount || nFieldCount < 37)
	{
		spiRecordIndex = NULL;
		this->MessageBox(L"获取数据异常！");
		return 0;
	}
	CComVariant varVal;
	spiRecordIndex->GetValueByName(0,CComBSTR(L"财务更新日期"),&varVal);
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
	spiRecordIndex->GetValueByName(0,CComBSTR(L"上市日期"),&varVal);
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

	spiRecordIndex->GetValueByName(0,CComBSTR(L"市场"),&varVal);
	varVal.ChangeType(VT_UI2);
	USHORT nMarket = varVal.uiVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"所属省份"),&varVal);
	varVal.ChangeType(VT_UI2);
	USHORT nProvince = varVal.uiVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"所属行业"),&varVal);
	varVal.ChangeType(VT_UI2);
	USHORT nIndustry = varVal.uiVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"流通股本"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nCCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"证券代码"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strFCode = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"总股本"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTCount = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"国家股"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nSCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"发起人法人股"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nICount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"法人股"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nLCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"B股"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nBCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"H股"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nHCount = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"职工股"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strECount = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"总资产"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"流动资产"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strCCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"固定资产"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strFCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"无形资产"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strICapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"资本公积金"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strRCapital = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"股东人数"),&varVal);
	varVal.ChangeType(VT_UI4);
	ULONG nSNumber = varVal.ulVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"流动负债"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strCLiability = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"长期负债"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strLLiability = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"净资产"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strNetAssets = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"主营收入"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strMainSale = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"主营利润"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strMainProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"应收帐款"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strReceivables = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"营业利润"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTradeProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"投资收益"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strInvestIn = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"经营现金流"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strCashFlow = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"总现金流"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTCash = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"存货"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strInventory = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"利润总额"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strTotalProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"税后利润"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strATProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"净利润"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strNetProfit = varVal.bstrVal;
	varVal.Clear();
	spiRecordIndex->GetValueByName(0,CComBSTR(L"未分利润"),&varVal);
	varVal.ChangeType(VT_BSTR);
	CString strUProfit = varVal.bstrVal;
	varVal.Clear();

	return 1;
}

LRESULT CMarketDlg::OnBnClickedQuote(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	if(NULL == m_spiMarket[0])
		return 0;
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_CODE).GetWindowText(&bstrStockCode);
	if(!bstrStockCode.Length())
		return 0;
	CString strStockCode(bstrStockCode.m_str);
	ULONG nReqID = 0;
	USHORT nConnID = 0;
	m_spiMarket[0]->get_CurConnID(&nConnID);
	m_spiMarket[0]->GetQuotes(nConnID,bstrStockCode,&nReqID);

	return 0;
}
