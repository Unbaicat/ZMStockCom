// MarketLevel2Dlg.h : interface of the CMarketLevel2Dlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WTL/atlddx.h"
#include "StockComEvent.h"
#include "MarketL2Event.h"

class CMarketLevel2Dlg : public CAxDialogImpl<CMarketLevel2Dlg>,public CWinDataExchange<CMarketLevel2Dlg>
{
protected:

	/// 从后往前
	BOOL								m_bSort;

	/// Level2行情服务接口
	IMarketLevel2Ptr					m_spiMarket;
	/// 行情接口事件COOKIE数组
	DWORD								m_nMarketEventCookie;
	/// 行情接口事件对象数组
	CComObject<CMarketL2Event>*			m_spiMarketEvent;

	/// 释放组件
	void ReleaseCom();

	/// 建立事件通知
	HRESULT AdviseMarketCom();
	/// 断开事件通知
	void UnAdviseMarketCom();

public:
	enum { IDD = IDD_DIALOG_LEVEL2MARKET };

	BEGIN_DDX_MAP(CAddAuthDlg)
		DDX_CHECK(IDC_CHECK_SORT,m_bSort);
    END_DDX_MAP()

	BEGIN_MSG_MAP(CMarketLevel2Dlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_TRADEEVENT_INITRETURN, OnInitReturn)
		MESSAGE_HANDLER(WM_TRADEEVENT_LOGINRETURN, OnLoginReturn)
		MESSAGE_HANDLER(WM_TRADEEVENT_SERVERERRVER, OnServerError)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKQUOTE, OnStockQuote)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKQUOTE10, OnStockQuote10)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKMINUTE, OnStockMintue)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKTRANS, OnStockTrans)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKBARS, OnStockBars)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKDETAILTRANS, OnDetailTrans)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKDETAILORDER, OnDetailOrder)
		MESSAGE_HANDLER(WM_TRADEEVENT_STOCKORDERLIST, OnOrderList)

		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_BUTTON_CONNECT, BN_CLICKED, OnBnClickedConnect)
		COMMAND_HANDLER(IDC_DISCONNECT, BN_CLICKED, OnBnClickedDisConnect)
		COMMAND_HANDLER(IDC_BUTTON_F10, BN_CLICKED, OnBnClickedF10)
		COMMAND_HANDLER(IDC_BUTTON_GETF, BN_CLICKED, OnBnClickedGetf)
		COMMAND_HANDLER(IDC_BUTTON_XDATA, BN_CLICKED, OnBnClickedXdata)
		COMMAND_HANDLER(IDC_BUTTON_KDATA, BN_CLICKED, OnBnClickedKdata)
		COMMAND_HANDLER(IDC_BUTTON_WK, BN_CLICKED, OnBnClickedWk)
		COMMAND_HANDLER(IDC_BUTTON_MKX, BN_CLICKED, OnBnClickedMkx)
		COMMAND_HANDLER(IDC_BUTTON_HKD, BN_CLICKED, OnBnClickedHkd)
		COMMAND_HANDLER(IDC_BUTTON_FIVEMK, BN_CLICKED, OnBnClickedFivemk)
		COMMAND_HANDLER(IDC_BUTTON_MK, BN_CLICKED, OnBnClickedMk)
		COMMAND_HANDLER(IDC_BUTTON_FST, BN_CLICKED, OnBnClickedFst)
		COMMAND_HANDLER(IDC_BUTTON_HMD, BN_CLICKED, OnBnClickedHmd)
		COMMAND_HANDLER(IDC_BUTTON_QUOTE10, BN_CLICKED, OnBnClickedQuote10)
		COMMAND_HANDLER(IDC_BUTTON_FSCJ, BN_CLICKED, OnBnClickedFscj)
		COMMAND_HANDLER(IDC_BUTTON_HFST, BN_CLICKED, OnBnClickedHfst)
		COMMAND_HANDLER(IDC_BUTTON_DETAILTRADE, BN_CLICKED, OnBnClickedDetailTrade)
		COMMAND_HANDLER(IDC_BUTTON_FIVEMARKET, BN_CLICKED, OnBnClickedQuote)
		COMMAND_HANDLER(IDC_BUTTON_DETAILTASK, BN_CLICKED, OnBnClickedDetailTask)
		COMMAND_HANDLER(IDC_BUTTON_DUILIE, BN_CLICKED, OnBnClickedDuiLi)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnInitReturn(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLoginReturn(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnServerError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStockQuote(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStockMintue(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStockTrans(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStockBars(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDetailTrans(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDetailOrder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOrderList(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStockQuote10(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedF10(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedGetf(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedXdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedKdata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedWk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedMkx(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedHkd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFivemk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedMk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedHmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedQuote10(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedFscj(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedHfst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedDetailTrade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedQuote(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedDetailTask(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedDuiLi(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
