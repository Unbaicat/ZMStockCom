/// 定义是否演示同步操作，包括同步登录和同步委托

#define SYNC_OPT

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using ZMStockCom;
/// 如果用DLL版本的，用下面的代码，在添加引用COM组件中选择ZMTradeCom
// using ZMTradeCom;

namespace CSharp_Demo
{
    public partial class TradeDemo : Form
    {
        /// <summary>
        ///  接口对象定义，每账号支持多个服务器的多连接。高级版支持多账号，通过定义多个对象，具体每个对象设置不同的账号和连接服务器地址即可。
        /// </summary>
        StockTradeClass m_StockTrade = null;
        /// <summary>
        ///  定义第二个交易对象，参照进行初始化和登录即可
        /// </summary>
        StockTradeClass m_StockTrade2 = null;
        /// <summary>
        /// 接口对象事件处理对象
        /// </summary>
        TradeEventSink m_TradeEvent = null;

        Dictionary<int, EZMBrokerType> BrokerMap;

        public TradeDemo()
        {
            BrokerMap = new Dictionary<int, EZMBrokerType>();

            InitializeComponent();
        }

        /// <summary>
        /// 程序退出时调用释放资源
        /// </summary>
        private void TradeDemo_Closed(object sender, EventArgs e)
        {
            /// 移除事件挂接
            m_StockTrade.InitEvent -= m_TradeEvent.InitEvent;
            m_StockTrade.LoginEvent -= m_TradeEvent.LoginEvent;
            m_StockTrade.OrderOKEvent -= m_TradeEvent.OrderOKEvent;
            m_StockTrade.OrderErrEvent -= m_TradeEvent.OrderErrEvent;
            m_StockTrade.OrderSuccessEvent -= m_TradeEvent.OrderSuccessEvent;
            m_StockTrade.StockQuoteEvent -= m_TradeEvent.StockQuoteEvent;

            m_StockTrade.ServerErrEvent -= m_TradeEvent.ServerErrEvent;
            m_StockTrade.ServerChangedEvent -= m_TradeEvent.ServerChangedEvent;

            if (null != m_StockTrade)
            {
                m_TradeEvent.ReleaseTrade();
                m_TradeEvent = null;
            }

            m_StockTrade = null;
            m_StockTrade2 = null;
        }

