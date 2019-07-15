#pragma once
#include "MsgDialog.h"
#include "LogDialog.h"

#define tm_WaitConnect_ID 1001

class CRigidICQDlg : public CDialog
{
public:
	CRigidICQDlg(CWnd* pParent = NULL);	// standard constructor
	//Messages dialog
	CMsgDialog *m_MsgDialog;
	CLogDialog *m_LogDialog;
	//window elements
	CTreeCtrl *m_Contacts;
	CEdit *m_UIN;
	CEdit *m_Password;
	CButton *m_btnGoOnline;
	CButton *m_btnGoOffline;
	//icq
	MICQClient *m_ICQClient;
	friend void MainLoopThread(void *Params);
	friend void ConnectThread(void *Params);
	bool FillContactsFromMG();
	HTREEITEM *Groups;
	HTREEITEM **Contacts;
	//timers
	UINT_PTR tm_WaitConnect;
	UINT tm_WaitConnect_Delay;
	// Dialog Data
	enum { IDD = IDD_RIGIDICQ_DIALOG };

protected:
	// DDX/DDV support
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	//icon
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGoOnline();
	afx_msg void OnGoOffline();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClickContacts(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT OnMsgDataMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgAddToLog(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgBoldContact(WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewLog();
	DECLARE_MESSAGE_MAP()
};