// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include <atlcomtime.h>

#include "MarketDlg.h"
#include "MarketExDlg.h"
#include "MarketLevel2Dlg.h"

#ifndef WIN64
/// 定义是否同步处理数据
#define	ZM_TDXSERVER_SYNC
#endif

void CMainDlg::AdviseTradeClient(int nIndex)
{
	if(NULL != m_spiTradeClientEvent[nIndex])
		return;
	m_nTradeEventCookie[nIndex] = NULL;
	this->m_spiTradeClientEvent[nIndex] = new CComObject <CStockComEvent>;
	ATLASSERT(this->m_spiTradeClientEvent[nIndex]);
	if(NULL == this->m_spiTradeClientEvent[nIndex])
		return;	
	/// 设置回调参数，用于事件发生时匹配是哪个对象侦听的
	m_spiTradeClientEvent[nIndex]->SetCallPara(this->m_hWnd,nIndex);
	//建立事件连接
	HRESULT hRet = AtlAdvise(m_spiTrade[nIndex],this->m_spiTradeClientEvent[nIndex]->GetUnknown(),
		DIID__ITradeEvents,&this->m_nTradeEventCookie[nIndex]);
}

void CMainDlg::UnAdviseTradeClient(int nIndex)
{
	if(0 == m_nTradeEventCookie[nIndex])
		return;
	AtlUnadvise(m_spiTrade[nIndex],DIID__ITradeEvents,m_nTradeEventCookie[nIndex]);
	m_nTradeEventCookie[nIndex] = 0;
	m_spiTradeClientEvent[nIndex] = NULL;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	m_wndTradeList.Attach(GetDlgItem(IDC_COMBO_TRADETYPE).m_hWnd);
	int nItemIndex = m_wndTradeList.AddString(L"资金");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CAPITAL);
	nItemIndex = m_wndTradeList.AddString(L"股份");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_STOCK);
	nItemIndex = m_wndTradeList.AddString(L"当日委托");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYORDER);
	nItemIndex = m_wndTradeList.AddString(L"当日成交");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYTRADE);
	nItemIndex = m_wndTradeList.AddString(L"当日可撤委托");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYCANREVOKE);
	nItemIndex = m_wndTradeList.AddString(L"股东代码");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_SHAREHOLDERCODE);
	nItemIndex = m_wndTradeList.AddString(L"融资余额");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BALANCEOFFINANCING);
	nItemIndex = m_wndTradeList.AddString(L"融券余额");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BORROWFUNDLEFT);
	nItemIndex = m_wndTradeList.AddString(L"可融证券");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANBORROWFUND);
	nItemIndex = m_wndTradeList.AddString(L"可申购新股");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANNEWSTOCK);
	nItemIndex = m_wndTradeList.AddString(L"新股申购额度");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANNEWSTOCKLIMIT);
	nItemIndex = m_wndTradeList.AddString(L"配号");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_ASSIGNNUMBER);
	nItemIndex = m_wndTradeList.AddString(L"中签");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BALLOT);
	m_wndTradeList.SetCurSel(0);

	m_wndHistoryList.Attach(GetDlgItem(IDC_COMBO_HISLIST).m_hWnd);
	nItemIndex = m_wndHistoryList.AddString(L"历史委托");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_TRUST);
	nItemIndex = m_wndHistoryList.AddString(L"历史成交");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_TRADE);
	nItemIndex = m_wndHistoryList.AddString(L"资金流水");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_CAPITALLIST);
	nItemIndex = m_wndHistoryList.AddString(L"交割单");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_DELIVERYORDER);
	m_wndHistoryList.SetCurSel(0);

	m_wndAccountType.Attach(GetDlgItem(IDC_COMBO_ACCOUNTTYPE).m_hWnd);
	nItemIndex = m_wndAccountType.AddString(L"资金账号");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_CAPITAL);
	nItemIndex = m_wndAccountType.AddString(L"客户号");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_CUSTOMER);
	nItemIndex = m_wndAccountType.AddString(L"深Ａ股东");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_SZA);
	nItemIndex = m_wndAccountType.AddString(L"沪Ａ股东");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_SHA);
	m_wndAccountType.SetCurSel(0);

	m_wndBrokerType.Attach(GetDlgItem(IDC_COMBO_BROKETYPE).m_hWnd);
	nItemIndex = m_wndBrokerType.AddString(L"模拟测试");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_MNCS);
	nItemIndex = m_wndBrokerType.AddString(L"长江证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"第一创业");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DYCY);
	nItemIndex = m_wndBrokerType.AddString(L"东莞证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DGZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国信证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华泰联合证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_LHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"平安证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_PAZQ);
	nItemIndex = m_wndBrokerType.AddString(L"广发证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"大通证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华西证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"兴业证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"招商证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZSZQ);
	nItemIndex = m_wndBrokerType.AddString(L"金元证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JYDT);
	nItemIndex = m_wndBrokerType.AddString(L"中信建投");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZXJT);
	nItemIndex = m_wndBrokerType.AddString(L"红塔证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YNHT);
	nItemIndex = m_wndBrokerType.AddString(L"长城证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"宏源证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国泰君安");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GTJA);
	nItemIndex = m_wndBrokerType.AddString(L"世纪证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"安信证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_AXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"财富证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"东兴证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"银河证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"光大证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"英大证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"德邦证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DBZQ);
	nItemIndex = m_wndBrokerType.AddString(L"南京证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_NJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中信证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"上海证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华宝证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HBZQ);
	nItemIndex = m_wndBrokerType.AddString(L"爱建证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_AJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中泰证券(齐鲁证券)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_QLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中银国际");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZYGJ);
	nItemIndex = m_wndBrokerType.AddString(L"民族证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_MZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"湘财证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国金证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"首创证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国联证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华林证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华福证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国元证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"广州证券(岭南创富)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"方正证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_FZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"渤海证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_BHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"西南证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XNZQ);
	nItemIndex = m_wndBrokerType.AddString(L"新时代证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XSDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中投证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华融证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HRZQ);
	nItemIndex = m_wndBrokerType.AddString(L"申万宏源");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SYWG);
	nItemIndex = m_wndBrokerType.AddString(L"华鑫证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"东北证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JLDB);
	nItemIndex = m_wndBrokerType.AddString(L"民生证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_MSZQ);
	nItemIndex = m_wndBrokerType.AddString(L"大同证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SXDT);
	nItemIndex = m_wndBrokerType.AddString(L"联储证券(众成证券)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"长城国瑞证券(厦门证券)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XMZQ);
	nItemIndex = m_wndBrokerType.AddString(L"东方证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"银泰证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中航证券(江南证券)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JLDT);
	nItemIndex = m_wndBrokerType.AddString(L"万和证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_WHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国开证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GKZQ);
	nItemIndex = m_wndBrokerType.AddString(L"青岛证券(中信万通)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZXWT);
	nItemIndex = m_wndBrokerType.AddString(L"信达证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"五矿证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_WKZQ);
	nItemIndex = m_wndBrokerType.AddString(L"江海证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华创证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"太平洋证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_TPYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国海证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"东海证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"西部证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XBZQ);
	nItemIndex = m_wndBrokerType.AddString(L"山西证券(汇通启富)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"开源证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_KYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华安证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HAHX);
	nItemIndex = m_wndBrokerType.AddString(L"国盛证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GSZQ);
	nItemIndex = m_wndBrokerType.AddString(L"宏信证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SCHX);
	nItemIndex = m_wndBrokerType.AddString(L"万联证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_WLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中天证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_LNZT);
	nItemIndex = m_wndBrokerType.AddString(L"恒泰证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_NMHT);
	nItemIndex = m_wndBrokerType.AddString(L"天风证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_TFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"华龙证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GSHL);
	nItemIndex = m_wndBrokerType.AddString(L"国融证券(日信证券)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_RXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中航证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"财通证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"海通证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"东吴证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DWZQ);
	nItemIndex = m_wndBrokerType.AddString(L"浙商证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZJZS);
	nItemIndex = m_wndBrokerType.AddString(L"联讯证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_LXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"上海华信");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SHHX);
	nItemIndex = m_wndBrokerType.AddString(L"东方财富");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XZTX);
	nItemIndex = m_wndBrokerType.AddString(L"中原证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"国都证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_BJGD);
	nItemIndex = m_wndBrokerType.AddString(L"中金证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"中邮证券");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SXZY);
	m_wndBrokerType.SetCurSel(0);

	m_nTradeEventCookie[0] = 0;
	m_spiTradeClientEvent[0] = NULL;
	m_nTradeEventCookie[1] = 0;
	m_spiTradeClientEvent[1] = NULL;

	/// 创建COM对象前请先注册，管理员权限执行ZMStockCom.exe /RegServer(为当前账号注册，如果需要，请注册为每一个用户，参数是RegServerPerUser)
	/// 卸载执行ZMStockCom.exe /UnregServer(为当前账号注册，如果注册成了每一个用户，参数是UnregServerPerUser)
	/// 如果创建失败，请先检查是否注册成功，检查运行所需要的依赖DLL是否都和本程序在一个目录
	HRESULT hRet = m_spiTrade[0].CreateInstance(__uuidof(StockTrade));
	ATLASSERT(m_spiTrade[0]);
	if(NULL != m_spiTrade[0])
	{
		/// 建立事件连接
		AdviseTradeClient(0);

		/// 启用调试日志输出
		hRet = m_spiTrade[0]->put_EnableLog(VARIANT_TRUE);
		/// 初始化使用环境，单个实例只需要调用一次，根据客户端版本号，最大连接数(默认为1)，最好和设置的服务器数量匹配
		/// 为提高执行效率，实现为异步操作，需要接收事件通知得到初始化结果
		m_spiTrade[0]->Init(CComBSTR(L"8.05"),1);
	}
	/// 默认设置为通达信模拟服务器IP，实际使用时请设置为您券商的交易服务器IP
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).SetWindowText(L"mock.tdx.com.cn");
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).SetWindowText(L"7708");
	this->GetDlgItem(IDC_EDIT_YYBID).SetWindowText(L"9000");
	/// 券商给你开通的登录账号ID
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).SetWindowText(L"");
	/// 券商给你开通的交易账号ID
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).SetWindowText(L"");

	this->GetDlgItem(IDC_EDIT_TRADEPASSWORD).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_COMMPASSWORD).SetWindowText(L"");

	this->GetDlgItem(IDC_EDIT_STOCKCODE).SetWindowText(L"000001");
	return TRUE;
}

LRESULT CMainDlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piTrade = (IDispatch* )lParam;
	IStockTradePtr spiTrade(piTrade);
	if(NULL == spiTrade)
		return 0;/// 错误交易接口
	if(wParam)
	{
#ifdef _DEBUG
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		if(m_spiTrade[0] == spiTrade)
		{
			this->MessageBox(L"对象1初始化接口成功！");
		}
		else if(m_spiTrade[1] == spiTrade)
			this->MessageBox(L"对象2初始化接口成功！");
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
		CComBSTR bstrErrInfo;
		spiTrade->get_LastErrDesc(&bstrErrInfo);
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMainDlg::OnLoginReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piTrade = (IDispatch* )lParam;
	IStockTradePtr spiTrade(piTrade);
	if(NULL == spiTrade)
		return 0;/// 错误交易接口
	if(wParam)
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		HRESULT hRet = spiTrade->get_ConnectValid(&bConnectValid);
		USHORT nTradeID = 0;
		hRet = spiTrade->get_CurTradeID(&nTradeID);
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		this->MessageBox(L"异步登录服务器成功！下面开始获取股东代码信息！");
		ITradeRecordPtr spiRecord = NULL;
		/// 获得股东账号信息
		spiTrade->get_ShareHolderCode(&spiRecord);
		if(NULL != spiRecord)
		{
			/// 获取JSON编码的数据内容
			CComBSTR bstrJsonVal;
			spiRecord->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();
			/// 股东代码对象，底层会缓存数据备用，所以请尽量不要调用Clear清空，避免影响底层功能
//			spiRecord->Clear();
			spiRecord = NULL;
		}
#ifdef ZM_TDXSERVER_SYNC
		/// 同步请求某只股票的实时5档行情
		spiRecord = NULL;
		this->MessageBox(L"下面开始获取指定股票的实时5档行情！");
		spiTrade->GetStockQuote(nTradeID,CComBSTR(L"000001"),&spiRecord);
		ATLASSERT(spiRecord);
		ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
		if(NULL != spiRecord)
		{
			hRet = spiRecord->get_FieldCount(&nFieldCount);
			hRet = spiRecord->get_RecordCount(&nRecordCount);
			CComBSTR bstrJsonVal;
			spiRecord->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();
		}
#else
		/// 异步请求多只股票的实时5档行情，可指定优先级和返回的请求序列号。nReqID会在事件回调中传回
		ULONG nReqID = 0;
		spiTrade->GetStockQuotes(nTradeID,CComBSTR(L"000001;000002;600001"),RUNPRITYPE_ABOVE_NORMAL,&nReqID);
#endif
	}
	else
	{
		/// 连接失败，弹出错误描述
		CComBSTR bstrErrInfo;
		spiTrade->get_LastErrDesc(&bstrErrInfo);
#ifdef _DEBUG
		/// 事件通知中尽量避免弹窗MessageBox等阻塞操作，弹窗等仅限于调试程序方便
		this->MessageBox(bstrErrInfo.m_str);
#endif
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMainDlg::OnOrderOK(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam] || NULL == m_spiTrade[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiTradeClientEvent[lParam]->GetReqData();
	if(NULL == spiRecord)
		return 0;
	ULONG nRecordCount = 0,nFieldCount = 0;
	spiRecord->get_FieldCount(&nFieldCount);
	spiRecord->get_RecordCount(&nRecordCount);
	if(!nFieldCount || !nRecordCount)
		return 0;/// 异常数据
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
	/// 获取到委托序号，正常业务流程需要记录请求ReqID和成功回报OrderID的对应关系，便于后面进行撤单
	CComVariant varOrderID;
	spiRecord->GetValueByName(0,CComBSTR(L"委托编号"),&varOrderID);
	varOrderID.ChangeType(VT_BSTR);
	spiRecord->Clear();
	spiRecord = NULL;
	/// 执行取消委托，需要传入市场类型，由委托成功时通知返回，也可以自己计算，上海交易所2，深圳交易所为1
	m_spiTrade[lParam]->CancelOrder((EZMExchangeType)wParam,varOrderID.bstrVal,&spiRecord);
	varOrderID.Clear();
	return 0;
}

LRESULT CMainDlg::OnOrderSuccess(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CString strOrderID((BSTR)wParam);
	this->MessageBox((BSTR)lParam);
	return 0;
}

LRESULT CMainDlg::OnOrderError(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam] || NULL == m_spiTrade[lParam])
		return 0;/// 对象已经释放
	return 0;
}

LRESULT CMainDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(NULL == m_spiTradeClientEvent[lParam])
		return 0;/// 对象已经释放
	ITradeRecordPtr spiRecord = m_spiTradeClientEvent[lParam]->GetReqData();
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

LRESULT CMainDlg::OnChangeServer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{

	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	ReleaseCom();

	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReleaseCom();
	EndDialog(wID);
	return 0;
}

LONG StrToNum(const ATL::CString& strTemp)
{
	LONG nValue = 0;
	if(strTemp.IsEmpty())
		return nValue;
	BOOL bLoss = FALSE;
	if(strTemp.GetAt(0) == '-')
		bLoss = TRUE;
	int j = 0;
	if(bLoss)
		j++;
	for (; j < strTemp.GetLength(); j++)
	{
		TCHAR c = strTemp[j];
		if (c <= '9' && c >= '0')
		{
			nValue *= 10;
			nValue = (nValue + (c - '0')) ; 
		}
	}
	if(bLoss)
		return -nValue;
	return nValue;
}

void CMainDlg::ReleaseCom()
{
	/// 释放COM组件对象
	if(NULL != m_spiTrade[0])
	{
		m_spiTrade[0]->LogOut(0);
		/// 断开事件连接
		UnAdviseTradeClient(0);
		m_spiTrade[0] = NULL;
	}

	if(NULL != m_spiTrade[1])
	{
		m_spiTrade[1]->LogOut(0);
		/// 断开事件连接
		UnAdviseTradeClient(1);
		m_spiTrade[1] = NULL;
	}
}

LRESULT CMainDlg::OnBnClickedUnInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiTrade[0])
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			this->MessageBox(L"还没有建立与交易服务器的连接！");
			return 0;
		}
		m_spiTrade[0]->LogOut(0);
	}

	this->MessageBox(L"断开通讯成功！");
	return 0;
}

LRESULT CMainDlg::OnBnClickedInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiTrade)
	{
		this->MessageBox(L"交易COM组件对象还没有创建成功！");
		return 0;
	}
	/// 获取初始化参数
	CComBSTR bstrServerAddr,bstrServerPort,bstrYybID;
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).GetWindowText(&bstrServerAddr);
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).GetWindowText(&bstrServerPort);
	this->GetDlgItem(IDC_EDIT_YYBID).GetWindowText(&bstrYybID);
	ATLASSERT(bstrServerAddr.Length());
	ATLASSERT(bstrServerPort.Length());
	ATLASSERT(bstrYybID.Length());
	CComBSTR bstrLoginID,bstrTradeAccount;
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).GetWindowText(&bstrLoginID);
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).GetWindowText(&bstrTradeAccount);
	ATLASSERT(bstrLoginID.Length());
	CComBSTR bstrTradePassword,bstrCommPassword;
	this->GetDlgItem(IDC_EDIT_TRADEPASSWORD).GetWindowText(&bstrTradePassword);
	this->GetDlgItem(IDC_EDIT_COMMPASSWORD).GetWindowText(&bstrCommPassword);
	if(!bstrTradePassword.Length())
	{
		this->MessageBox(L"还没有设置交易密码，无法登录服务器！");
		return 0;
	}
	/// 设置券商类型和账号类型
	EZMLoginAccountType eAccountType = LOGINIACCOUNTTYPE_CAPITAL;
	if(m_wndAccountType.GetCurSel())
		eAccountType = (EZMLoginAccountType )m_wndAccountType.GetItemData(m_wndAccountType.GetCurSel());
	EZMBrokerType eBrokerType = BROKERTYPE_MNCS;
	if(m_wndBrokerType.GetCurSel())
		eBrokerType = (EZMBrokerType)m_wndBrokerType.GetItemData(m_wndBrokerType.GetCurSel());
	m_spiTrade[0]->put_BrokerType(eBrokerType);
	m_spiTrade[0]->put_AccountType(eAccountType);
	//m_spiTrade[0]->put_CreditAccount(VARIANT_TRUE);/// 设置是否信用账号
	/// 设置成交自动回报定时器1000毫秒，设为0表示不启用
	m_spiTrade[0]->put_ReportSuccessTimer(1000);

	/// 设置交易服务器地址
	m_spiTrade[0]->put_CurServerHost(bstrServerAddr);
	/// 设置交易服务器端口
	m_spiTrade[0]->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	/// 设置营业部ID
	m_spiTrade[0]->put_DepartmentID((USHORT)StrToNum(bstrYybID.m_str));
	/// 设置登录账户
	m_spiTrade[0]->put_LoginID(bstrLoginID);
	if(bstrTradeAccount.Length())
		m_spiTrade[0]->put_TradeAccount(bstrTradeAccount);/// 如果用资金账号登录，此值一般和登录账号一样，可以不设置
	/// 设置交易密码
	m_spiTrade[0]->put_TradePassword(bstrTradePassword);
	/// 设置通信密码
	if(bstrCommPassword.Length())
		m_spiTrade[0]->put_CommPassword(bstrCommPassword);/// 可以不设置，部分券商需要设置成和交易密码一样的，华泰证券需要设置通讯密码

	VARIANT_BOOL bRetFlag = VARIANT_FALSE;