        private void TradeDemo_Load(object sender, EventArgs e)
        {
            /// 默认操作中国平安
            this.STOCKCODE.Text = "000001";
            /// 初始化界面参数，模拟账号
            this.SERVERADDR.Text = "mock.tdx.com.cn";/// 券商的交易服务器IP，这儿默认模拟服务器
            this.SERVERPORT.Text = "7708";
            this.TRADEACCOUNT.Text = "";    ///你的交易账号
            this.LOGINID.Text = "";         ///你的登录账号
            this.LOGINPW.Text = "";
            this.DEPID.Text = "9000";

            int nAddItem = this.ACCOUNTTYPE.Items.Add("模拟");
            nAddItem = this.ACCOUNTTYPE.Items.Add("资金账号");
            nAddItem = this.ACCOUNTTYPE.Items.Add("客户号");

            nAddItem = BROKERTYPE.Items.Add("模拟测试");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_MNCS);
            nAddItem = BROKERTYPE.Items.Add("长江证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_CJZQ);
            nAddItem = BROKERTYPE.Items.Add("第一创业");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DYCY);
            nAddItem = BROKERTYPE.Items.Add("东莞证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DGZQ);
            nAddItem = BROKERTYPE.Items.Add("国信证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GXZQ);
            nAddItem = BROKERTYPE.Items.Add("联合证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_LHZQ);
            nAddItem = BROKERTYPE.Items.Add("平安证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_PAZQ);
            nAddItem = BROKERTYPE.Items.Add("广发证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GFZQ);
            nAddItem = BROKERTYPE.Items.Add("大通证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DTZQ);
            nAddItem = BROKERTYPE.Items.Add("华西证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HXZQ);
            nAddItem = BROKERTYPE.Items.Add("兴业证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XYZQ);
            nAddItem = BROKERTYPE.Items.Add("招商证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZSZQ);
            nAddItem = BROKERTYPE.Items.Add("金元证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_JYDT);
            nAddItem = BROKERTYPE.Items.Add("中信建投");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZXJT);
            nAddItem = BROKERTYPE.Items.Add("红塔证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_YNHT);
            nAddItem = BROKERTYPE.Items.Add("长城证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_CCZQ);
            nAddItem = BROKERTYPE.Items.Add("宏源证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HYZQ);
            nAddItem = BROKERTYPE.Items.Add("国泰君安");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GTJA);
            nAddItem = BROKERTYPE.Items.Add("世纪证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SJZQ);
            nAddItem = BROKERTYPE.Items.Add("安信证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_AXZQ);
            nAddItem = BROKERTYPE.Items.Add("财富证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_CFZQ);
            nAddItem = BROKERTYPE.Items.Add("东兴证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DXZQ);
            nAddItem = BROKERTYPE.Items.Add("银河证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_YHZQ);
            nAddItem = BROKERTYPE.Items.Add("光大证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GDZQ);
            nAddItem = BROKERTYPE.Items.Add("英大证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_YDZQ);
            nAddItem = BROKERTYPE.Items.Add("德邦证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DBZQ);
            nAddItem = BROKERTYPE.Items.Add("南京证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_NJZQ);
            nAddItem = BROKERTYPE.Items.Add("中信证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZXZQ);
            nAddItem = BROKERTYPE.Items.Add("上海证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SHZQ);
            nAddItem = BROKERTYPE.Items.Add("华宝证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HBZQ);
            nAddItem = BROKERTYPE.Items.Add("爱建证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_AJZQ);
            nAddItem = BROKERTYPE.Items.Add("齐鲁证券(中泰证券)");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_QLZQ);
            nAddItem = BROKERTYPE.Items.Add("中银国际");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZYGJ);
            nAddItem = BROKERTYPE.Items.Add("民族证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_MZZQ);
            nAddItem = BROKERTYPE.Items.Add("湘财证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XCZQ);
            nAddItem = BROKERTYPE.Items.Add("国金证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GJZQ);
            nAddItem = BROKERTYPE.Items.Add("首创证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SCZQ);
            nAddItem = BROKERTYPE.Items.Add("国联证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GLZQ);
            nAddItem = BROKERTYPE.Items.Add("华林证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HLZQ);
            nAddItem = BROKERTYPE.Items.Add("华福证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HFZQ);
            nAddItem = BROKERTYPE.Items.Add("国元证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GYZQ);
            nAddItem = BROKERTYPE.Items.Add("广州证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GZZQ);
            nAddItem = BROKERTYPE.Items.Add("方正证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_FZZQ);
            nAddItem = BROKERTYPE.Items.Add("渤海证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_BHZQ);
            nAddItem = BROKERTYPE.Items.Add("西南证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XNZQ);
            nAddItem = BROKERTYPE.Items.Add("新时代证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XSDZQ);
            nAddItem = BROKERTYPE.Items.Add("中投证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZTZQ);
            nAddItem = BROKERTYPE.Items.Add("华融证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HRZQ);
            nAddItem = BROKERTYPE.Items.Add("申万宏源");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SYWG);
            nAddItem = BROKERTYPE.Items.Add("华鑫证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SHXZQ);
            nAddItem = BROKERTYPE.Items.Add("华泰证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HUATZQ);
            nAddItem = BROKERTYPE.Items.Add("东北证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_JLDB);
            nAddItem = BROKERTYPE.Items.Add("民生证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_MSZQ);
            nAddItem = BROKERTYPE.Items.Add("大同证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SXDT);
            nAddItem = BROKERTYPE.Items.Add("联储证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZCZQ);
            nAddItem = BROKERTYPE.Items.Add("厦门证券(长城国瑞证券)");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XMZQ);
            nAddItem = BROKERTYPE.Items.Add("东方证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DFZQ);
            nAddItem = BROKERTYPE.Items.Add("银泰证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_YTZQ);
            nAddItem = BROKERTYPE.Items.Add("江南证券(中航证券)");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_JLDT);
            nAddItem = BROKERTYPE.Items.Add("万和证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_WHZQ);
            nAddItem = BROKERTYPE.Items.Add("国开证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GKZQ);
            nAddItem = BROKERTYPE.Items.Add("中信万通(青岛证券)");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZXWT);
            nAddItem = BROKERTYPE.Items.Add("信达证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XDZQ);
            nAddItem = BROKERTYPE.Items.Add("五矿证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_WKZQ);
            nAddItem = BROKERTYPE.Items.Add("江海证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_JHZQ);
            nAddItem = BROKERTYPE.Items.Add("华创证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HCZQ);
            nAddItem = BROKERTYPE.Items.Add("太平洋证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_TPYZQ);
            nAddItem = BROKERTYPE.Items.Add("国海证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GHZQ);
            nAddItem = BROKERTYPE.Items.Add("东海证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DHZQ);
            nAddItem = BROKERTYPE.Items.Add("西部证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XBZQ);
            nAddItem = BROKERTYPE.Items.Add("山西证券(汇通启富)");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SXZQ);
            nAddItem = BROKERTYPE.Items.Add("华安证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HAHX);
            nAddItem = BROKERTYPE.Items.Add("国盛证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GSZQ);
            nAddItem = BROKERTYPE.Items.Add("宏信证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SCHX);
            nAddItem = BROKERTYPE.Items.Add("万联证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_WLZQ);
            nAddItem = BROKERTYPE.Items.Add("中天证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_LNZT);
            nAddItem = BROKERTYPE.Items.Add("恒泰证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_NMHT);
            nAddItem = BROKERTYPE.Items.Add("天风证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_TFZQ);
            nAddItem = BROKERTYPE.Items.Add("华龙证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_GSHL);
            nAddItem = BROKERTYPE.Items.Add("日信证券(国融证券)");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_RXZQ);
            nAddItem = BROKERTYPE.Items.Add("财通证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_CTZQ);
            nAddItem = BROKERTYPE.Items.Add("海通证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_HTZQ);
            nAddItem = BROKERTYPE.Items.Add("东吴证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_DWZQ);
            nAddItem = BROKERTYPE.Items.Add("浙商证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZJZS);
            nAddItem = BROKERTYPE.Items.Add("联讯证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_LXZQ);
            nAddItem = BROKERTYPE.Items.Add("上海华信");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SHHX);
            nAddItem = BROKERTYPE.Items.Add("东方财富");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_XZTX);
            nAddItem = BROKERTYPE.Items.Add("中原证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_ZYZQ);
            nAddItem = BROKERTYPE.Items.Add("国都证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_BJGD);
            nAddItem = BROKERTYPE.Items.Add("中邮证券");
            BrokerMap.Add(nAddItem, EZMBrokerType.BROKERTYPE_SXZY);
            this.BROKERTYPE.SelectedIndex = 0;

            nAddItem = this.TRADEDATA.Items.Add("资金");
            nAddItem = this.TRADEDATA.Items.Add("股份");
            nAddItem = this.TRADEDATA.Items.Add("当日委托");
            nAddItem = this.TRADEDATA.Items.Add("当日成交");
            nAddItem = this.TRADEDATA.Items.Add("当日可撤委托");
            nAddItem = this.TRADEDATA.Items.Add("股东代码");
            nAddItem = this.TRADEDATA.Items.Add("融资余额");
            nAddItem = this.TRADEDATA.Items.Add("融券余额");
            nAddItem = this.TRADEDATA.Items.Add("可融证券");
            nAddItem = this.TRADEDATA.Items.Add("可申购新股");
            nAddItem = this.TRADEDATA.Items.Add("新股申购额度");
            nAddItem = this.TRADEDATA.Items.Add("配号");
            nAddItem = this.TRADEDATA.Items.Add("中签");
            this.TRADEDATA.SelectedIndex = 0;

            nAddItem = this.HISDATA.Items.Add("历史委托");
            nAddItem = this.HISDATA.Items.Add("历史成交");
            nAddItem = this.HISDATA.Items.Add("资金流水");
            nAddItem = this.HISDATA.Items.Add("交割单");
            this.HISDATA.SelectedIndex = 0;

            /// 创建对象并初始化
            m_StockTrade = new StockTradeClass();
            if (null != m_StockTrade)
            {
                m_TradeEvent = new TradeEventSink();
                m_TradeEvent.SetIndex(1);

                /// 挂接事件
                m_StockTrade.InitEvent += m_TradeEvent.InitEvent;
                m_StockTrade.LoginEvent += m_TradeEvent.LoginEvent;
                m_StockTrade.OrderOKEvent += m_TradeEvent.OrderOKEvent;
                m_StockTrade.OrderErrEvent += m_TradeEvent.OrderErrEvent;
                m_StockTrade.OrderSuccessEvent += m_TradeEvent.OrderSuccessEvent;
                m_StockTrade.StockQuoteEvent += m_TradeEvent.StockQuoteEvent;
 
                m_StockTrade.ServerErrEvent += m_TradeEvent.ServerErrEvent;
                m_StockTrade.ServerChangedEvent += m_TradeEvent.ServerChangedEvent;

                /// 启用日志输出，便于调试程序
                m_StockTrade.EnableLog = true;

                /// 测试指定授权文件路径，否则使用默认和COM组件同目录的TradeAuth.zmd
                //m_StockTrade.AuthFile = "D:\\TradeAuth.zmd";

                /// 设置通讯版本(请查看自己券商的TDX版本)，初始化结果异步通过事件通知
                /// 设置最大连接数，默认传1(最好跟调用登录前设置的服务器主机数量一致)
                m_StockTrade.Init("8.05", 1);
            }
            else
            {
                /// 创建失败，请检查是否正常注册完成
                MessageBox.Show("创建失败，请检查是否正常注册完成");
            }

            if (null == m_StockTrade2)
            {
                m_StockTrade2 = new StockTradeClass();
                m_TradeEvent.SetIndex(2);
            }
        }

        private void CONNECT_BTN_Click(object sender, EventArgs e)
        {
             /// 自动保持连接
            m_StockTrade.AutoKeepConn = true;
            /// 设置为普通账号,true为信用账号
            m_StockTrade.CreditAccount = false;
            /// 设置成交自动回报定时器1000毫秒，设为0表示不启用
            m_StockTrade.ReportSuccessTimer = 1000;

            /// 设置券商
            int CurSel = this.BROKERTYPE.SelectedIndex;
            m_StockTrade.BrokerType = BrokerMap[CurSel];

            /// 设置登录账号类型
            EZMLoginAccountType eAccountType = EZMLoginAccountType.LOGINIACCOUNTTYPE_MNCS;
            CurSel = this.ACCOUNTTYPE.SelectedIndex;
            if (1 == CurSel)
                eAccountType = EZMLoginAccountType.LOGINIACCOUNTTYPE_CAPITAL;
            else if (2 == CurSel)
                eAccountType = EZMLoginAccountType.LOGINIACCOUNTTYPE_CUSTOMER;
            m_StockTrade.AccountType = eAccountType;
  
            /// 设置登录服务器
            m_StockTrade.CurServerHost = this.SERVERADDR.Text;
            m_StockTrade.CurServerPort = ushort.Parse(this.SERVERPORT.Text);

            /// 设置服务器交易账户和密码
            m_StockTrade.LoginID = this.LOGINID.Text;
            m_StockTrade.TradePassword = this.LOGINPW.Text;
            if (0 == this.LOGINPW.Text.Length)
            {
                MessageBox.Show("请先输入登录密码！");
                return;
            }
            /// 设置其他参数
            m_StockTrade.TradeAccount = this.TRADEACCOUNT.Text;/// 交易账号，一般为资金账号
            m_StockTrade.DepartmentID = ushort.Parse(this.DEPID.Text);/// 营业部ID

#if SYNC_OPT
            /// 指定同步连接，直到返回结果
            bool bRet = m_StockTrade.LogIn(false);
            if (bRet)
            {
                /// 无错误，获得登录成功的交易连接标识
                ushort nTradeID = m_StockTrade.CurTradeID;

                MessageBox.Show("登录成功，下面开始获取股东代码信息！");

                /// 获得股东信息
                ITradeRecord TradeRecord = m_StockTrade.ShareHolderCode;
                if (null != TradeRecord)
                {
                    /// 获得记录集的列数和行数
                    uint nFieldCount = TradeRecord.FieldCount;
                    uint nRecordCount = TradeRecord.RecordCount;

                    /// 弹出JSON格式数据包
                    MessageBox.Show(TradeRecord.GetJsonString());

                    for (uint i = 0; i < nRecordCount; i++)
                    {
                        /// 根据列字段名直接取数据，获取股东代码
                        var StockCode = TradeRecord.GetValueByName(i, "股东代码");
                        /// 遍历数据集合
                        for (uint j = 0; j < nFieldCount; j++)
                        {
                            /// 获取指定行和列的数据
                            var temVal = TradeRecord.GetValue(i, j);
                            var temType = TradeRecord.GetDataType(j);
                        }
                    }
                }
            }
#else
            /// 指定异步连接,事件回调的时候会传递自己的交易接口对象
            bool bRet = m_StockTrade.LogIn(true);
#endif
            if (!bRet)
            {
                /// 连接失败时获取错误描述和类型
                string strErrDesc = m_StockTrade.LastErrDesc;
                EZMTradeErrType ErrType = m_StockTrade.LastErrType;
                MessageBox.Show(strErrDesc);
            }
        }

        private void RELASE_BTN_Click(object sender, EventArgs e)
        {
            if (m_StockTrade.CurTradeID > 0)
                m_StockTrade.LogOut(m_StockTrade.CurTradeID);
        }

        private void GetQuote_Click(object sender, EventArgs e)
        {
            if (this.STOCKCODE.Text.Length < 6)
            {
                MessageBox.Show("查询股票代码应该是6位！");
                return;/// 代码错误
            }
            if (0 == m_StockTrade.CurTradeID)
            {
                MessageBox.Show("请先登录服务器再操作！");
                return;/// 没有登录
            }
            ITradeRecord StockRecord = m_StockTrade.GetStockQuote(m_StockTrade.CurTradeID, this.STOCKCODE.Text);
            if (null == StockRecord)
            {
                MessageBox.Show("获取指定股票实时5档行情失败！");
                return;/// 获取指定股票实时5档行情失败
            }
            if (0 == StockRecord.RecordCount)
            {
                MessageBox.Show("获取指定股票实时5档行情数据无记录！");
                return;/// 没有记录
            }
            /// 弹出JSON格式数据包
            MessageBox.Show(StockRecord.GetJsonString());
        }

        private void NewIPO_Click(object sender, EventArgs e)
        {
            m_StockTrade.BuyIPO();
        }

        private void GETTRADEDATA_Click(object sender, EventArgs e)
        {
            EZMStockQueryType eQueryType = EZMStockQueryType.STOCKQUERYTYPE_UNKNOWN;
            int CurSel = this.TRADEDATA.SelectedIndex;
            switch (CurSel)
            {
                case 0:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_CAPITAL;
                    break;
                case 1:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_STOCK;
                    break;
                case 2:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_TODAYORDER;
                    break;
                case 3:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_TODAYTRADE;
                    break;
                case 4:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_TODAYCANREVOKE;
                    break;
                case 5:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_SHAREHOLDERCODE;
                    break;
                case 6:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_BALANCEOFFINANCING;
                    break;
                case 7:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_BORROWFUNDLEFT;
                    break;
                case 8:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_CANBORROWFUND;
                    break;
                case 9:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_CANNEWSTOCK;
                    break;
                case 10:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_CANNEWSTOCKLIMIT;
                    break;
                case 11:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_ASSIGNNUMBER;
                    break;
                case 12:
                    eQueryType = EZMStockQueryType.STOCKQUERYTYPE_BALLOT;
                    break;
                default:
                    MessageBox.Show("错误查询类型！");
                    break;
            }
            ITradeRecord StockRecord = m_StockTrade.QueryTradeData(m_StockTrade.CurTradeID, eQueryType);
            if (null != StockRecord)
            {
                /// 遍历获取的数据
                uint nFieldCount = StockRecord.FieldCount;
                uint nRecordCount = StockRecord.RecordCount;

                /// 弹出JSON格式数据包
                MessageBox.Show(StockRecord.GetJsonString());

                StockRecord.Clear();
                StockRecord = null;
            }
        }

        private void GETHISDATA_Click(object sender, EventArgs e)
        {
            int CurSel = this.HISDATA.SelectedIndex;
            /// 获取17年5月8号到119号的历史数据
            ITradeRecord StockRecord = m_StockTrade.QueryHisTradeData(m_StockTrade.CurTradeID, 
                (EZMHisOrderType)(CurSel + 1), "20171113", "20171117");
            if (null != StockRecord)
            {
                /// 遍历获取的数据
                uint nFieldCount = StockRecord.FieldCount;
                uint nRecordCount = StockRecord.RecordCount;

                /// 弹出JSON格式数据包
                MessageBox.Show(StockRecord.GetJsonString());

                StockRecord.Clear();
                StockRecord = null;
            }
        }

        private void BUYBUT_Click(object sender, EventArgs e)
        {
            if (this.STOCKCODE.Text.Length < 6)
            {
                MessageBox.Show("查询股票代码应该是6位！");
                return;/// 代码错误
            }
            if (0 == m_StockTrade.CurTradeID)
            {
                MessageBox.Show("请先登录服务器再操作！");
                return;/// 没有登录
            }
            ITradeRecord StockRecord = m_StockTrade.GetStockQuote(m_StockTrade.CurTradeID, this.STOCKCODE.Text);
            if (null == StockRecord)
            {
                MessageBox.Show("获取指定股票实时5档行情失败！");
                return;/// 获取指定股票实时5档行情失败
            }
            if (0 == StockRecord.RecordCount)
            {
                MessageBox.Show("获取指定股票实时5档行情数据无记录！");
                return;/// 没有记录
            }
            /// 弹出JSON格式数据包
            MessageBox.Show(StockRecord.GetJsonString());

            /// 取当前价
            var vBuy4 = StockRecord.GetValueFloat(0, 5);
            EZMExchangeType eExchangeType = EZMExchangeType.EXCHANGETYPE_SH;
            if ("0" == this.STOCKCODE.Text.Substring(0, 1) || "1" == this.STOCKCODE.Text.Substring(0, 1) || "3" == this.STOCKCODE.Text.Substring(0, 1))
                eExchangeType = EZMExchangeType.EXCHANGETYPE_SZ;
#if SYNC_OPT
            /// 同步提交委托，知道返回结果
            ITradeRecord OrderRecord = m_StockTrade.SyncCommitOrder(true, EZMStockOrderType.STOCKORDERTYPE_BUY,
                EZMOrderPriceType.ORDERPRICETYPE_LIMIT, this.STOCKCODE.Text, vBuy4, 500,eExchangeType);
            if (null != OrderRecord)
            {
                if (OrderRecord.RecordCount > 0)
                {
                    /// 弹出JSON格式数据包
                    MessageBox.Show(OrderRecord.GetJsonString());

                    /// 取消委托，需要获取前面委托成功的ID
                    string strOrderID = OrderRecord.GetValueByName(0,"委托编号").ToString();
                    m_StockTrade.CancelOrder(eExchangeType, strOrderID);
                }
            }
            else
            {
                MessageBox.Show(m_StockTrade.LastErrDesc);
            }
#else
            /// 下面演示批量买入操作，通过AddOrder重复调用可以实现提交多条委托，然后调用CommitOrder一次性提交到服务器
            var vBuy5 = StockRecord.GetValueByName(0, "买五价");
            /// 限价买
            uint nReq1 = m_StockTrade.AddOrder(EZMStockOrderType.STOCKORDERTYPE_BUY, EZMOrderPriceType.ORDERPRICETYPE_LIMIT, this.STOCKCODE.Text,(float)vBuy5, 500,eExchangeType);
            uint nReq2 = m_StockTrade.AddOrder(EZMStockOrderType.STOCKORDERTYPE_BUY, EZMOrderPriceType.ORDERPRICETYPE_LIMIT, this.STOCKCODE.Text, (float)vBuy4, 500,eExchangeType);
            /// 真正提交委托操作，每个委托结果通过事件来通知，通过AddOrder返回的请求ID标识
            m_StockTrade.CommitOrder(m_StockTrade.CurTradeID, true, EZMRunPriType.RUNPRITYPE_NORMAL);
#endif
            StockRecord.Clear();
            StockRecord = null;
        }

        private void SELL_Click(object sender, EventArgs e)
        {
            if (0 == m_StockTrade.CurTradeID)
                return;/// 没有登录
            ITradeRecord StockRecord = m_StockTrade.QueryTradeData(m_StockTrade.CurTradeID,
                EZMStockQueryType.STOCKQUERYTYPE_STOCK);
            if (null == StockRecord)
                return;
            uint nRecordCount = StockRecord.RecordCount;
            if (0 == nRecordCount)
            {
                MessageBox.Show("没有持股");
                return;
            }
            /// 弹出JSON格式数据包
            MessageBox.Show(StockRecord.GetJsonString());

            for (uint nIndex = 0; nIndex < nRecordCount; nIndex++)
            {
                var varVal = StockRecord.GetValueByName(nIndex, "可用股份");
                uint nCount = (uint)varVal;
                if (0 == nCount)
                    continue;/// 无可用股份
                string strStockName = StockRecord.GetValueByName(nIndex, "证券名称").ToString();
                string strHolderCode = StockRecord.GetValueByName(nIndex, "股东代码").ToString();
                string strStockCode = StockRecord.GetValueByName(nIndex, "证券代码").ToString();

                /// 先获取当前证券的实时5档行情
                ITradeRecord QuoteRecord = m_StockTrade.GetStockQuote(m_StockTrade.CurTradeID, strStockCode);
                if (null == QuoteRecord)
                    continue;
                if (0 == QuoteRecord.RecordCount)
                {
                    QuoteRecord.Clear();
                    QuoteRecord = null;
                    continue;
                }
                /// 弹出JSON格式数据包
                MessageBox.Show(QuoteRecord.GetJsonString());

                varVal = QuoteRecord.GetValueByName(0, "卖一价");
                float fSell = (float)varVal;
                EZMExchangeType eExchangeType = EZMExchangeType.EXCHANGETYPE_UNKNOWN;
#if SYNC_OPT
                /// 同步操作，直到提交委托服务器返回结果
                ITradeRecord SellRecord = m_StockTrade.SyncCommitOrder(true, EZMStockOrderType.STOCKORDERTYPE_SALE,
                    EZMOrderPriceType.ORDERPRICETYPE_LIMIT, strStockCode, fSell, nCount, eExchangeType);
                if (null == SellRecord)
                {
                    /// 提交失败，取错误描述
                    string strLastErr = m_StockTrade.LastErrDesc;
                    QuoteRecord.Clear();
                    QuoteRecord = null;
                    continue;
                }
                if (0 == SellRecord.RecordCount)
                {
                    /// 提交委托异常
                    QuoteRecord.Clear();
                    QuoteRecord = null;
                    continue;
                }
                /// 弹出JSON格式数据包
                MessageBox.Show(SellRecord.GetJsonString());

                /// 得到交易服务器返回的委托编号ID
                /// 方便测试，马上取消委托卖出 
                string strOrderID = SellRecord.GetValueByName(0,"委托编号").ToString();
                m_StockTrade.CancelOrder(eExchangeType, strOrderID);
#else
                /// 返回的请求ID，会由事件通知的时候传回，从而知道每个委托的实际结果
                uint nReqID = m_StockTrade.AddOrder(EZMStockOrderType.STOCKORDERTYPE_SALE,
                    EZMOrderPriceType.ORDERPRICETYPE_LIMIT, strStockCode, fSell, nCount,eExchangeType);
#endif
                QuoteRecord.Clear();
                QuoteRecord = null;
            }
            /// 批量提交委托，结果通过事件通知得到
            m_StockTrade.CommitOrder(m_StockTrade.CurTradeID, true, EZMRunPriType.RUNPRITYPE_NORMAL);

            StockRecord.Clear();
            StockRecord = null;
        }

        public class TradeEventSink : _ITradeEvents
        {
            /// <summary>
            ///  记录事件回调对应的交易对象
            /// </summary>
            IStockTrade m_spiTrade = null;
            /// <summary>
            /// 记录交易对象的序号，高级版多账号下用
            /// </summary>
            ushort m_nTradeIndex = 0;

            /// <summary>
            /// 初始化接口通知
            /// </summary>
            /// <param name="vTrade" Desc="交易接口对象"></param>
            /// <param name="bInitOK" Desc="是否初始化成功标记"></param>
            public void InitEvent(object vTrade, bool bInitOK)
            {
                if (bInitOK)
                    MessageBox.Show("初始化成功！");
                /// 获得接口对象
                if (null == m_spiTrade)
                {
                    m_spiTrade = (IStockTrade)vTrade;
                }
            }

            /// <summary>
            /// 登录服务器成功通知
            /// </summary>
            /// <param name="vTrade" Desc="交易接口对象"></param>
            /// <param name="nTradeID" Desc="交易ID标识"></param>
            /// <param name="strHost" Desc="当前登录的交易服务器主机"></param>
            /// <param name="nPort" Desc="当前登录的交易服务器端口"></param>
            /// <param name="bLoginOK" Desc="是否登录成功标记"></param>
            public void LoginEvent(object vTrade, ushort nTradeID, string strHost, ushort nPort, bool bLoginOK)
            {
                if (null == m_spiTrade)
                    m_spiTrade = (IStockTrade)vTrade;
                if (bLoginOK)
                {
                    /// 异步事件处理中，请尽量不要有阻塞操作，避免影响底层流程正常处理。弹出消息框仅限于调试程序
                    if (1 == m_nTradeIndex)
                        MessageBox.Show("异步连接1成功！下面开始获取股东代码信息！");
                    else if (2 == m_nTradeIndex)
                        MessageBox.Show("异步连接2成功！下面开始获取股东代码信息！");

                    /// 可以检测连接状态有效性
                    bool bValid = m_spiTrade.ConnectValid;
                    /// 获取股东信息对象
                    ITradeRecord ShareHolder = m_spiTrade.ShareHolderCode;
                    if (null != ShareHolder)
                    {
                        MessageBox.Show(ShareHolder.GetJsonString());
                        ShareHolder = null;
                    }

                    MessageBox.Show("下面开始演示批量获取股票实时5档行情");
                    /// 异步获取多只股票的5档实时行情
                    m_spiTrade.GetStockQuotes(m_spiTrade.CurTradeID, "000001;000002;600001", EZMRunPriType.RUNPRITYPE_ABOVE_NORMAL);

                }
                else
                {
                    /// 弹出登录错误提示
                    MessageBox.Show(m_spiTrade.LastErrDesc);
                }
            }

            /// <summary>
            /// 委托交易提交成功通知
            /// </summary>
            /// <param name="nReqID" Desc="本组件维护的交易请求序列号"></param>
            /// <param name="eExchangeType Desc="交易市场类型""></param>
            /// <param name="vRecord" Desc="交易结果对象"></param>
            public void OrderOKEvent(uint nReqID, EZMExchangeType eExchangeType, object vRecord)
            {
                ITradeRecord TradeRecord = (ITradeRecord)vRecord;
                if (null != TradeRecord)
                {
                    uint nFieldCount = TradeRecord.FieldCount;
                    uint nRecordCount = TradeRecord.RecordCount;
                    if (nRecordCount > 0)
                    {
                        /// 弹出JSON格式数据包
                        MessageBox.Show(TradeRecord.GetJsonString());

                        /// 这儿测试马上取消委托，注意有些券商返回的委托成功记录第一个不是委托编号，需要调用GetValueByName来获取委托编号
                        string strOrderID = TradeRecord.GetValueByName(0,"委托编号").ToString();
                        ITradeRecord CancelRecord = m_spiTrade.CancelOrder(eExchangeType, strOrderID);
                        if (null != CancelRecord)
                        {
                            /// 弹出JSON格式数据包
                            MessageBox.Show(CancelRecord.GetJsonString());

                            uint nRC = CancelRecord.RecordCount;
                            /// 判断取消返回记录是否正常取消
                            CancelRecord.Clear();
                            CancelRecord = null;
                        }
                    }
                    TradeRecord.Clear();
                    TradeRecord = null;
                }
            }

            /// <summary>
            /// 委托交易成交通知，当日提交的委托，实际成交后发送的结果通知
            /// </summary>
            /// <param name="strOrderID" Desc="券商服务器上的委托ID标识"></param>
            /// <param name="strSuccessJson" Desc="成功的JSON数据包"></param>
            public void OrderSuccessEvent(string strOrderID, string strSuccessJson)
            {
                MessageBox.Show(strSuccessJson);
            }

            /// <summary>
            /// 委托错误通知
            /// </summary>
            /// <param name="nReqID" Desc="本组件维护的交易请求序列号"></param>
            /// <param name="ErrInfo"></param>
            public void OrderErrEvent(uint nReqID, string ErrInfo)
            {
                MessageBox.Show(ErrInfo);
            }

            /// <summary>
            /// 查询股票实时5档行情返回数据通知
            /// </summary>
            /// <param name="nReqID" Desc="本组件维护的交易请求序列号"></param>
            /// <param name="StockCode" Desc="单个股票编码"></param>
            /// <param name="vRecord" Desc="行情数据包"></param>
            public void StockQuoteEvent(uint nReqID, string StockCode, object vRecord)
            {
                ITradeRecord TradeRecord = (ITradeRecord)vRecord;
                if (null != TradeRecord)
                {
                    uint nFieldCount = TradeRecord.FieldCount;
                    uint nRecordCount = TradeRecord.RecordCount;

                    /// 弹出JSON格式数据包
                    MessageBox.Show(TradeRecord.GetJsonString());

                    for (uint i = 0; i < nRecordCount; i++)
                    {
                        /// 遍历数据集合
                        for (uint j = 0; j < nFieldCount; j++)
                        {
                            /// 获取指定行和列的数据
                            var temVal = TradeRecord.GetValue(i, j);
                            var temType = TradeRecord.GetDataType(j);
                        }
                    }
                }

            }

            /// <summary>
            ///  服务器产生错误通知
            /// </summary>
            /// <param name="nTradeID" Desc="交易ID标识"></param>
            /// <param name="nReqID" Desc="请求ID标识"></param>
            public void ServerErrEvent(ushort nTradeID, uint nReqID)
            {
            }

            /// <summary>
            ///  服务器切换通知，只有高级版多服务器主机配置才有本事件，暂未实现
            /// </summary>
            /// <param name="nPreTradeID" Desc="上一个交易ID标识"></param>
            /// <param name="nCurTradeID" Desc="当前交易ID标识"></param>
            public void ServerChangedEvent(ushort nPreTradeID, ushort nCurTradeID)
            {
            }

            public void SetIndex(ushort nIndex)
            {
                m_nTradeIndex = nIndex;
            }

            public void ReleaseTrade()
            {
                m_spiTrade = null;
            }
        };
    }
}
