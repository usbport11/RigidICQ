#include "stdafx.h"
#include "RigidICQ.h"
#include "RigidICQDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRigidICQDlg::CRigidICQDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRigidICQDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//interface elements
	m_Contacts = new CTreeCtrl;
	m_UIN = new CEdit;
	m_Password = new CEdit;
	m_btnGoOnline = new CButton;
	m_btnGoOffline = new CButton;

	m_ICQClient = new MICQClient;
	tm_WaitConnect_Delay = 10;

	Groups = NULL;
	Contacts = NULL;

	//Dialogs
	m_MsgDialog = new CMsgDialog;
	m_MsgDialog->Create(IDD_MSG_DIALOG, this);
	m_MsgDialog->pm_ICQClient = m_ICQClient;
	m_LogDialog = new CLogDialog;
	m_LogDialog->Create(IDD_LOG_DIALOG, this);
}

void CRigidICQDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, tr_Contacts, *m_Contacts);
	DDX_Control(pDX, edt_UIN, *m_UIN);
	DDX_Control(pDX, edt_Password, *m_Password);
	DDX_Control(pDX, btn_GoOnline, *m_btnGoOnline);
	DDX_Control(pDX, btn_GoOffline, *m_btnGoOffline);
}

BEGIN_MESSAGE_MAP(CRigidICQDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND( btn_GoOnline, OnGoOnline )
	ON_COMMAND( btn_GoOffline, OnGoOffline )
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, tr_Contacts, OnClickContacts)
	ON_MESSAGE(MSGDATA_DATAMESSAGE, OnMsgDataMessage)
	ON_MESSAGE(MSGDATA_ADDTOLOG, OnMsgAddToLog)
	ON_MESSAGE(MSGDATA_BOLDCONTACT, OnMsgBoldContact)
	ON_COMMAND( btn_ViewLog, OnViewLog )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CRigidICQDlg message handlers

BOOL CRigidICQDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	return TRUE;
}

void CRigidICQDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CRigidICQDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

