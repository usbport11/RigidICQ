#include "stdafx.h"
#include "RigidICQ.h"
#include "MsgDialog.h"

IMPLEMENT_DYNAMIC(CMsgDialog, CDialog)

CMsgDialog::CMsgDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDialog::IDD, pParent)
{
}

CMsgDialog::~CMsgDialog()
{
	vector<char *>::iterator iterNam;
	for(iterNam = TabNames.begin(); iterNam!=TabNames.end(); iterNam++)
	{
		if(*iterNam) delete[] *iterNam;
	}
	TabNames.clear();
	vector<CTabDialog *>::iterator iterTab;
	for(iterTab = TabDialogs.begin(); iterTab!=TabDialogs.end(); iterTab++)
	{
		if(*iterTab) delete *iterTab;
	}
	TabDialogs.clear();
}

BOOL CMsgDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}

void CMsgDialog::AddTabToDialogs(char *UIN, bool NeedTest)
{
	if(!UIN) return;

	//prepare to create
	WORD FindNum;
	if(NeedTest)
	{
		//seek duplicate
		if(FindUINInTabs(UIN, FindNum))
		{
			delete [] UIN;

			//go to need tab
			TC_ITEM tci;
			tci.mask = TCIF_PARAM;
			m_Tabs.GetItem(FindNum, &tci);
			CWnd* pWnd = (CWnd *)tci.lParam;
			pWnd->ShowWindow(SW_SHOW); 
			return;
		}
		else
		{
			FindNum = TabNames.size();
			TabNames.push_back(UIN);
			TabDialogs.push_back(new CTabDialog);
		}
	}
	else
	{
		FindNum = TabNames.size();
		TabNames.push_back(UIN);
		TabDialogs.push_back(new CTabDialog);
	}

	//create tabs
	TC_ITEM tci;
	tci.mask = TCIF_TEXT;//error
	tci.iImage = -1;
	tci.pszText = TabNames[FindNum];
	m_Tabs.InsertItem(FindNum, &tci);
	//filling tabs
	tci.mask = TCIF_PARAM;
	tci.lParam = (LPARAM)TabDialogs[FindNum];
	m_Tabs.SetItem(FindNum, &tci);
	CWnd *Parent = &m_Tabs;
	TabDialogs[FindNum]->Create(IDD_TAB_DIALOG, Parent);
	TabDialogs[FindNum]->SetWindowPos(NULL, 5, 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	TabDialogs[FindNum]->ShowWindow(SW_SHOW);
	m_Tabs.SetCurSel(FindNum);
}
void CMsgDialog::AddMessageToDialog(char *UIN, char *Message)
{
	if(!Message) return;
	
	//seek duplicate
	WORD FindNum;
	if(FindUINInTabs(UIN, FindNum))
	{
		delete [] UIN;
	}
	else
	{
		FindNum = TabNames.size();
		//add tab without test on exist contact in tabs because search alredy was
		AddTabToDialogs(UIN, false);
	}

	//go to need tab
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_Tabs.GetItem(FindNum, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	//paste text
	CEdit *pEdit = (CEdit *)pWnd->GetDlgItem(edtDlg_History);
	if(!pEdit) return;
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(TabNames[FindNum]);
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(": ");
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(Message);
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel("\r\n\r\n");
}
bool CMsgDialog::FindUINInTabs(char *UIN, WORD &Number)
{
	Number = 0;
	if(!UIN) return false;
	if(TabNames.empty()) return false;

	for(WORD i=0; i<TabNames.size(); i++)
	{
		if(!strcmp(TabNames[i], UIN))
		{
			Number = i;
			return true;
		}
		Sleep(1);
	}

	return false;
}
void CMsgDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MSGS_TABS, m_Tabs);
}

BEGIN_MESSAGE_MAP(CMsgDialog, CDialog)
	ON_BN_CLICKED(btnDlg_SendMsg, &CMsgDialog::OnBnClickedSendmsg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MSGS_TABS, &CMsgDialog::OnTcnSelchangeMsgsTabs)
	ON_NOTIFY(TCN_SELCHANGING, IDC_MSGS_TABS, &CMsgDialog::OnTcnSelchangingMsgsTabs)
END_MESSAGE_MAP()

void CMsgDialog::OnBnClickedSendmsg()
{
	int iTab = m_Tabs.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_Tabs.GetItem(iTab, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	char Message[256]={0};
	pWnd->GetDlgItemTextA(edtDlg_Message, Message, 256);

	char UIN[16] = {0};
    tci.pszText = UIN;
    tci.cchTextMax = 16;
    tci.mask = TCIF_TEXT;
    m_Tabs.GetItem(iTab, &tci);

	//send
	if(pm_ICQClient->GetResultConnectCode() != 1) return;
	pm_ICQClient->bSendSimpleMessage(UIN, Message);

	//clear message edit
	pWnd->SetDlgItemTextA(edtDlg_Message, "");
	//paste msg into history
	CEdit *pEdit = (CEdit *)pWnd->GetDlgItem(edtDlg_History);
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel("Myself");
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(": ");
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel(Message);
	pEdit->SetSel(-1, -1);
	pEdit->ReplaceSel("\r\n\r\n");
}
void CMsgDialog::OnTcnSelchangeMsgsTabs(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTab = m_Tabs.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_Tabs.GetItem(iTab, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_SHOW); 
	*pResult = 0;
}

void CMsgDialog::OnTcnSelchangingMsgsTabs(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTab = m_Tabs.GetCurSel();
	TC_ITEM tci;
	tci.mask = TCIF_PARAM;
	m_Tabs.GetItem(iTab, &tci);
	CWnd* pWnd = (CWnd *)tci.lParam;
	pWnd->ShowWindow(SW_HIDE); 
	*pResult = 0;
}