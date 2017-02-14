#pragma once
#include "webbrowser2Ex.h"
#include "afxwin.h"

class CWebbrowserDialogDlg : public CDialog
{
public:
	CWebbrowserDialogDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_WEBBROWSERDIALOG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
public:
	DECLARE_EVENTSINK_MAP()
	void DocumentCompleteExplorerDisplay(LPDISPATCH pDisp, VARIANT* URL);

private:
	void DrawSkin(CDC* pDC, BOOL bFirst = FALSE);
	BOOL LoadSkin();
	void MoveLocationDialog();
	CGdiPlusBitmapResource* m_pBackSkin;	
	BOOL m_bRGNStyle;
	
	CString GetModulePath();
	void InitControl();	
	void Navigate(CString strURL);
	CWebBrowser2Ex m_WebBrowser;
	BOOL m_LoadedBrowserFlag;

public:	
	int m_nOffset;
	int m_nWidth;
	int m_nHeight;
	CButton m_btnLeft;
	CButton m_btnRight;
	CButton m_btnOK;	
	void WebBrowserMoveClick(int x, int y, int width = 0, int height = 0);	
};
