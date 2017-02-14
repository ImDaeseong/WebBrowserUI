#include "stdafx.h"
#include "Resource.h"
#include "webbrowser2Ex.h"
#include "WebbrowserDialogDlg.h"

CWebBrowser2Ex::CWebBrowser2Ex()
{
}

CWebBrowser2Ex::~CWebBrowser2Ex()
{
}

BEGIN_MESSAGE_MAP(CWebBrowser2Ex, CWebBrowser2)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

BOOL CWebBrowser2Ex::PreTranslateMessage(MSG* pMsg) 
{		
	if (pMsg->message == WM_LBUTTONUP)
	{
		POINT pos;   
		GetCursorPos(&pos);   
		ScreenToClient(&pos);

		CWebbrowserDialogDlg* pMain = (CWebbrowserDialogDlg*)AfxGetMainWnd();
		pMain->MoveButtonClick(pos.x, pos.y);
	}
	return CWnd::PreTranslateMessage(pMsg);
}
