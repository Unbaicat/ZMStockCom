﻿namespace CSharp_Demo
{
    partial class TradeDemo
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.GETHISDATA = new System.Windows.Forms.Button();
            this.HISDATA = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.GETTRADEDATA = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.TRADEDATA = new System.Windows.Forms.ComboBox();
            this.STOCKCODE = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.BUYBUT = new System.Windows.Forms.Button();
            this.LOGINPW = new System.Windows.Forms.TextBox();
            this.DEPID = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.LOGINID = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.TRADEACCOUNT = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.RELASE_BTN = new System.Windows.Forms.Button();
            this.CONNECT_BTN = new System.Windows.Forms.Button();
            this.SERVERPORT = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.SERVERADDR = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.SELLALL = new System.Windows.Forms.Button();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.BROKERTYPE = new System.Windows.Forms.ComboBox();
            this.ACCOUNTTYPE = new System.Windows.Forms.ComboBox();
            this.GetQuote = new System.Windows.Forms.Button();
            this.NewIPO = new System.Windows.Forms.Button();
            this.label12 = new System.Windows.Forms.Label();
            this.CLIENTVERSION = new System.Windows.Forms.TextBox();
            this.checkBox_RZRQ = new System.Windows.Forms.CheckBox();
            this.label13 = new System.Windows.Forms.Label();
            this.COREVERSION = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // GETHISDATA
            // 
            this.GETHISDATA.Location = new System.Drawing.Point(166, 327);
            this.GETHISDATA.Name = "GETHISDATA";
            this.GETHISDATA.Size = new System.Drawing.Size(75, 24);
            this.GETHISDATA.TabIndex = 45;
            this.GETHISDATA.Text = "读取数据";
            this.GETHISDATA.UseVisualStyleBackColor = true;
            this.GETHISDATA.Click += new System.EventHandler(this.GETHISDATA_Click);
            // 
            // HISDATA
            // 
            this.HISDATA.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.HISDATA.FormattingEnabled = true;
            this.HISDATA.Location = new System.Drawing.Point(131, 291);
            this.HISDATA.Name = "HISDATA";
            this.HISDATA.Size = new System.Drawing.Size(110, 20);
            this.HISDATA.TabIndex = 44;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(15, 294);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(113, 12);
            this.label9.TabIndex = 43;
            this.label9.Text = "历史交易数据类型：";
            // 
            // GETTRADEDATA
            // 
            this.GETTRADEDATA.Location = new System.Drawing.Point(166, 250);
            this.GETTRADEDATA.Name = "GETTRADEDATA";
            this.GETTRADEDATA.Size = new System.Drawing.Size(75, 24);
            this.GETTRADEDATA.TabIndex = 42;
            this.GETTRADEDATA.Text = "获取数据";
            this.GETTRADEDATA.UseVisualStyleBackColor = true;
            this.GETTRADEDATA.Click += new System.EventHandler(this.GETTRADEDATA_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(18, 223);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(89, 12);
            this.label8.TabIndex = 41;
            this.label8.Text = "交易数据类型：";
            // 
            // TRADEDATA
            // 
            this.TRADEDATA.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.TRADEDATA.FormattingEnabled = true;
            this.TRADEDATA.Location = new System.Drawing.Point(131, 220);
            this.TRADEDATA.Name = "TRADEDATA";
            this.TRADEDATA.Size = new System.Drawing.Size(110, 20);
            this.TRADEDATA.TabIndex = 40;
            // 
            // STOCKCODE
            // 
            this.STOCKCODE.Location = new System.Drawing.Point(395, 253);
            this.STOCKCODE.Name = "STOCKCODE";
            this.STOCKCODE.Size = new System.Drawing.Size(161, 21);
            this.STOCKCODE.TabIndex = 39;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(325, 259);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(65, 12);
            this.label7.TabIndex = 38;
            this.label7.Text = "股票代码：";
            // 
            // BUYBUT
            // 
            this.BUYBUT.Location = new System.Drawing.Point(479, 286);
            this.BUYBUT.Name = "BUYBUT";
            this.BUYBUT.Size = new System.Drawing.Size(75, 24);
            this.BUYBUT.TabIndex = 37;
            this.BUYBUT.Text = "委托买入";
            this.BUYBUT.UseVisualStyleBackColor = true;
            this.BUYBUT.Click += new System.EventHandler(this.BUYBUT_Click);
            // 
            // LOGINPW
            // 
            this.LOGINPW.Location = new System.Drawing.Point(420, 98);
            this.LOGINPW.Name = "LOGINPW";
            this.LOGINPW.PasswordChar = '*';
            this.LOGINPW.Size = new System.Drawing.Size(136, 21);
            this.LOGINPW.TabIndex = 36;
            // 
            // DEPID
            // 
            this.DEPID.Location = new System.Drawing.Point(420, 57);
            this.DEPID.Name = "DEPID";
            this.DEPID.Size = new System.Drawing.Size(68, 21);
            this.DEPID.TabIndex = 35;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(317, 100);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(89, 12);
            this.label6.TabIndex = 34;
            this.label6.Text = "登录账号密码：";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(317, 61);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(65, 12);
            this.label5.TabIndex = 33;
            this.label5.Text = "营业部ID：";
            // 
            // LOGINID
            // 
            this.LOGINID.Location = new System.Drawing.Point(131, 97);
            this.LOGINID.Name = "LOGINID";
            this.LOGINID.Size = new System.Drawing.Size(154, 21);
            this.LOGINID.TabIndex = 32;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(17, 100);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(77, 12);
            this.label4.TabIndex = 31;
            this.label4.Text = "登录账号ID：";
            // 
            // TRADEACCOUNT
            // 
            this.TRADEACCOUNT.Location = new System.Drawing.Point(131, 55);
            this.TRADEACCOUNT.Name = "TRADEACCOUNT";
            this.TRADEACCOUNT.Size = new System.Drawing.Size(154, 21);
            this.TRADEACCOUNT.TabIndex = 30;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 58);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(77, 12);
            this.label3.TabIndex = 29;
            this.label3.Text = "交易账号ID：";
            // 
            // RELASE_BTN
            // 
            this.RELASE_BTN.Location = new System.Drawing.Point(418, 215);
            this.RELASE_BTN.Name = "RELASE_BTN";
            this.RELASE_BTN.Size = new System.Drawing.Size(75, 24);
            this.RELASE_BTN.TabIndex = 28;
            this.RELASE_BTN.Text = "退出";
            this.RELASE_BTN.UseVisualStyleBackColor = true;
            this.RELASE_BTN.Click += new System.EventHandler(this.RELASE_BTN_Click);
            // 
            // CONNECT_BTN
            // 
            this.CONNECT_BTN.Location = new System.Drawing.Point(321, 216);
            this.CONNECT_BTN.Name = "CONNECT_BTN";
            this.CONNECT_BTN.Size = new System.Drawing.Size(75, 24);
            this.CONNECT_BTN.TabIndex = 27;
            this.CONNECT_BTN.Text = "登录";
            this.CONNECT_BTN.UseVisualStyleBackColor = true;
            this.CONNECT_BTN.Click += new System.EventHandler(this.CONNECT_BTN_Click);
            // 
            // SERVERPORT
            // 
            this.SERVERPORT.Location = new System.Drawing.Point(420, 12);
            this.SERVERPORT.Name = "SERVERPORT";
            this.SERVERPORT.Size = new System.Drawing.Size(68, 21);
            this.SERVERPORT.TabIndex = 26;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(317, 15);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 12);
            this.label2.TabIndex = 25;
            this.label2.Text = "服务器端口：";
            // 
            // SERVERADDR
            // 
            this.SERVERADDR.Location = new System.Drawing.Point(131, 12);
            this.SERVERADDR.Name = "SERVERADDR";
            this.SERVERADDR.Size = new System.Drawing.Size(154, 21);
            this.SERVERADDR.TabIndex = 24;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(17, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 12);
            this.label1.TabIndex = 23;
            this.label1.Text = "服务器地址：";
            // 
            // SELLALL
            // 
            this.SELLALL.Location = new System.Drawing.Point(479, 325);
            this.SELLALL.Name = "SELLALL";
            this.SELLALL.Size = new System.Drawing.Size(75, 24);
            this.SELLALL.TabIndex = 46;
            this.SELLALL.Text = "一键清仓";
            this.SELLALL.UseVisualStyleBackColor = true;
            this.SELLALL.Click += new System.EventHandler(this.SELL_Click);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(20, 142);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(65, 12);
            this.label10.TabIndex = 47;
            this.label10.Text = "券商类型：";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(317, 142);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(65, 12);
            this.label11.TabIndex = 48;
            this.label11.Text = "账号类型：";
            // 
            // BROKERTYPE
            // 
            this.BROKERTYPE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.BROKERTYPE.FormattingEnabled = true;
            this.BROKERTYPE.Location = new System.Drawing.Point(131, 139);
            this.BROKERTYPE.Name = "BROKERTYPE";
            this.BROKERTYPE.Size = new System.Drawing.Size(154, 20);
            this.BROKERTYPE.TabIndex = 49;
            // 
            // ACCOUNTTYPE
            // 
            this.ACCOUNTTYPE.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ACCOUNTTYPE.FormattingEnabled = true;
            this.ACCOUNTTYPE.Location = new System.Drawing.Point(420, 139);
            this.ACCOUNTTYPE.Name = "ACCOUNTTYPE";
            this.ACCOUNTTYPE.Size = new System.Drawing.Size(136, 20);
            this.ACCOUNTTYPE.TabIndex = 50;
            // 
            // GetQuote
            // 
            this.GetQuote.Location = new System.Drawing.Point(395, 287);
            this.GetQuote.Name = "GetQuote";
            this.GetQuote.Size = new System.Drawing.Size(75, 23);
            this.GetQuote.TabIndex = 51;
            this.GetQuote.Text = "查询行情";
            this.GetQuote.UseVisualStyleBackColor = true;
            this.GetQuote.Click += new System.EventHandler(this.GetQuote_Click);
            // 
            // NewIPO
            // 
            this.NewIPO.Location = new System.Drawing.Point(395, 327);
            this.NewIPO.Name = "NewIPO";
            this.NewIPO.Size = new System.Drawing.Size(73, 23);
            this.NewIPO.TabIndex = 52;
            this.NewIPO.Text = "一键打新";
            this.NewIPO.UseVisualStyleBackColor = true;
            this.NewIPO.Click += new System.EventHandler(this.NewIPO_Click);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(22, 182);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(89, 12);
            this.label12.TabIndex = 53;
            this.label12.Text = "客户端版本号：";
            // 
            // CLIENTVERSION
            // 
            this.CLIENTVERSION.Location = new System.Drawing.Point(131, 178);
            this.CLIENTVERSION.Name = "CLIENTVERSION";
            this.CLIENTVERSION.Size = new System.Drawing.Size(57, 21);
            this.CLIENTVERSION.TabIndex = 54;
            // 
            // checkBox_RZRQ
            // 
            this.checkBox_RZRQ.AutoSize = true;
            this.checkBox_RZRQ.Location = new System.Drawing.Point(204, 181);
            this.checkBox_RZRQ.Name = "checkBox_RZRQ";
            this.checkBox_RZRQ.Size = new System.Drawing.Size(72, 16);
            this.checkBox_RZRQ.TabIndex = 55;
            this.checkBox_RZRQ.Text = "融资融券";
            this.checkBox_RZRQ.UseVisualStyleBackColor = true;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(321, 183);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(65, 12);
            this.label13.TabIndex = 56;
            this.label13.Text = "内核版本：";
            // 
            // COREVERSION
            // 
            this.COREVERSION.Location = new System.Drawing.Point(420, 178);
            this.COREVERSION.Name = "COREVERSION";
            this.COREVERSION.Size = new System.Drawing.Size(31, 21);
            this.COREVERSION.TabIndex = 57;
            // 
            // TradeDemo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(579, 357);
            this.Controls.Add(this.COREVERSION);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.checkBox_RZRQ);
            this.Controls.Add(this.CLIENTVERSION);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.NewIPO);
            this.Controls.Add(this.GetQuote);
            this.Controls.Add(this.ACCOUNTTYPE);
            this.Controls.Add(this.BROKERTYPE);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.SELLALL);
            this.Controls.Add(this.GETHISDATA);
            this.Controls.Add(this.HISDATA);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.GETTRADEDATA);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.TRADEDATA);
            this.Controls.Add(this.STOCKCODE);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.BUYBUT);
            this.Controls.Add(this.LOGINPW);
            this.Controls.Add(this.DEPID);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.LOGINID);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.TRADEACCOUNT);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.RELASE_BTN);
            this.Controls.Add(this.CONNECT_BTN);
            this.Controls.Add(this.SERVERPORT);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.SERVERADDR);
            this.Controls.Add(this.label1);
            this.Name = "TradeDemo";
            this.Text = "佐罗金COM组件C#例程";
            this.Closed += new System.EventHandler(this.TradeDemo_Closed);
            this.Load += new System.EventHandler(this.TradeDemo_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button GETHISDATA;
        private System.Windows.Forms.ComboBox HISDATA;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button GETTRADEDATA;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox TRADEDATA;
        private System.Windows.Forms.TextBox STOCKCODE;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button BUYBUT;
        private System.Windows.Forms.TextBox LOGINPW;
        private System.Windows.Forms.TextBox DEPID;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox LOGINID;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox TRADEACCOUNT;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button RELASE_BTN;
        private System.Windows.Forms.Button CONNECT_BTN;
        private System.Windows.Forms.TextBox SERVERPORT;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox SERVERADDR;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button SELLALL;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox BROKERTYPE;
        private System.Windows.Forms.ComboBox ACCOUNTTYPE;
        private System.Windows.Forms.Button GetQuote;
        private System.Windows.Forms.Button NewIPO;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox CLIENTVERSION;
        private System.Windows.Forms.CheckBox checkBox_RZRQ;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox COREVERSION;
    }
}

