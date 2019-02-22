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
/// �����Ƿ�ͬ����������
#define	ZM_TDXSERVER_SYNC
#endif

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
		if('.' == c)
			break;
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

void CMainDlg::AdviseTradeClient()
{
	if(NULL != m_spiTradeClientEvent)
		return;
	m_nTradeEventCookie = NULL;
	this->m_spiTradeClientEvent = new CComObject <CStockComEvent>;
	ATLASSERT(this->m_spiTradeClientEvent);
	if(NULL == this->m_spiTradeClientEvent)
		return;	
	/// ���ûص������������¼�����ʱƥ�����ĸ�����������
	m_spiTradeClientEvent->SetCallPara(this->m_hWnd);
	//�����¼�����
	HRESULT hRet = AtlAdvise(m_spiTrade,this->m_spiTradeClientEvent->GetUnknown(),
		DIID__ITradeEvents,&this->m_nTradeEventCookie);
}

void CMainDlg::UnAdviseTradeClient()
{
	if(0 == m_nTradeEventCookie)
		return;
	AtlUnadvise(m_spiTrade,DIID__ITradeEvents,m_nTradeEventCookie);
	m_nTradeEventCookie = 0;
	m_spiTradeClientEvent = NULL;
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
	int nItemIndex = m_wndTradeList.AddString(L"�ʽ�");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CAPITAL);
	nItemIndex = m_wndTradeList.AddString(L"�ɷ�");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_STOCK);
	nItemIndex = m_wndTradeList.AddString(L"����ί��");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYORDER);
	nItemIndex = m_wndTradeList.AddString(L"���ճɽ�");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYTRADE);
	nItemIndex = m_wndTradeList.AddString(L"���տɳ�ί��");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_TODAYCANREVOKE);
	nItemIndex = m_wndTradeList.AddString(L"�ɶ�����");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_SHAREHOLDERCODE);
	nItemIndex = m_wndTradeList.AddString(L"�������");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BALANCEOFFINANCING);
	nItemIndex = m_wndTradeList.AddString(L"��ȯ���");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BORROWFUNDLEFT);
	nItemIndex = m_wndTradeList.AddString(L"����֤ȯ");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANBORROWFUND);
	nItemIndex = m_wndTradeList.AddString(L"���깺�¹�");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANNEWSTOCK);
	nItemIndex = m_wndTradeList.AddString(L"�¹��깺���");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_CANNEWSTOCKLIMIT);
	nItemIndex = m_wndTradeList.AddString(L"���");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_ASSIGNNUMBER);
	nItemIndex = m_wndTradeList.AddString(L"��ǩ");
	m_wndTradeList.SetItemData(nItemIndex,STOCKQUERYTYPE_BALLOT);
	m_wndTradeList.SetCurSel(0);

	m_wndHistoryList.Attach(GetDlgItem(IDC_COMBO_HISLIST).m_hWnd);
	nItemIndex = m_wndHistoryList.AddString(L"��ʷί��");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_TRUST);
	nItemIndex = m_wndHistoryList.AddString(L"��ʷ�ɽ�");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_TRADE);
	nItemIndex = m_wndHistoryList.AddString(L"�ʽ���ˮ");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_CAPITALLIST);
	nItemIndex = m_wndHistoryList.AddString(L"���");
	m_wndHistoryList.SetItemData(nItemIndex,HISORDERTYPE_DELIVERYORDER);
	m_wndHistoryList.SetCurSel(0);

	m_wndAccountType.Attach(GetDlgItem(IDC_COMBO_ACCOUNTTYPE).m_hWnd);
	nItemIndex = m_wndAccountType.AddString(L"�ʽ��˺�");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_CAPITAL);
	nItemIndex = m_wndAccountType.AddString(L"�ͻ���");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_CUSTOMER);
	nItemIndex = m_wndAccountType.AddString(L"����ɶ�");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_SZA);
	nItemIndex = m_wndAccountType.AddString(L"�����ɶ�");
	m_wndAccountType.SetItemData(nItemIndex,LOGINIACCOUNTTYPE_SHA);
	m_wndAccountType.SetCurSel(0);

	m_wndBrokerType.Attach(GetDlgItem(IDC_COMBO_BROKETYPE).m_hWnd);
	nItemIndex = m_wndBrokerType.AddString(L"ģ�����");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_MNCS);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��һ��ҵ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DYCY);
	nItemIndex = m_wndBrokerType.AddString(L"��ݸ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DGZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GXZQ);
	//nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	//m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_LHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"ƽ��֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_PAZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�㷢֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��֤ͨȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��ҵ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZSZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��Ԫ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JYDT);
	nItemIndex = m_wndBrokerType.AddString(L"���Ž�Ͷ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZXJT);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YNHT);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��Դ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��̩����");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GTJA);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_AXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�Ƹ�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"���֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"Ӣ��֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�°�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DBZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�Ͼ�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_NJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�Ϻ�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HBZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_AJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��̩֤ȯ(��³֤ȯ)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_QLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��������");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZYGJ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_MZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"���֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�״�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��Ԫ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ(���ϴ���)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_FZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_BHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XNZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��ʱ��֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XSDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��Ͷ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HRZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�����Դ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SYWG);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��̩֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HUATZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JLDB);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_MSZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��֤ͬȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SXDT);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ(�ڳ�֤ȯ)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZCZQ);
	nItemIndex = m_wndBrokerType.AddString(L"���ǹ���֤ȯ(����֤ȯ)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XMZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��̩֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_YTZQ);
	//nItemIndex = m_wndBrokerType.AddString(L"�к�֤ȯ(����֤ȯ)");
	//m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JLDT);
	nItemIndex = m_wndBrokerType.AddString(L"���֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_WHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GKZQ);
	//nItemIndex = m_wndBrokerType.AddString(L"�ൺ֤ȯ(������ͨ)");
	//m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZXWT);
	nItemIndex = m_wndBrokerType.AddString(L"�Ŵ�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XDZQ);
	nItemIndex = m_wndBrokerType.AddString(L"���֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_WKZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"̫ƽ��֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_TPYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XBZQ);
	nItemIndex = m_wndBrokerType.AddString(L"ɽ��֤ȯ(��ͨ����)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HAHX);
	nItemIndex = m_wndBrokerType.AddString(L"��ʢ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GSZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SCHX);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_WLZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_LNZT);
	nItemIndex = m_wndBrokerType.AddString(L"��̩֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_NMHT);
	nItemIndex = m_wndBrokerType.AddString(L"���֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_TFZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_GSHL);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ(����֤ȯ)");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_RXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�к�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZHZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��֤ͨȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��֤ͨȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HTZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_DWZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZJZS);
	nItemIndex = m_wndBrokerType.AddString(L"��Ѷ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_LXZQ);
	nItemIndex = m_wndBrokerType.AddString(L"�Ϻ�����");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SHHX);
	nItemIndex = m_wndBrokerType.AddString(L"�����Ƹ�");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_XZTX);
	nItemIndex = m_wndBrokerType.AddString(L"��ԭ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZYZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_BJGD);
	nItemIndex = m_wndBrokerType.AddString(L"�н�֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_SXZY);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_HJZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_JZZQ);
	nItemIndex = m_wndBrokerType.AddString(L"����֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_CCAIZQ);
	nItemIndex = m_wndBrokerType.AddString(L"��ɽ֤ȯ");
	m_wndBrokerType.SetItemData(nItemIndex,BROKERTYPE_ZHSZQ);

	m_wndBrokerType.SetCurSel(0);

	m_bRZRQ = FALSE;
	m_spiTrade = NULL;
	m_nTradeEventCookie= 0;
	m_spiTradeClientEvent = NULL;

	DoDataExchange(false,IDC_CHECK_RZRQ);

	/// Ĭ������Ϊͨ����ģ�������IP��ʵ��ʹ��ʱ������Ϊ��ȯ�̵Ľ��׷�����IP
	this->GetDlgItem(IDC_EDIT_TRADESERVERADDR).SetWindowText(L"mock.tdx.com.cn");
	this->GetDlgItem(IDC_EDIT_TRADESERVERPORT).SetWindowText(L"7708");
	this->GetDlgItem(IDC_EDIT_YYBID).SetWindowText(L"9000");
	/// ȯ�̸��㿪ͨ�ĵ�¼�˺�ID
	this->GetDlgItem(IDC_EDIT_LOGINACCOUNTNO).SetWindowText(L"");
	/// ȯ�̸��㿪ͨ�Ľ����˺�ID
	this->GetDlgItem(IDC_EDIT_TRADEACCOUNT).SetWindowText(L"");

	this->GetDlgItem(IDC_EDIT_TRADEPASSWORD).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_COMMPASSWORD).SetWindowText(L"");
	this->GetDlgItem(IDC_EDIT_VERSION).SetWindowText(L"8.09");

	this->GetDlgItem(IDC_EDIT_STOCKCODE).SetWindowText(L"000001");
	GetDlgItem(IDC_EDIT_COREVER).SetWindowText(L"0");
	return TRUE;
}

LRESULT CMainDlg::OnInitReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piTrade = (IDispatch* )lParam;
	IStockTradePtr spiTrade = NULL;
	if(NULL != piTrade)
		piTrade->QueryInterface(IID_IStockTrade,(LPVOID *)&spiTrade);
	ATLASSERT(spiTrade);
	if(NULL == spiTrade)
		return 0;/// �����׽ӿ�
	if(wParam)
	{
#ifdef _DEBUG
		/// �¼�֪ͨ�о������ⵯ��MessageBox�����������������Ƚ����ڵ��Գ��򷽱�
		if(m_spiTrade == spiTrade)
		{
			this->MessageBox(L"����1��ʼ���ӿڳɹ���");
		}
#endif
	}
	else
	{
		/// ����ʧ�ܣ�������������
		CComBSTR bstrErrInfo;
		spiTrade->get_LastErrDesc(&bstrErrInfo);
		/// �¼�֪ͨ�о������ⵯ��MessageBox�����������������Ƚ����ڵ��Գ��򷽱�
		this->MessageBox(bstrErrInfo.m_str);
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMainDlg::OnLoginReturn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piTrade = (IDispatch* )lParam;
	ATLASSERT(piTrade);
	IStockTradePtr spiTrade = NULL;
	if(NULL != piTrade)
		piTrade->QueryInterface(IID_IStockTrade,(LPVOID *)&spiTrade);
	ATLASSERT(spiTrade);
	if(NULL == spiTrade)
		return 0;/// �����׽ӿ�
	if(wParam)
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		HRESULT hRet = spiTrade->get_ConnectValid(&bConnectValid);
		ULONG nTradeID = 0;
		hRet = spiTrade->get_CurTradeID(&nTradeID);
		/// �¼�֪ͨ�о������ⵯ��MessageBox�����������������Ƚ����ڵ��Գ��򷽱�
		if(VARIANT_TRUE == bConnectValid)
			this->MessageBox(L"�첽��¼�������ɹ������濪ʼ��ȡ�ɶ�������Ϣ��");
		ITradeRecordPtr spiRecord = NULL;
		/// ��ùɶ��˺���Ϣ
		spiTrade->get_ShareHolderCode(&spiRecord);
		if(NULL != spiRecord)
		{
			/// ��ȡJSON�������������
			CComBSTR bstrJsonVal;
			spiRecord->GetJsonString(&bstrJsonVal);
			this->MessageBox(bstrJsonVal.m_str);
			bstrJsonVal.Empty();
			/// �ɶ�������󣬵ײ�Ỻ�����ݱ��ã������뾡����Ҫ����Clear��գ�����Ӱ��ײ㹦��
//			spiRecord->Clear();
			spiRecord = NULL;
		}
#ifdef ZM_TDXSERVER_SYNC
		/// ͬ������ĳֻ��Ʊ��ʵʱ5������
		spiRecord = NULL;
		this->MessageBox(L"���濪ʼ��ȡָ����Ʊ��ʵʱ5�����飡");
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
		/// �첽�����ֻ��Ʊ��ʵʱ5�����飬��ָ�����ȼ��ͷ��ص��������кš�nReqID�����¼��ص��д���
		ULONG nReqID = 0;
		spiTrade->GetStockQuotes(nTradeID,CComBSTR(L"000001;000002;600001"),RUNPRITYPE_ABOVE_NORMAL,&nReqID);
#endif
	}
	else
	{
		/// ����ʧ�ܣ�������������
		CComBSTR bstrErrInfo;
		spiTrade->get_LastErrDesc(&bstrErrInfo);
#ifdef _DEBUG
		/// �¼�֪ͨ�о������ⵯ��MessageBox�����������������Ƚ����ڵ��Գ��򷽱�
		this->MessageBox(bstrErrInfo.m_str);
#endif
		bstrErrInfo.Empty();
	}

	return 0;
}

LRESULT CMainDlg::OnOrderOK(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piRecordInfo = (IDispatch*)lParam;
	if(NULL == piRecordInfo || NULL == m_spiTrade)
		return 0;/// �����Ѿ��ͷ�
	ITradeRecordPtr spiRecord = NULL;
	piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&spiRecord);
	if(NULL == spiRecord)
		return 0;
	ULONG nRecordCount = 0,nFieldCount = 0;
	spiRecord->get_FieldCount(&nFieldCount);
	spiRecord->get_RecordCount(&nRecordCount);
	if(!nFieldCount || !nRecordCount)
		return 0;/// �쳣����
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();
	/// ��ȡ��ί����ţ�����ҵ��������Ҫ��¼����ReqID�ͳɹ��ر�OrderID�Ķ�Ӧ��ϵ�����ں�����г���
	CComVariant varOrderID;
	spiRecord->GetValueByName(0,CComBSTR(L"ί�б��"),&varOrderID);
	varOrderID.ChangeType(VT_BSTR);
	spiRecord->Clear();
	spiRecord = NULL;
	/// ִ��ȡ��ί�У���Ҫ�����г����ͣ���ί�гɹ�ʱ֪ͨ���أ�Ҳ�����Լ����㣬�Ϻ�������2�����ڽ�����Ϊ1
	m_spiTrade->CancelOrder((EZMExchangeType)wParam,varOrderID.bstrVal,&spiRecord);
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
	if(NULL == m_spiTradeClientEvent || NULL == m_spiTrade)
		return 0;/// �����Ѿ��ͷ�
	return 0;
}

LRESULT CMainDlg::OnStockQuote(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatch* piRecordInfo = (IDispatch*)lParam;
	if(NULL == piRecordInfo || NULL == m_spiTrade)
		return 0;/// �����Ѿ��ͷ�
	ITradeRecordPtr spiRecord = NULL;
	piRecordInfo->QueryInterface(IID_ITradeRecord,(LPVOID *)&spiRecord);
	if(NULL == spiRecord)
		return 0;
	/// ���ݽӿڷ�����������
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	HRESULT hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	/// ��ȡJSON�������������
	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

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

LRESULT CMainDlg::OnServerError(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	/// ����������������Ҫ�Ͽ����µ�¼
	if(NULL != m_spiTrade)
		m_spiTrade->LogOut(0);
	/// �����µ�¼...
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

void CMainDlg::ReleaseCom()
{
	/// �ͷ�COM�������
	if(NULL != m_spiTrade)
	{
		m_spiTrade->LogOut(0);
		/// �Ͽ��¼�����
		UnAdviseTradeClient();
		m_spiTrade = NULL;
	}
}

LRESULT CMainDlg::OnBnClickedUnInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL != m_spiTrade)
	{
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		m_spiTrade->get_ConnectValid(&bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			this->MessageBox(L"��û�н����뽻�׷����������ӣ�");
			return 0;
		}
		m_spiTrade->LogOut(0);
	}

	this->MessageBox(L"�Ͽ�ͨѶ�ɹ���");
	return 0;
}

LRESULT CMainDlg::OnBnClickedInit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	if(NULL == m_spiTrade)
	{
		/// ����COM����ǰ����ע�ᣬ����ԱȨ��ִ��ZMStockCom.exe /RegServer
		/// ж��ִ��ZMStockCom.exe /UnregServer
		/// �������ʧ�ܣ����ȼ���Ƿ�ע��ɹ��������������Ҫ������DLL�Ƿ񶼺ͱ�������һ��Ŀ¼
		HRESULT hRet = m_spiTrade.CreateInstance(__uuidof(StockTrade));
		ATLASSERT(m_spiTrade);
		if(NULL != m_spiTrade)
		{
			/// �����¼�����
			AdviseTradeClient();
			/// �����ں˰汾��Ĭ��0��������Ϊ���ں�1
			CComBSTR bstrCoreType;
			this->GetDlgItem(IDC_EDIT_COREVER).GetWindowText(&bstrCoreType);
			m_spiTrade->put_TradeType((USHORT)StrToNum(bstrCoreType.m_str));
			m_spiTrade->put_CurServerPort((USHORT)StrToNum(bstrCoreType.m_str));
			bstrCoreType.Empty();
			/// ���õ�����־���
			hRet = m_spiTrade->put_EnableLog(VARIANT_TRUE);
		}
		/// ����ʵ�ʵ���Ȩ�ļ�ȫ·��
//		m_spiTrade->put_AuthFile(L"TradeAuth.zmd");
	}
	if(NULL == m_spiTrade)
	{
		this->MessageBox(L"����COM�������û�д����ɹ���");
		return 0;
	}
	/// ��ʼ��ʹ�û���������ʵ��ֻ��Ҫ����һ�Σ����ݿͻ��˰汾�ţ����������(Ĭ��Ϊ1)����ú����õķ���������ƥ��
	/// Ϊ���ִ��Ч�ʣ�ʵ��Ϊ�첽��������Ҫ�����¼�֪ͨ�õ���ʼ�����
	CComBSTR bstrVersion;
	this->GetDlgItem(IDC_EDIT_VERSION).GetWindowText(&bstrVersion);
	ATLASSERT(bstrVersion.Length());
	m_spiTrade->Init(bstrVersion,1);
	/// �����Ƿ�������ȯ��¼
	DoDataExchange(true,IDC_CHECK_RZRQ);
	if(m_bRZRQ)
		m_spiTrade->put_CreditAccount(VARIANT_TRUE);
	else
		m_spiTrade->put_CreditAccount(VARIANT_FALSE);

	/// ��ȡ��ʼ������
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
		this->MessageBox(L"��û�����ý������룬�޷���¼��������");
		return 0;
	}
	/// ����ȯ�����ͺ��˺�����
	EZMLoginAccountType eAccountType = LOGINIACCOUNTTYPE_CAPITAL;
	if(m_wndAccountType.GetCurSel())
		eAccountType = (EZMLoginAccountType )m_wndAccountType.GetItemData(m_wndAccountType.GetCurSel());
	EZMBrokerType eBrokerType = BROKERTYPE_MNCS;
	if(m_wndBrokerType.GetCurSel())
		eBrokerType = (EZMBrokerType)m_wndBrokerType.GetItemData(m_wndBrokerType.GetCurSel());
	m_spiTrade->put_BrokerType(eBrokerType);
	m_spiTrade->put_AccountType(eAccountType);

	//m_spiTrade->put_CreditAccount(VARIANT_TRUE);/// �����Ƿ������˺�
	/// ���óɽ��Զ��ر���ʱ��1000���룬��Ϊ0��ʾ������
	m_spiTrade->put_ReportSuccessTimer(1000);

	/// ���ý��׷�������ַ
	m_spiTrade->put_CurServerHost(bstrServerAddr);
	/// ���ý��׷������˿�
	m_spiTrade->put_CurServerPort((USHORT)StrToNum(bstrServerPort.m_str));

	/// ����Ӫҵ��ID
	m_spiTrade->put_DepartmentID((USHORT)StrToNum(bstrYybID.m_str));
	/// ���õ�¼�˻�
	m_spiTrade->put_LoginID(bstrLoginID);
	if(bstrTradeAccount.Length())
		m_spiTrade->put_TradeAccount(bstrTradeAccount);/// ������ʽ��˺ŵ�¼����ֵһ��͵�¼�˺�һ�������Բ�����
	/// ���ý�������
	m_spiTrade->put_TradePassword(bstrTradePassword);
	/// ����ͨ������
	if(bstrCommPassword.Length())
		m_spiTrade->put_CommPassword(bstrCommPassword);/// ���Բ����ã�����ȯ����Ҫ���óɺͽ�������һ���ģ���̩֤ȯ��Ҫ����ͨѶ����

	VARIANT_BOOL bRetFlag = VARIANT_FALSE;
#ifndef ZM_TDXSERVER_SYNC
	///  �첽ִ�е�¼���׷���������¼���ͨ���¼�֪ͨ���
	HRESULT hRet = m_spiTrade->LogIn(VARIANT_TRUE,&bRetFlag);
#else
	///  ͬ��ִ�е�¼���׷�������ֱ�����ؽ��
	HRESULT hRet = m_spiTrade->LogIn(VARIANT_FALSE,&bRetFlag);
	if(VARIANT_FALSE == bRetFlag)
	{
		/// ��ȡ������Ϣ
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrType(&nErrCode);
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
		if(bstrErrDesc.Length())
			this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return 0;
	}
	this->MessageBox(L"ͬ����¼���׷������ɹ���");
#endif
	return 1;
}

ITradeRecordPtr CMainDlg::GetCategoryData(EZMStockQueryType eCategoryType)
{
	ITradeRecordPtr spiRecord = NULL;
	ULONG nTradeID = 0;
	HRESULT hRet = m_spiTrade->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"���ȳ�ʼ����¼��");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// һ���ǳ�ʼ���ɹ���ʹ��һ��ʱ�䣬���ύ���׺Ͳ�ѯ����ǰ������Щ����ʧ��ʱ�ż���������Ƿ�����
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// ����ִ��ͬ����¼����ԭ�����õ���ز������Զ��ٴ�ʹ��
			hRet = m_spiTrade->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// ͨ�Ų�����
	hRet = m_spiTrade->QueryTradeData(nTradeID,eCategoryType,&spiRecord);
	if(NULL == spiRecord)
	{
		/// ��ȡ������Ϣ
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrType(&nErrCode);
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
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
		if(!bstrJsonVal.Length())
			spiRecord->GetTitleJson(&bstrJsonVal);
		this->MessageBox(bstrJsonVal.m_str);
		bstrJsonVal.Empty();

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
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

ITradeRecordPtr CMainDlg::GetHisData(EZMHisOrderType eCategoryType)
{
	ITradeRecordPtr spiRecord = NULL;
	ULONG nTradeID = 0;
	HRESULT hRet = m_spiTrade->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"���ȳ�ʼ����¼��");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// һ���ǳ�ʼ���ɹ���ʹ��һ��ʱ�䣬���ύ���׺Ͳ�ѯ����ǰ������Щ����ʧ��ʱ�ż���������Ƿ�����
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// ����ִ��ͬ����¼����ԭ�����õ���ز������Զ��ٴ�ʹ��
			hRet = m_spiTrade->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// ͨ�Ų�����
	/// ָ��ʱ�䷶Χ
	COleDateTime curTime = COleDateTime::GetCurrentTime();
	CString strBeginTime,strEndTime;
	strEndTime.Format(L"%d%02d%02d",curTime.GetYear(),curTime.GetMonth(),curTime.GetDay());
	curTime -= COleDateTimeSpan(15,0,0,0);
	strBeginTime.Format(L"%d%02d%02d",curTime.GetYear(),curTime.GetMonth(),curTime.GetDay());
	hRet = m_spiTrade->QueryHisTradeData(nTradeID,eCategoryType,CComBSTR(strBeginTime),CComBSTR(strEndTime),&spiRecord);
	if(NULL == spiRecord)
	{
		/// ��ȡ������Ϣ
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrType(&nErrCode);
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return spiRecord;
	}
	ULONG nFieldCount = 0,nRecordCount = 0,nIndex = 0;
	hRet = spiRecord->get_FieldCount(&nFieldCount);
	hRet = spiRecord->get_RecordCount(&nRecordCount);

	CComBSTR bstrJsonVal;
	spiRecord->GetJsonString(&bstrJsonVal);
	if(!bstrJsonVal.Length())
	{
		spiRecord->GetTitleJson(&bstrJsonVal);
		this->MessageBox(bstrJsonVal.m_str);
		bstrJsonVal.Empty();
		return spiRecord;
	}
	this->MessageBox(bstrJsonVal.m_str);
	bstrJsonVal.Empty();

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
	spiRecord->Clear();
	spiRecord = NULL;
	return 0;
}

ITradeRecordPtr CMainDlg::GetStockMarket(const CString& strStockCode)
{
	ITradeRecordPtr spiRecord = NULL;
	if(NULL == m_spiTrade)
	{
		this->MessageBox(L"�����δ�����ɹ���");
		return spiRecord;
	}
	ULONG nTradeID = 0;
	HRESULT hRet = m_spiTrade->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"���ȳ�ʼ����¼��");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// һ���ǳ�ʼ���ɹ���ʹ��һ��ʱ�䣬���ύ���׺Ͳ�ѯ����ǰ������Щ����ʧ��ʱ�ż���������Ƿ�����
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// ����ִ��ͬ����¼����ԭ�����õ���ز������Զ��ٴ�ʹ��
			hRet = m_spiTrade->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
	{
		this->MessageBox(L"ͨ�Ų�������");
		return spiRecord;/// ͨ�Ų�����
	}
	hRet = m_spiTrade->GetStockQuote(nTradeID,CComBSTR(strStockCode),&spiRecord);
	if(NULL == spiRecord)
	{
		/// ��ȡ������Ϣ
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrType(&nErrCode);
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
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
	/// ���нӿ���Ҫ������쳣�����COM���������̱�����������ӿڹ��ܵ�ʱ�򣬻᷵�ش�����0x800706BA����˼�ǡ�RPC �����������á�
	/// ��Ҫ�ͷŵ�ǰʹ�õĽ��׽ӿڶ�������´��� HRESULT hRet = m_spiTrade.CreateInstance(__uuidof(Trade)) ������������

	CComVariant varVal;
	/// ȡ��һ�۸���������
	HRESULT hRet(E_FAIL);
	EZMExchangeType eExchangeType = EXCHANGETYPE_UNKNOWN;
	/// ������Ż�ȡ���ܴ��ڵ����⣺��Ҫ����ȯ�̣�ȯ�̲�ͬ�����ܷ��ص��ֶ�˳��һ��
//	hRet = spiRecord->GetValue(0,16,&varVal);
	/// �������ֶα����ȡ
	hRet = spiRecord->GetValueByName(0,CComBSTR(L"�����"),&varVal);
	ULONG nReqID1 = 0;/// ����Ψһ��ʶ��ί��ID����������¼���ʽ֪ͨ������������Ҫ����ÿ��ί�еĽ��֪ͨ����Ҫ��¼���������¼�֪ͨ��ID����ƥ��
	m_spiTrade->AddOrder(STOCKORDERTYPE_BUY,ORDERPRICETYPE_LIMIT,bstrStockCode,varVal.fltVal,500,eExchangeType,&nReqID1);
	varVal.Clear();
	ATLASSERT(nReqID1);
	if(!nReqID1)
	{
		/// ���ί��ʧ�ܣ���ѯ����ԭ��
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
	}
	/// ȡ��ǰ������
//	hRet = spiRecord->GetValue(0,5,&varVal);
	hRet = spiRecord->GetValueByName(0,CComBSTR(L"���ļ�"),&varVal);
	ULONG nReqID2 = 0;/// ����Ψһ��ʶ��ί��ID����������¼���ʽ֪ͨ���
	hRet = m_spiTrade->AddOrder(STOCKORDERTYPE_BUY,ORDERPRICETYPE_LIMIT,bstrStockCode,varVal.fltVal,500,eExchangeType,&nReqID2);
	bstrStockCode.Empty();
	ATLASSERT(nReqID1);
	if(!nReqID2)
	{
		/// ���ί��ʧ�ܣ���ѯ����ԭ��
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
	}

	/// �ύί�У���һ���������ΪVARIANT_TRUE,�ײ���Զ���鵱ǰͨ��״̬����������ʱ����Զ���¼һ�����ύί��
	VARIANT_BOOL bCommitRet = VARIANT_FALSE;
	/// �������ύ�����ȼ�
	ULONG nTradeID = 0;
	m_spiTrade->get_CurTradeID(&nTradeID);
	hRet = m_spiTrade->CommitOrder(nTradeID,VARIANT_FALSE,RUNPRITYPE_NORMAL,&bCommitRet);
	spiRecord->Clear();
	spiRecord = NULL;
	if(VARIANT_FALSE == bCommitRet)
	{
		/// ��ȡ������Ϣ
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrType(&nErrCode);
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
		this->MessageBox(bstrErrDesc.m_str);
		bstrErrDesc.Empty();
		return 0;
	}
	if(NULL != spiRecord)
	{
		/// ���ί�лر�
		spiRecord->Clear();
		spiRecord = NULL;
	}
	return 0;
}

LRESULT CMainDlg::OnBnClickedSell(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	ITradeRecordPtr spiRecord = NULL;
	ULONG nTradeID = 0;
	HRESULT hRet = m_spiTrade->get_CurTradeID(&nTradeID);
	if(!nTradeID)
	{
		this->MessageBox(L"���ȳ�ʼ����¼��");
		return spiRecord;
	}
	if(nTradeID)
	{
		/// һ���ǳ�ʼ���ɹ���ʹ��һ��ʱ�䣬���ύ���׺Ͳ�ѯ����ǰ������Щ����ʧ��ʱ�ż���������Ƿ�����
		VARIANT_BOOL bConnectValid = VARIANT_FALSE;
		hRet = m_spiTrade->get_ConnectValid(&bConnectValid);
		ATLASSERT(VARIANT_TRUE == bConnectValid);
		if(VARIANT_FALSE == bConnectValid)
		{
			VARIANT_BOOL bRetFlag = VARIANT_FALSE;
			/// ����ִ��ͬ����¼����ԭ�����õ���ز������Զ��ٴ�ʹ��
			hRet = m_spiTrade->LogIn(VARIANT_FALSE,&bRetFlag);
		}
	}
	if(FAILED(hRet))
		return spiRecord;/// ͨ�Ų�����
	/// �����Ʊ�б�
	hRet = m_spiTrade->QueryTradeData(nTradeID,STOCKQUERYTYPE_STOCK,&spiRecord);
	if(NULL == spiRecord)
	{
		/// ��ȡ������Ϣ
		EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
		CComBSTR bstrErrDesc;
		m_spiTrade->get_LastErrType(&nErrCode);
		m_spiTrade->get_LastErrDesc(&bstrErrDesc);
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

	/// ���ҿ������Ĺ�Ʊ
	CComVariant varVal;
	for(nIndex = 0;nIndex < nRecordCount;nIndex++)
	{
		spiRecord->GetValueByName(nIndex,CComBSTR(L"���ùɷ�"),&varVal);
		varVal.ChangeType(VT_I2);
		if(!varVal.iVal)
			continue;/// �޿��ã��޷�������
		int nSellCount = varVal.iVal;
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"֤ȯ����"),&varVal);
		CString strStockName(varVal.bstrVal);
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"֤ȯ����"),&varVal);
		CString strStockCode(varVal.bstrVal);
		varVal.Clear();
		spiRecord->GetValueByName(nIndex,CComBSTR(L"�ɶ�����"),&varVal);
		CString strHolderCode(varVal.bstrVal);
		varVal.Clear();

		/// ��ȡ��ǰ����۸�
		ITradeRecordPtr spiQuote = NULL;
		m_spiTrade->GetStockQuote(nTradeID,CComBSTR(strStockCode),&spiQuote);
		if(NULL == spiQuote)
			continue;
		ULONG nQuoteCount = 0;
		spiQuote->get_RecordCount(&nQuoteCount);
		if(!nQuoteCount)
		{
			/// û�л�ȡ����ǰ����
			spiQuote = NULL;
			continue;
		}
		CComBSTR bstrJsonVal;
		spiQuote->GetJsonString(&bstrJsonVal);
		this->MessageBox(bstrJsonVal.m_str);
		bstrJsonVal.Empty();

		/// ����5������
		spiQuote->GetValueByName(0,CComBSTR(L"�����"),&varVal);
		varVal.ChangeType(VT_R4);
		spiQuote = NULL;
		float fSell = varVal.fltVal;
		varVal.Clear();
		/// �޼��������п��ù�Ʊ
		EZMExchangeType eExchangeType = EXCHANGETYPE_UNKNOWN;