#ifndef ZM_TDXSERVER_SYNC
	///  异步执行登录交易服务器，登录结果通过事件通知获得
	HRESULT hRet = m_spiTrade[0]->LogIn(VARIANT_TRUE,&bRetFlag);
#else
	///  同步执行登录交易服务器，直到返回结果
	HRESULT hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
	if(VARIANT_FALSE == bRetFlag)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return 0;
	}
	this->MessageBox(L"同步登录交易服务器成功！");
#endif
	return 1;
}

ITradeRecordPtr CMainDlg::GetCategoryData(EZMStockQueryType eCategoryType)
{
	ITradeRecordPtr spiRecord = NULL;
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// 通信不正常
	hRet = m_spiTrade[0]->QueryTradeData(nTradeID,eCategoryType,&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ATLASSERT(spiRecord);
	return spiRecord;
}

LRESULT CMainDlg::OnBnClickedGetTradeData(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	int nIndex = m_wndTradeList.GetCurSel();
	DWORD dwType = (DWORD )m_wndTradeList.GetItemData(nIndex);
	if(NULL == m_spiTrade)
		return 0;
	ITradeRecordPtr spiRecord = GetCategoryData((EZMStockQueryType)dwType);
	if(NULL != spiRecord)
	{
		ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
		HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
		hRet = spiRecord->get_RecordCount(&nRecordCount);
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
	}
	return 0;
}

LRESULT CMainDlg::OnBnClickedGetFiveMarket(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_STOCKCODE).GetWindowText(&bstrStockCode);
	ATLASSERT(bstrStockCode.Length());
	ITradeRecordPtr spiRecord = GetStockMarket(bstrStockCode.m_str);
	if(NULL == spiRecord)
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType = ZMDATATYPE_UNKNOW;
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

ITradeRecordPtr CMainDlg::GetHisData(EZMHisOrderType eCategoryType)
{
	ITradeRecordPtr spiRecord = NULL;
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// 通信不正常
	/// 指定时间范围
	COleDateTime curTime = COleDateTime::GetCurrentTime();
	CString strBeginTime,strEndTime;
	strEndTime.Format(L"%d%02d%02d",curTime.GetYear(),curTime.GetMonth(),curTime.GetDay());
	curTime -= COleDateTimeSpan(15,0,0,0);
	strBeginTime.Format(L"%d%02d%02d",curTime.GetYear(),curTime.GetMonth(),curTime.GetDay());
	hRet = m_spiTrade[0]->QueryHisTradeData(nTradeID,eCategoryType,CComBSTR(strBeginTime),CComBSTR(strEndTime),&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType = ZMDATATYPE_UNKNOW;
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
	return spiRecord;
}

LRESULT CMainDlg::OnBnClickedGetHisData(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	DWORD dwType = (DWORD )m_wndHistoryList.GetItemData(m_wndHistoryList.GetCurSel());
	if(NULL == m_spiTrade)
		return 0;
	ITradeRecordPtr spiRecord = GetHisData((EZMHisOrderType)dwType);
	if(NULL == spiRecord)
		return 0;
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	CComBSTR bstrName;
	for(nIndex = 0;nIndex < nFieldCount;nIndex++)
	{
		/// 输出返回记录的字段类型
		EZMDataType eDataType = ZMDATATYPE_UNKNOW;
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

ITradeRecordPtr CMainDlg::GetStockMarket(const CString& strStockCode)
{
	ITradeRecordPtr spiRecord = NULL;
	if(NULL == m_spiTrade)
	{
		this->MessageBox(L"组件还未创建成功！");
		return spiRecord;
	}
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
	{
		this->MessageBox(L"通信不正常！");
		return spiRecord;/// 通信不正常
	}
	hRet = m_spiTrade[0]->GetStockQuote(nTradeID,CComBSTR(strStockCode),&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ATLASSERT(spiRecord);
	return spiRecord;
}

LRESULT CMainDlg::OnBnClickedBuy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CComBSTR bstrStockCode;
	this->GetDlgItem(IDC_EDIT_STOCKCODE).GetWindowText(&bstrStockCode);
	ATLASSERT(bstrStockCode.Length());
	ITradeRecordPtr spiRecord = GetStockMarket(bstrStockCode.m_str);
	if(NULL == spiRecord)
		return 0;
	/// 所有接口需要处理的异常，如果COM组件服务进程崩溃，在请求接口功能的时候，会返回错误码0x800706BA，意思是“RPC 服务器不可用”
	/// 需要释放当前使用的交易接口对象后重新创建 HRESULT hRet = m_spiTrade.CreateInstance(__uuidof(Trade)) 后再自行连接

	CComVariant varVal;
	/// 取卖一价格主动买入
	HRESULT hRet(E_FAIL);
	EZMExchangeType eExchangeType = EXCHANGETYPE_UNKNOWN;
	/// 根据序号获取可能存在的问题：需要适配券商，券商不同，可能返回的字段顺序不一样
//	hRet = spiRecord->GetValue(0,16,&varVal);
	/// 或者用字段标题获取
	hRet = spiRecord->GetValueByName(0,CComBSTR(L"买五价"),&varVal);
	ULONG nReqID1 = 0;/// 返回唯一标识的委托ID，组件会以事件方式通知结果，如果您需要跟踪每个委托的结果通知，需要记录下来，和事件通知的ID进行匹配
	m_spiTrade[0]->AddOrder(STOCKORDERTYPE_BUY,ORDERPRICETYPE_LIMIT,bstrStockCode,varVal.fltVal,500,eExchangeType,&nReqID1);
	varVal.Clear();
	ATLASSERT(nReqID1);
	if(!nReqID1)
	{
		/// 添加委托失败，查询错误原因
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
	}
	/// 取当前价买入
//	hRet = spiRecord->GetValue(0,5,&varVal);
	hRet = spiRecord->GetValueByName(0,CComBSTR(L"买四价"),&varVal);
	ULONG nReqID2 = 0;/// 返回唯一标识的委托ID，组件会以事件方式通知结果
	hRet = m_spiTrade[0]->AddOrder(STOCKORDERTYPE_BUY,ORDERPRICETYPE_LIMIT,bstrStockCode,varVal.fltVal,500,eExchangeType,&nReqID2);
	bstrStockCode.Empty();
	ATLASSERT(nReqID1);
	if(!nReqID2)
	{
		/// 添加委托失败，查询错误原因
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
	}

	/// 提交委托，第一个参数如果为VARIANT_TRUE,底层会自动检查当前通信状态，不正常的时候会自动登录一次再提交委托
	VARIANT_BOOL bCommitRet = VARIANT_FALSE;
	/// 可设置提交的优先级
	USHORT nTradeID = 0;
	m_spiTrade[0]->get_CurTradeID(&nTradeID);
	hRet = m_spiTrade[0]->CommitOrder(nTradeID,VARIANT_FALSE,RUNPRITYPE_NORMAL,&bCommitRet);
	spiRecord->Clear();
	spiRecord = NULL;
	if(VARIANT_FALSE == bCommitRet)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return 0;
	}
	if(NULL != spiRecord)
	{
		/// 获得委托回报
		spiRecord->Clear();
		spiRecord = NULL;
	}
	return 0;
}

LRESULT CMainDlg::OnBnClickedSell(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	ITradeRecordPtr spiRecord = NULL;
	USHORT nTradeID = 0;
	HRESULT hRet = m_spiTrade[0]->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"请先初始化登录！");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// 一般是初始化成功后使用一段时间，在提交交易和查询请求前、或这些请求失败时才检查下连接是否正常
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade[0]->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// 重新执行同步登录请求，原来设置的相关参数会自动再次使用
			hRet = m_spiTrade[0]->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// 通信不正常
	/// 请求股票列表
	hRet = m_spiTrade[0]->QueryTradeData(nTradeID,STOCKQUERYTYPE_STOCK,&spiRecord);
	if(NULL == spiRecord)
	{
		/// 获取错误信息
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade[0]->get_LastErrType(&nErrCode);
		m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);
	if(!nFieldCount || !nRecordCount)
	{
		spiRecord = NULL;
		return 0;
	}
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

	/// 查找可卖出的股票
	CComVariant varVal;
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		spiRecord->GetValueByName(nIndex,CComBSTR(L"可用股份"),&varVal);
		varVal.ChangeType(VT_I2);
		if(!varVal.iVal)
			continue;/// 无可用，无法卖出的
		int nSellCount = varVal.iVal;
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"证券名称"),&varVal);
		CString strStockName(varVal.bstrVal);
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"证券代码"),&varVal);
		CString strStockCode(varVal.bstrVal);
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"股东代码"),&varVal);
		CString strHolderCode(varVal.bstrVal);
		varVal.Clear();

		/// 获取当前行情价格
		ITradeRecordPtr spiQuote = NULL;
		m_spiTrade[0]->GetStockQuote(nTradeID,CComBSTR(strStockCode),&spiQuote);
		if(NULL == spiQuote)
			continue;
		ULONG nQuoteCount = 0;
		spiQuote->get_RecordCount(&nQuoteCount);
		if(!nQuoteCount)
		{
			/// 没有获取到当前行情
			spiQuote = NULL;
			continue;
		}
		CComBSTR bstrJsonVal;
		spiQuote->GetJsonString(&bstrJsonVal);
		this->MessageBox(bstrJsonVal.m_str);
		bstrJsonVal.Empty();

		/// 挂卖5价卖出
		spiQuote->GetValueByName(0,CComBSTR(L"卖五价"),&varVal);
		varVal.ChangeType(VT_R4);
		spiQuote = NULL;
		float fSell = varVal.fltVal;
		varVal.Clear();
		/// 限价卖出所有可用股票
