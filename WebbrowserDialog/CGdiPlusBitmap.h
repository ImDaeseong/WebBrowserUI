#pragma once

class CGdiPlusBitmap
{
public:
	Gdiplus::Bitmap* m_pBitmap;

public:
	CGdiPlusBitmap()							{ m_pBitmap = NULL; }
	CGdiPlusBitmap(LPCWSTR pFile)				{ m_pBitmap = NULL; Load(pFile); }
	virtual ~CGdiPlusBitmap()					{ Empty(); }

	virtual void Empty()						{ if(m_pBitmap != NULL) delete m_pBitmap; m_pBitmap = NULL; }

	Bitmap* CloneToBitmap(Bitmap *pOrgPng)
	{        		
		if (!pOrgPng)
			return NULL;

		Bitmap *pTmpPng = new Bitmap(pOrgPng->GetWidth(), pOrgPng->GetHeight(), pOrgPng->GetPixelFormat());
		if (!pTmpPng)
			return NULL;

		Graphics *pGps = Graphics::FromImage(pTmpPng);
		pGps->DrawImage(pOrgPng, 0, 0, pOrgPng->GetWidth(), pOrgPng->GetHeight());
		delete pGps;

		return pTmpPng;		
	}

	bool Load(LPCWSTR pFile)
	{
		bool bLoad = false;

		CComBSTR wsFileName(pFile);

		Empty();
		Bitmap* pBitmap = Gdiplus::Bitmap::FromFile( wsFileName );
		if(pBitmap->GetLastStatus() == Gdiplus::Ok)
		{
			m_pBitmap = CloneToBitmap(pBitmap);
			bLoad = true;
		}
		delete pBitmap;
		pBitmap = NULL;

		return bLoad;
	}

	operator Gdiplus::Bitmap*() const			{ return m_pBitmap; }
};

class CGdiPlusBitmapResource : public CGdiPlusBitmap
{
protected:
	HGLOBAL m_hBuffer;

public:
	CGdiPlusBitmapResource()					{ m_hBuffer = NULL; }
	CGdiPlusBitmapResource(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
												{ m_hBuffer = NULL; Load(pName, pType, hInst); }
	CGdiPlusBitmapResource(UINT id, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
												{ m_hBuffer = NULL; Load(id, pType, hInst); }
	CGdiPlusBitmapResource(UINT id, UINT type, HMODULE hInst = NULL)
												{ m_hBuffer = NULL; Load(id, type, hInst); }
	virtual ~CGdiPlusBitmapResource()			{ Empty(); }

	virtual void Empty();

	bool Load(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL);
	bool Load(UINT id, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
												{ return Load(MAKEINTRESOURCE(id), pType, hInst); }
	bool Load(UINT id, UINT type, HMODULE hInst = NULL)
												{ return Load(MAKEINTRESOURCE(id), MAKEINTRESOURCE(type), hInst); }

	bool LoadPath(LPCTSTR pName);
	bool LoadDLL(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL);

	HRGN CreateRgnFromBitmap(Bitmap* pBitmap, int nXPos, int nYPos, int nImgWidth, int nImgHeight, COLORREF cfTransparent);
};

inline
void CGdiPlusBitmapResource::Empty()
{
	CGdiPlusBitmap::Empty();
	if (m_hBuffer)
	{
		::GlobalUnlock(m_hBuffer);
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	} 
}

inline
bool CGdiPlusBitmapResource::LoadPath(LPCTSTR pName)
{
	bool bLoad = false;

	CT2CW strFileName(pName);
	
	Empty();
	Bitmap* pBitmap = Gdiplus::Bitmap::FromFile((LPCWSTR) strFileName );
	if(pBitmap->GetLastStatus() == Gdiplus::Ok)
	{
		m_pBitmap = CloneToBitmap(pBitmap);
		bLoad = true;
	}
	delete pBitmap;
	pBitmap = NULL;

	return bLoad;
}

inline
bool CGdiPlusBitmapResource::Load(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	Empty();

	HRSRC hResource = ::FindResource(hInst, pName, pType);
	if (!hResource)
		return false;
	
	DWORD imageSize = ::SizeofResource(hInst, hResource);
	if (!imageSize)
		return false;

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
	if (!pResourceData)
		return false;

	m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			::GlobalUnlock(pBuffer);

			IStream* pStream = NULL;
			if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pBitmap = Gdiplus::Bitmap::FromStream(pStream);
				pStream->Release();
				if (m_pBitmap)
				{ 
					if (m_pBitmap->GetLastStatus() == Gdiplus::Ok)
						return true;

					delete m_pBitmap;
					m_pBitmap = NULL;
				}
			}			
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	return false;
}

inline
HRGN CGdiPlusBitmapResource::CreateRgnFromBitmap(Bitmap* pBitmap, int nXPos, int nYPos, int nImgWidth, int nImgHeight, COLORREF cfTransparent)
{
    RECT rcMemDC = {0, 0, nImgWidth, nImgHeight};
   	
	HDC hDC = GetDC(GetDesktopWindow());
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hMemBmp = CreateCompatibleBitmap(hDC, nImgWidth, nImgHeight);
    HBITMAP hOldMemBmp = (HBITMAP)SelectObject(hMemDC, hMemBmp);

    HBRUSH hBrush = CreateSolidBrush(cfTransparent);
    FillRect(hMemDC, &rcMemDC, hBrush);
    DeleteObject(hBrush);

    SetStretchBltMode(hMemDC, HALFTONE);
    Graphics graphics(hMemDC);

    RectF gRect; 
    gRect.X = (REAL)0;
    gRect.Y = (REAL)0; 
    gRect.Width = (REAL)nImgWidth; 
    gRect.Height = (REAL)nImgHeight;
   
    graphics.DrawImage(pBitmap, gRect, (REAL)nXPos, (REAL)nYPos, (REAL)nImgWidth, (REAL)nImgHeight, UnitPixel);

    int i = 0;
    int j = 0;
    POINT ptStart = {-1, -1};
    POINT ptEnd = {-1, -1};
    COLORREF color = 0;
    HRGN hImgRgn = CreateRectRgn(0, 0, nImgWidth, nImgHeight);
    for(i = 0; i < nImgWidth; i++)
    {
        for(j = 0; j < nImgHeight; j++)
        {
            color = GetPixel(hMemDC, i, j);
            if(color == cfTransparent)
            {
                if(ptStart.x == -1)
                {
                    ptStart.x = i;
                    ptStart.y = j;
                }
            }
            else
            {
                if(ptStart.x != -1)
                {
                    ptEnd.x = i + 1;
                    ptEnd.y = j;
                    HRGN hTransparentRgn = CreateRectRgn(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
                    CombineRgn(hImgRgn, hImgRgn, hTransparentRgn, RGN_DIFF);

                    DeleteObject(hTransparentRgn);
                    ptStart.x = -1;
                }
            }
        }
        if(ptStart.x != -1)
        {
            ptEnd.x = i + 1;
            ptEnd.y = j;
            HRGN hTransparentRgn = CreateRectRgn(ptStart.x, ptStart.y, ptEnd.x, ptEnd.y);
            CombineRgn(hImgRgn, hImgRgn, hTransparentRgn, RGN_DIFF);

            DeleteObject(hTransparentRgn);
            ptStart.x = -1;
        }
    }

    SelectObject(hMemDC, hOldMemBmp);
    DeleteObject(hMemBmp);
    DeleteDC(hMemDC);
	::ReleaseDC(GetDesktopWindow(), hDC);

    return hImgRgn;
}