afx_msg void CRigidICQDlg::OnGoOnline()
{
	//start connect thread
	_beginthread(ConnectThread, 0, this);
}
afx_msg void CRigidICQDlg::OnGoOffline()
{
	KillTimer(tm_WaitConnect);
	if(m_ICQClient->GetResultConnectCode() == 2) 
	{
		m_ICQClient->UserTerminateFlag = true;
		Sleep(2000);//wait for terminate threads
	}
	m_ICQClient->bDisconnect();
	m_LogDialog->m_Log->AddString("Disconnected");
	//set UserTerminateFlag to initial state
	m_ICQClient->UserTerminateFlag = false;
}
afx_msg void CRigidICQDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_ICQClient) return;
	switch(nIDEvent)
	{
		case tm_WaitConnect_ID:
			switch(m_ICQClient->GetResultConnectCode()) //may error
			{
				case 0: //not connected
					break;
				case 2: //error while connecting
					Sleep(100);
					KillTimer(tm_WaitConnect);
					m_ICQClient->bDisconnect();
					m_LogDialog->m_Log->AddString("Error while connecting");
					break;
				case 1: //connected
					//kill wait connect timer
					KillTimer(tm_WaitConnect);
					//fill contacts tree
					FillContactsFromMG();
					m_LogDialog->m_Log->AddString("Contacts tree filled");
					//Start main thread
					_beginthread(MainLoopThread, 0, this);
					break;
			}
			break;
	};
}
afx_msg void CRigidICQDlg::OnDestroy()
{
	//interface elements
	if(m_Contacts) delete m_Contacts;
	if(m_UIN) delete m_UIN;
	if(m_Password) delete m_Password;
	if(m_btnGoOnline) delete m_btnGoOnline;
	if(m_btnGoOffline) delete m_btnGoOffline;
	//delete Mastergroup
	MasterGroup *pMasterGroup = m_ICQClient->GetMasterGroup();
	if(pMasterGroup)
	{
		for(WORD i=0; i<pMasterGroup->mGroup.size(); i++)
		{
			if(Contacts[i]) delete [] Contacts[i];
		}
		if(Contacts) delete Contacts;
		if(Groups) delete [] Groups;
	}
	//delete ICQClass
	if(m_ICQClient) delete m_ICQClient;
	//delete dialogs
	if(m_MsgDialog) delete m_MsgDialog;
	if(m_LogDialog) delete m_LogDialog;
	CDialog::OnDestroy();
}
afx_msg void CRigidICQDlg::OnClickContacts(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM sel = m_Contacts->GetSelectedItem();
    CPoint point;
    GetCursorPos(&point);
    m_Contacts->ScreenToClient(&point);
    UINT flags = 0;
    HTREEITEM hitItem = m_Contacts->HitTest(point, &flags);
	//test on existing childs
	if(m_Contacts->GetChildItem(hitItem)) return;

    if(hitItem)
    {
        sel = hitItem;
        m_Contacts->SelectItem(sel);
        CString str;
        str = m_Contacts->GetItemText(sel);
		//add tab with test on exist contact in tabs
		char *UIN = new char[str.GetLength() + 1];
		memcpy(UIN, str, str.GetLength() + 1);
		m_MsgDialog->AddTabToDialogs(UIN, true);
		m_MsgDialog->ShowWindow(SW_SHOW);
    }  
}
LRESULT CRigidICQDlg::OnMsgDataMessage(WPARAM wParam, LPARAM lParam)
{
	char *UIN = (char*)wParam;
	char *Message = (char*)lParam;
	m_LogDialog->m_Log->AddString((CString)"Message from " + (CString)UIN);
	m_MsgDialog->AddMessageToDialog(UIN, Message);
	return 0;
}
LRESULT CRigidICQDlg::OnMsgAddToLog(WPARAM wParam, LPARAM lParam)
{
	char *Part1 = (char*)wParam;
	char *Part2 = (char*)lParam;
	m_LogDialog->m_Log->AddString((CString)Part1+ (CString)Part2);
	return 0;
}
LRESULT CRigidICQDlg::OnMsgBoldContact(WPARAM wParam, LPARAM lParam)
{
	TV_ITEM tvi;
	tvi.mask = TVIF_STATE | TVIF_HANDLE;
	tvi.hItem = Contacts[wParam][lParam];
	tvi.state = TVIS_BOLD;
	tvi.stateMask = TVIS_BOLD;
	m_Contacts->SetItem(&tvi);
	return 0;
}
afx_msg void CRigidICQDlg::OnViewLog()
{
	m_LogDialog->ShowWindow(SW_SHOW);
}
void MainLoopThread(void *Params)
{
	CRigidICQDlg *pRigidICQDlg = (CRigidICQDlg *)Params;
	pRigidICQDlg->m_LogDialog->m_Log->AddString("MainLoop started");
	pRigidICQDlg->m_ICQClient->bMainLoop();
	pRigidICQDlg->m_LogDialog->m_Log->AddString("MainLoop ended");
	_endthread();
}
void ConnectThread(void *Params)
{
	CRigidICQDlg *pRigidICQDlg = (CRigidICQDlg *)Params;

	//disable connect button
	EnableWindow(pRigidICQDlg->m_btnGoOnline->m_hWnd, 0);
	//start timer
	pRigidICQDlg->tm_WaitConnect = SetTimer(pRigidICQDlg->m_hWnd, tm_WaitConnect_ID, pRigidICQDlg->tm_WaitConnect_Delay, 0);
	pRigidICQDlg->m_LogDialog->m_Log->AddString("Waiting for connect...");
	//try to connect
	if(!pRigidICQDlg->m_ICQClient->bConnect())
	{
		pRigidICQDlg->m_LogDialog->m_Log->AddString("Cannot connect to icq");
		return;
	}
	else pRigidICQDlg->m_LogDialog->m_Log->AddString("Connected to icq");

	//try login
	CString HelpStr;
	pRigidICQDlg->m_UIN->GetWindowText(HelpStr);
	char *UIN = new char[HelpStr.GetLength() + 1];
	memcpy(UIN, HelpStr, HelpStr.GetLength() + 1);
	pRigidICQDlg->m_Password->GetWindowText(HelpStr);
	char *Password = new char[HelpStr.GetLength() + 1];
	memcpy(Password, HelpStr, HelpStr.GetLength() + 1);
	HelpStr = "";
	//test UIN and Password
	if(!strlen(UIN) || !strlen(Password))
	{
		MessageBox(NULL, "Empty UIN or Password", "Error", 0);
		return;
	}
	if(strlen(UIN) < 8)
	{
		MessageBox(NULL, "UIN too small", "Error", 0);
		return;
	}
	pRigidICQDlg->m_LogDialog->m_Log->AddString("Start login... Please wait");
	if(pRigidICQDlg->m_ICQClient->bLogin(UIN, Password))
	{
		pRigidICQDlg->m_LogDialog->m_Log->AddString("Login success");
	}
	else pRigidICQDlg->m_LogDialog->m_Log->AddString("Login failed");

	if(UIN) delete UIN;
	if(Password) delete Password;

	_endthread();
}
bool CRigidICQDlg::FillContactsFromMG()
{	
	MasterGroup *pMasterGroup = m_ICQClient->GetMasterGroup();
	if(pMasterGroup->mGroup.empty()) return 0;
	m_LogDialog->m_Log->AddString("Filling contacts tree...");
	char buf[10] = {0};
	Groups = new HTREEITEM[pMasterGroup->mGroup.size()];
	Contacts = new HTREEITEM*[pMasterGroup->mGroup.size()];
	for(unsigned int i=0; i<pMasterGroup->mGroup.size(); i++)
	{
		Contacts[i] = new HTREEITEM[pMasterGroup->mGroup[i].mBuddie.size()];
		memset(buf, 0, 10);
		itoa(pMasterGroup->mGroup[i].Id, buf, 10);
		Groups[i] = m_Contacts->InsertItem(buf);
		if(pMasterGroup->mGroup[i].mBuddie.empty()) continue;
		for(unsigned int j=0; j<pMasterGroup->mGroup[i].mBuddie.size(); j++)
		{
			Contacts[i][j] = m_Contacts->InsertItem(pMasterGroup->mGroup[i].mBuddie[j].UIN, Groups[i]);
			Sleep(1);
		}
		Sleep(1);
	}
	return 1;
}