#ifndef ZM_TDXSERVER_SYNC
		/// 异步提交，在这儿要记录返回的请求ID，便于和事件通知里的委托回报相对应，从而本次知道实际委托结果
		ULONG nReqID = 0;
		m_spiTrade[0]->AddOrder(STOCKORDERTYPE_SALE,ORDERPRICETYPE_LIMIT,\
			CComBSTR(strStockCode),fSell,nSellCount,&nReqID);
#else
		/// 同步提交
		EZMExchangeType eExchangeType = EXCHANGETYPE_UNKNOWN;
		ITradeRecordPtr spiSell = NULL;
		m_spiTrade[0]->SyncCommitOrder(VARIANT_TRUE,STOCKORDERTYPE_SALE,ORDERPRICETYPE_LIMIT,\
			CComBSTR(strStockCode),fSell,nSellCount,eExchangeType,&spiSell);
		if(NULL == spiSell)
		{
			/// 提交失败，获取错误描述
			EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
			CComBSTR bstrErrDesc;
			m_spiTrade[0]->get_LastErrType(&nErrCode);
			m_spiTrade[0]->get_LastErrDesc(&bstrErrDesc);
			this->MessageBox(bstrErrDesc.m_str);
			bstrErrDesc.Empty();
			continue;
		}
		ULONG nSellRecord = 0;
		spiSell->get_RecordCount(&nSellRecord);
		if(nSellRecord)
		{
			CComBSTR bstrJsonVal;
			spiSell->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();

			/// 获得交易服务器上返回的委托回报编号ID，通过此ID可以进行撤单操作
			spiSell->GetValueByName(0,CComBSTR(L"委托编号"),&varVal);
			varVal.ChangeType(VT_BSTR);
			ITradeRecordPtr spiCancle = NULL;
			m_spiTrade[0]->CancelOrder(eExchangeType,varVal.bstrVal,&spiCancle);
			/// 通过提取spiCancle里的返回值得知撤单结果
			spiCancle = NULL;
			varVal.Clear();
		}
		spiSell = NULL;
#endif
	}
	spiRecord->Clear();
	spiRecord = NULL;
#ifndef ZM_TDXSERVER_SYNC
	VARIANT_BOOL bCommitRet = VARIANT_FALSE;
	m_spiTrade[0]->CommitOrder(nTradeID,VARIANT_TRUE,RUNPRITYPE_NORMAL,&bCommitRet);
#endif
	return 0;
}

LRESULT CMainDlg::OnBnClickedIPO(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	m_spiTrade[0]->BuyIPO(NULL);
	return 0;
}

LRESULT CMainDlg::OnBnClickedMarket(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	CMarketDlg MarketDlag;
	int nRet = (int)MarketDlag.DoModal();
	return 0;
}

LRESULT CMainDlg::OnBnClickedMarketEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	CMarketExDlg MarketDlag;
	int nRet = (int)MarketDlag.DoModal();

	return 0;
}

LRESULT CMainDlg::OnBnClickedMarketL2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	CMarketLevel2Dlg MarketDlag;
	int nRet = (int)MarketDlag.DoModal();

	return 0;
}
