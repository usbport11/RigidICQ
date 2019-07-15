#pragma once 
#include "TabDialog.h" 
#include "icqclient.h" 
class CMsgDialog : public CDialog 
{
DECLARE_DYNAMIC(CMsgDialog) 
public: 
	CMsgDialog(CWnd* pParent = NULL);
	virtual ~CMsgDialog(); 
	MICQClient *pm_ICQClient; 
	CTabCtrl m_Tabs; 
	vector<CTabDialog *> TabDialogs; 
	vector<char*> TabNames; 
	void AddTabToDialogs(char *UIN,bool NeedTest); 
	void AddMessageToDialog(char *UIN, char *Message); 
	bool FindUINInTabs(char *UIN, WORD &Number); 
	// Dialog Data 
	enum { IDD = IDD_MSG_DIALOG }; 
protected: 
	// DDX/DDV support 
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog(); 
	DECLARE_MESSAGE_MAP() 
public: 
	afx_msg void OnBnClickedSendmsg(); 
	afx_msg void OnTcnSelchangeMsgsTabs(NMHDR *pNMHDR, LRESULT *pResult); 
	afx_msg void OnTcnSelchangingMsgsTabs(NMHDR *pNMHDR, LRESULT *pResult); 
};