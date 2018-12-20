// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define WINDOW_MENU_POSITION	3
#define PANE_DOCK_LEFT 0xEE01
#define PANE_DOCK_RIGHT 0xEE02
#define DEFAULT_PANE_WIDTH 400

class CMainFrame :
	public WTL::CFrameWindowImpl<CMainFrame>,
	public WTL::CUpdateUI<CMainFrame>,
	public WTL::CMessageFilter,
	public WTL::CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	WTL::CSplitterWindow m_SplitterWindow;
	WTL::CTabView m_TabView;
	WTL::CCommandBarCtrl m_CmdBar;
	freeze::CDockingContainer m_PaneContainer;

	std::map<std::wstring, int> m_mapImageFileView;
	std::wstring m_ActivedImage;

	DWORD m_dwDock = PANE_DOCK_RIGHT;
	int m_PaneWidth = DEFAULT_PANE_WIDTH;

	// TODO: 当用户激活时的处理
	CView* m_pActiveView = nullptr;
	freeze::CCannyDlg m_CannyDlg;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (WTL::CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_TabView.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER_EX(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER_EX(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER_EX(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_CLOSE, OnWindowClose)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_CANNY, OnCanny)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_LAPLACIAN, OnLaplacian)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_SOBEL, OnSobel)
		COMMAND_RANGE_HANDLER_EX(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
		MESSAGE_HANDLER_EX(WM_OPEN_IMAGE, OnOpenImage)
		MESSAGE_HANDLER_EX(WM_CANNY,OnCanny)
		CHAIN_MSG_MAP(WTL::CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(WTL::CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void AddImageFileView(std::wstring const& file, int page)
	{
		m_mapImageFileView.insert(std::make_pair(file, page));
	}

	void RemoveImageFileView(std::wstring const& file)
	{
		auto find = m_mapImageFileView.find(file);
		if (find != m_mapImageFileView.end())
		{
			m_mapImageFileView.erase(file);
		}
	}

	void RemoveImageFileView(int page)
	{
		auto find = std::find_if(std::begin(m_mapImageFileView), std::end(m_mapImageFileView), [&page](auto pair) {
			return pair.second == page;
		});
		if (m_mapImageFileView.end() != find)
		{
			m_mapImageFileView.erase(find);
		}
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		// 工具栏
		HWND hWndToolBar = CreateSimpleToolBarCtrl(
			m_hWnd,
			IDR_MAINFRAME,
			FALSE,
			ATL_SIMPLE_TOOLBAR_PANE_STYLE
		);

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		// 状态栏
		CreateSimpleStatusBar();

		// 客户区视图
		m_hWndClient = m_SplitterWindow.Create(
			m_hWnd,
			rcDefault,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0//WS_EX_CLIENTEDGE
		);

		// 容器面板
		CreatePaneContainer();

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		WTL::CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		WTL::CMenuHandle menuMain = m_CmdBar.GetMenu();
		m_TabView.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));

		// 算子对话框
		m_CannyDlg.Create(this->m_hWnd);

		return 0;
	}

	void UpdateLayout(BOOL bResizeBars = TRUE)
	{
		RECT rect = {};
		this->GetClientRect(&rect);

		// position bars and offset their dimensions
		UpdateBarsPosition(rect, bResizeBars);

		// resize client window
		if (m_hWndClient != NULL)
			::SetWindowPos(m_hWndClient, NULL, rect.left, rect.top,
				rect.right - rect.left, rect.bottom - rect.top,
				SWP_NOZORDER | SWP_NOACTIVATE);
	}

	// Canny算子参数
	LRESULT OnCanny(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (m_pActiveView)
		{
			m_pActiveView->PostMessage(WM_CANNY, wParam, lParam);
		}
		else
		{
			//WTL::AtlMessageBox(this->m_hWnd, L"没有活动视图", L"警告", MB_OK | MB_ICONWARNING);
		}
		SetMsgHandled(FALSE);
		return 0;
	}

	void OnDestroy()
	{
		// unregister message filtering and idle updates
		WTL::CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		SetMsgHandled(FALSE);
	}

	// Menu -> Exit
	void OnFileExit(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		PostMessage(WM_CLOSE);
	}

	LRESULT OnOpenImage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
	{
		m_ActivedImage = reinterpret_cast<wchar_t*>(lParam);
		if (auto active = static_cast<int>(wParam); active == 0)
		{
			// 已经显示的图像，激活到前台
			auto find = m_mapImageFileView.find(m_ActivedImage);
			if (m_mapImageFileView.end() != find)
			{
				m_TabView.SetActivePage(find->second);
				return 0;
			}

			if (auto page = m_TabView.GetActivePage(); -1 != page)
			{
				//CView* pView = &(*reinterpret_cast<CView*>(m_TabView.GetPageHWND(page)));
				//pView->RemoveBitmap();
				//RemoveImageFileView(pView);

				//pView->SetBitmap(freeze::convert_from(m_ActivedImage));
				//AddImageFileView(m_ActivedImage, page);
				OnWindowClose(0, 0, nullptr);
			}
			OnFileNew(0, 0, nullptr);
		}
		else
		{
			OnFileNew(0, 0, nullptr);
		}
		return 0;
	}

	// Menu -> New
	void OnFileNew(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		if (m_ActivedImage.empty())return;

		// TODO: 在哪里销毁的？
		CView* pView = new CView;
		m_pActiveView = pView;
		pView->Create(m_TabView, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

		m_TabView.AddPage(pView->m_hWnd, m_ActivedImage.c_str());
		pView->SetBitmap2(m_ActivedImage);

		AddImageFileView(m_ActivedImage, m_TabView.GetActivePage());
	}

	// Menu -> View -> Toolbar
	void OnViewToolBar(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		WTL::CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		WTL::CFrameWindowImpl<CMainFrame>::UpdateLayout();
	}

	// Menu -> View -> Status Bar
	void OnViewStatusBar(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		WTL::CFrameWindowImpl<CMainFrame>::UpdateLayout();
	}

	// Menu -> Help -> About
	void OnAppAbout(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
	}

	// Menu -> Window -> Close
	void OnWindowClose(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		int nActivePage = m_TabView.GetActivePage();
		if (nActivePage != -1)
		{
			m_TabView.RemovePage(nActivePage);
			RemoveImageFileView(nActivePage);
		}
		else
		{
			::MessageBeep((UINT)-1);
		}
	}

	// Menu -> Window -> Close All
	void OnWindowCloseAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_TabView.RemoveAllPages();
		m_mapImageFileView.clear();
		m_ActivedImage = L"";
	}

	// 0xFF00 - 0xFFFD = 254
	void OnWindowActivate(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
	{
		int nPage = nID - ID_WINDOW_TABFIRST;
		m_TabView.SetActivePage(nPage);
	}

	// Menu -> Operator -> Canny
	void OnCanny(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_CannyDlg.ShowWindow(SW_SHOW);
		if (m_pActiveView)
		{
			m_pActiveView->PostMessage(WM_CANNY, 0, 0);
		}
	}

	// Menu -> Operator -> Laplacian
	void OnLaplacian(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{

	}

	// Menu -> Operator -> Sobel
	void OnSobel(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{

	}

	void CreatePaneContainer()
	{
		m_TabView.Create(
			this->m_hWndClient,
			rcDefault,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0//WS_EX_CLIENTEDGE
		);

		m_PaneContainer.Create(this->m_hWndClient, L"图像分类");

		CRect rcSplit;
		GetClientRect(rcSplit);
		m_SplitterWindow.SetSplitterRect(rcSplit);

		if (m_dwDock == PANE_DOCK_LEFT)
		{
			m_SplitterWindow.SetSplitterPanes(m_PaneContainer, m_TabView);
			m_SplitterWindow.SetSplitterPos(DEFAULT_PANE_WIDTH);
		}
		else if (m_dwDock == PANE_DOCK_RIGHT)
		{
			m_SplitterWindow.SetSplitterPanes(m_TabView, m_PaneContainer);
			m_SplitterWindow.SetSplitterPos(rcSplit.Width() - DEFAULT_PANE_WIDTH);
		}
	}
};
