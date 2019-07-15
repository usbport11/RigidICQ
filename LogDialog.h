#pragma once

class CLogDialog : public CDialog
{
	DECLARE_DYNAMIC(CLogDialog)
public:
	CLogDialog(CWnd* pParent = NULL);
	virtual ~CLogDialog();
	CListBox *m_Log;
	//dialog data
	enum { IDD = IDD_LOG_DIALOG };
protected:
	// DDX/DDV support
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
