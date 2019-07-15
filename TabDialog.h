#pragma once

class CTabDialog : public CDialog
{
	DECLARE_DYNAMIC(CTabDialog)
public:
	CTabDialog(CWnd* pParent = NULL);
	virtual ~CTabDialog();
	//dialog data
	enum { IDD = IDD_TAB_DIALOG };
protected:
	// DDX/DDV support
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};