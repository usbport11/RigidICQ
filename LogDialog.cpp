#include "stdafx.h"
#include "RigidICQ.h"
#include "LogDialog.h"

IMPLEMENT_DYNAMIC(CLogDialog, CDialog)

CLogDialog::CLogDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLogDialog::IDD, pParent)
{
	m_Log = new CListBox;
}

CLogDialog::~CLogDialog()
{
	if(m_Log) delete m_Log;
}

void CLogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, listLog, *m_Log);
}

BEGIN_MESSAGE_MAP(CLogDialog, CDialog)
END_MESSAGE_MAP()
