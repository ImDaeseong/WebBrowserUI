#include "stdafx.h"
#include "WebbrowserDialog.h"
#include "WebbrowserDialogDlg.h"

#include <comdef.h>
#include <mshtml.h>

CWebbrowserDialogDlg::CWebbrowserDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebbrowserDialogDlg::IDD, pParent)
{
	m_pBackSkin = NULL;
	m_LoadedBrowserFlag = FALSE;
	m_nOffset = 20;
	m_nWidth = 100;
	m_nHeight = 30;
	m_bRGNStyle = FALSE;
}

void CWebbrowserDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_DISPLAY, m_WebBrowser);
	DDX_Control(pDX, IDC_BUTTON1, m_btnLeft);
	DDX_Control(pDX, IDC_BUTTON2, m_btnRight);
	DDX_Control(pDX, IDC_BUTTON3, m_btnOK);
}

BEGIN_MESSAGE_MAP(CWebbrowserDialogDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CWebbrowserDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);

	if(!LoadSkin())
	{
		CDialog::OnCancel();
		return FALSE;
	}

	CDC* pDC = GetDC();
	if(pDC)
	{
		DrawSkin(pDC, TRUE);
		ReleaseDC(pDC);
	}
		 
	InitControl();

	return TRUE; 
}

void CWebbrowserDialogDlg::OnPaint()
{
	CPaintDC dc(this); 
	DrawSkin(&dc);
}

void CWebbrowserDialogDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_pBackSkin != NULL)
		delete m_pBackSkin;
	m_pBackSkin = NULL;	
}

void CWebbrowserDialogDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON)
	{	
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, TRUE,  0,0);
	}
	else
	{
		SystemParametersInfo(SPI_SETDRAGFULLWINDOWS, FALSE,  0,0);
	}	

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CWebbrowserDialogDlg::PreTranslateMessage(MSG* pMsg)
{	
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CWebbrowserDialogDlg::DrawSkin(CDC* pDC, BOOL bFirst)
{
	if (m_pBackSkin->m_pBitmap == NULL)	
		return;

	Graphics gps(pDC->GetSafeHdc());
	int x = m_pBackSkin->m_pBitmap->GetWidth();
	int y = m_pBackSkin->m_pBitmap->GetHeight();
	    	
	if(m_bRGNStyle)
	{
		CRect rc;
		GetClientRect(&rc);

		Color color; 
		color.SetFromCOLORREF(RGB(205,205,205));
		Pen pen(color, 1);
		gps.DrawRectangle(&pen, rc.left + m_nOffset-1, rc.top + m_nOffset-1, rc.Width() - 40 -1, rc.Height() - 40 - 2);

		if(bFirst)
		{
			HRGN hImgRgn = m_pBackSkin->CreateRgnFromBitmap(m_pBackSkin->m_pBitmap,  0, 0, x, y, RGB(236, 233, 216));
			SetWindowPos( NULL, 0, 0, x, y, SWP_NOZORDER | SWP_NOMOVE );
			SetWindowRgn( hImgRgn, FALSE );
		}
		else
		{
			gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
		}	
	}
	else
	{
		CRect rc;
		GetClientRect(&rc);

		Color color; 
		color.SetFromCOLORREF(RGB(205,205,205));
		
		CRoundRect rr;
		rr.DrawRoundRect(&gps, Rect(rc.left + m_nOffset + 10, rc.top + m_nOffset+ 10, rc.Width() - 40 -21, rc.Height() - 40 - 21), color, 15, 1);
		
		//Pen pen(color, 1);
		//GraphicsPath pPath;
		//GetRoundRectPath(&pPath,  Rect(rc.left + m_nOffset + 10, rc.top + m_nOffset+ 10, rc.Width() - 40 -21, rc.Height() - 40 - 21), 10, 2);
		//gps.DrawPath(&pen, &pPath);

		gps.DrawImage(m_pBackSkin->m_pBitmap, Rect(0, 0, x, y) , 0, 0, x, y, UnitPixel);
	}
}

BOOL CWebbrowserDialogDlg::LoadSkin()
{	
	UINT nBG_ID; 
	if(m_bRGNStyle)
		nBG_ID = IDB_PNG_BG1;		
	else
		nBG_ID = IDB_PNG_BG;		

	m_pBackSkin = new CGdiPlusBitmapResource;
	if(!m_pBackSkin->LoadPath(""))
	{		
		if(!m_pBackSkin->Load(MAKEINTRESOURCE(nBG_ID), _T("PNG"), AfxGetApp()->m_hInstance))
		{
			delete m_pBackSkin;
			m_pBackSkin = NULL;
			return FALSE;
		}
	}

	MoveLocationDialog();			
	
	return TRUE;
}

void CWebbrowserDialogDlg::MoveButtonClick(int x, int y, int width, int height)
{
	//m_btnOK.MoveWindow(x + m_nOffset, y + m_nOffset, width, height);
	
	CRect rLeft, rRight, rOk;

	m_btnLeft.GetWindowRect(&rLeft);
	ScreenToClient(&rLeft);

	m_btnRight.GetWindowRect(&rRight);
	ScreenToClient(&rRight);

	m_btnOK.GetWindowRect(&rOk);
	ScreenToClient(&rOk);


	POINT pos;
	pos.x = x + m_nOffset;
	pos.y = y + m_nOffset;
	if( PtInRect( &rLeft, pos ) == TRUE )
	{	
		OutputDebugString("left");
	}	
	else if( PtInRect( &rRight, pos ) == TRUE )
	{
		OutputDebugString("right");
	}	
	else if( PtInRect( &rOk, pos ) == TRUE )
	{
		OutputDebugString("ok");
	}	
}

void CWebbrowserDialogDlg::MoveLocationDialog()
{	
	int cx = m_pBackSkin->m_pBitmap->GetWidth();
	int cy = m_pBackSkin->m_pBitmap->GetHeight();

	RECT rcWorkArea;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
	MoveWindow( ((rcWorkArea.right - cx)/2), ((rcWorkArea.bottom - cy)/2), cx, cy);
}

CString CWebbrowserDialogDlg::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

void CWebbrowserDialogDlg::InitControl()
{		
	Navigate("https://github.com/");

	m_btnLeft.MoveWindow(10 + m_nOffset, 70 + m_nOffset,  m_nWidth, m_nHeight );
	m_btnRight.MoveWindow(160 + m_nOffset, 70 + m_nOffset,  m_nWidth, m_nHeight  );
	m_btnOK.MoveWindow(80 + m_nOffset, 120 + m_nOffset,  m_nWidth, m_nHeight  );	
	m_btnLeft.ShowWindow(SW_HIDE);
	m_btnRight.ShowWindow(SW_HIDE);
	m_btnOK.ShowWindow(SW_HIDE);
	
	
	CRect rc;
	GetClientRect(&rc);
    m_WebBrowser.MoveWindow(rc.left + m_nOffset, rc.top + m_nOffset, rc.Width() - 40, rc.Height() - 40);
	m_WebBrowser.ShowRoundRectRgn();
}

BEGIN_EVENTSINK_MAP(CWebbrowserDialogDlg, CDialog)
ON_EVENT(CWebbrowserDialogDlg, IDC_EXPLORER_DISPLAY, 259, CWebbrowserDialogDlg::DocumentCompleteExplorerDisplay, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

CString GetHtmlSource(IHTMLDocument2* pHDoc2, BOOL bHtml)
{
	if(pHDoc2==NULL)
		return "";

	CString strResult = "";
	
	CComPtr<IHTMLElementCollection> pAllColl;
	HRESULT hr;
	hr=pHDoc2->get_all(&pAllColl);
	if(hr==S_OK)
	{
		LONG length=0;
		hr=pAllColl->get_length(&length);
		if(hr==S_OK)
		{
			int nCheck = length;
			for(int i=0; i<length; i++)
			{
				VARIANT vIndex,vName;
				vName.vt=vIndex.vt=VT_I4;
				vName.lVal=vIndex.lVal=i;
				CComPtr<IDispatch> pDisp;
				hr=pAllColl->item(vName,vIndex,&pDisp);
				if( hr==S_OK )
				{
					CComPtr<IHTMLElement> pElement;
					hr=pDisp->QueryInterface(IID_IHTMLElement,(void**)&pElement);

					if( hr==S_OK )
					{
						CComBSTR tagName;
						hr=pElement->get_tagName(&tagName);
						if(hr==S_OK)
						{
							CString strTage(tagName);

							if(bHtml)
							{								
								if(lstrcmpi(strTage, "HTML") == 0)
								{
									CComBSTR pContent;
									hr=pElement->get_outerHTML(&pContent);								
									if(hr==S_OK)
										strResult += pContent;
								}
							}
							else
							{
								if(lstrcmpi(strTage, "HTML") != 0)
								{
									CComBSTR pContent;
									hr=pElement->get_outerHTML(&pContent);								
									if(hr==S_OK)
										strResult += pContent;
								}
							}
						}						
					}
				}
			}
		}
	}
	return strResult;
}

void CWebbrowserDialogDlg::DocumentCompleteExplorerDisplay(LPDISPATCH pDisp, VARIANT* URL)
{
	HRESULT   hr;
	LPUNKNOWN lpUnknown;
	LPUNKNOWN lpUnknownWB = NULL;
	LPUNKNOWN lpUnknownDC = NULL;
	lpUnknown = m_WebBrowser.GetControlUnknown();

	if (lpUnknown)
	{
		hr = lpUnknown->QueryInterface(IID_IUnknown,(LPVOID*)&lpUnknownWB);
		ASSERT(SUCCEEDED(hr));if (FAILED(hr))return;

		hr = pDisp->QueryInterface(IID_IUnknown,(LPVOID*)&lpUnknownDC);
		ASSERT(SUCCEEDED(hr));

		if (SUCCEEDED(hr) && lpUnknownWB == lpUnknownDC)
		{
			IWebBrowser2 *thisBrowser		= NULL;		
			HRESULT hr;

			hr = pDisp->QueryInterface(IID_IWebBrowser2, reinterpret_cast<void **>(&thisBrowser));

			if( SUCCEEDED(hr) ) {				
				thisBrowser->put_Silent( TRUE ); // 이거해야 자바스크립트 오류 안뜬다!!		
			}		

			if(m_LoadedBrowserFlag)
			{				
				if(thisBrowser)thisBrowser->Release();			
				if(thisBrowser)thisBrowser = NULL;

				if (lpUnknownWB)lpUnknownWB->Release();
				if (lpUnknownDC)lpUnknownDC->Release();

				GetParent()->SendMessage(WM_CALL_DLG_SHOW, 0, 0);
				return;
			}
			m_LoadedBrowserFlag = TRUE;

			IHTMLDocument2* pDoc = (IHTMLDocument2*)m_WebBrowser.GetDocument();
			CString szContent = GetHtmlSource(pDoc, FALSE);
			OutputDebugString(szContent);

			if(thisBrowser)thisBrowser->Release();			
			if(thisBrowser)thisBrowser = NULL;

			if (lpUnknownWB)lpUnknownWB->Release();
			if (lpUnknownDC)lpUnknownDC->Release();
		}
	}
}

void CWebbrowserDialogDlg::Navigate(CString strURL)
{
	VARIANT vtHeader, vtTarget, vtEmpty; 
	CString strHeader, strTarget; 
	
    strHeader = "Content-Type: application/x-www-form-urlencoded\r\n"; 
	
    strTarget = "_parent"; 

	::VariantInit(&vtHeader); 
	::VariantInit(&vtTarget); 
	::VariantInit(&vtEmpty); 
	
	vtHeader.vt = VT_BSTR; 
	
	vtHeader.bstrVal = strHeader.AllocSysString(); 
	
	vtTarget.vt = VT_BSTR; 
	
	vtTarget.bstrVal = strTarget.AllocSysString(); 
	
    m_WebBrowser.Navigate(strURL, &vtEmpty, &vtTarget, &vtEmpty, &vtHeader);

	SysFreeString(vtHeader.bstrVal);
	SysFreeString(vtTarget.bstrVal);
}