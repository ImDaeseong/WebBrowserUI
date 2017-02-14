#pragma once

#include "webbrowser2.h"

class CWebBrowser2Ex : public CWebBrowser2
{
public:
	CWebBrowser2Ex();
	virtual ~CWebBrowser2Ex();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
};
