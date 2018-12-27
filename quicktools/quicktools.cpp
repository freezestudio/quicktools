// quicktools.cpp : main source file for quicktools.exe
//

#include "stdafx.h"

#include "resource.h"
#include "image/utils.h"
#include "image/image_data.h"
#include "image/batch_image.h"
#include "image/image_view.h"
#include "pane/directorybar.h"
#include "pane/searchbar.h"
#include "pane/custom_checked_listview.h"
#include "pane/opera_tools.h"
#include "pane/normal_opera_form.h"
#include "pane/drag_check_listview.h"
#include "pane/dock_opera_pane_item.h"
#include "pane/dock_listview_pane.h"
#include "pane/dock_opera_pane.h"
#include "aboutdlg.h"
#include "dlgs/canny_dlg.h"
#include "dlgs/gaussian_dlg.h"
#include "dlgs/laplacian_of_gaussian_dlg.h"
#include "View.h"
#include "MainFrm.h"

WTL::CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	WTL::CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(SW_MAXIMIZE);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	WTL::AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