#ifndef ZM_TDXSERVER_SYNC
		/// �첽�ύ�������Ҫ��¼���ص�����ID�����ں��¼�֪ͨ���ί�лر����Ӧ���Ӷ�����֪��ʵ��ί�н��
		ULONG nReqID = 0;
		m_spiTrade->AddOrder(STOCKORDERTYPE_SALE,ORDERPRICETYPE_LIMIT,\
			CComBSTR(strStockCode),fSell,nSellCount,eExchangeType,&nReqID);
#else
		/// ͬ���ύ
		ITradeRecordPtr spiSell = NULL;
		m_spiTrade->SyncCommitOrder(VARIANT_TRUE,STOCKORDERTYPE_SALE,ORDERPRICETYPE_LIMIT,\
			CComBSTR(strStockCode),fSell,nSellCount,eExchangeType,&spiSell);
		if(NULL == spiSell)
		{
			/// �ύʧ�ܣ���ȡ��������
			EZMTradeErrType nErrCode = TRADEERRTYPE_SUCCESS;
			CComBSTR bstrErrDesc;
			m_spiTrade->get_LastErrType(&nErrCode);
			m_spiTrade->get_LastErrDesc(&bstrErrDesc);
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

			/// ��ý��׷������Ϸ��ص�ί�лر����ID��ͨ����ID���Խ��г�������
			spiSell->GetValueByName(0,CComBSTR(L"ί�б��"),&varVal);
			varVal.ChangeType(VT_BSTR);
			ITradeRecordPtr spiCancle = NULL;
			m_spiTrade->CancelOrder(eExchangeType,varVal.bstrVal,&spiCancle);
			/// ͨ����ȡspiCancle��ķ���ֵ��֪�������
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
	m_spiTrade->CommitOrder(nTradeID,VARIANT_TRUE,RUNPRITYPE_NORMAL,&bCommitRet);
#endif
	return 0;
}

LRESULT CMainDlg::OnBnClickedIPO(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_spiTrade->BuyIPO(NULL);
	return 0;
}

LRESULT CMainDlg::OnBnClickedMarket(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMarketDlg MarketDlag;
	int nRet = (int)MarketDlag.DoModal();
	return 0;
}

LRESULT CMainDlg::OnBnClickedMarketEx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMarketExDlg MarketDlag;
	int nRet = (int)MarketDlag.DoModal();

	return 0;
}

LRESULT CMainDlg::OnBnClickedMarketL2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMarketLevel2Dlg MarketDlag;
	int nRet = (int)MarketDlag.DoModal();

	return 0;
}
