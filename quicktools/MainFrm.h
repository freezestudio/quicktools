// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define WINDOW_MENU_POSITION	3
#define PANE_DOCK_LEFT 0xFE01
#define PANE_DOCK_RIGHT 0xFE02
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
	WTL::CPaneContainer m_PaneContainer;

	DWORD m_dwDock = PANE_DOCK_RIGHT;
	int m_PaneWidth = DEFAULT_PANE_WIDTH;

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
		COMMAND_RANGE_HANDLER_EX(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
		CHAIN_MSG_MAP(WTL::CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(WTL::CFrameWindowImpl<CMainFrame>)
		END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

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
			WS_EX_CLIENTEDGE
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

	// Menu -> New
	void OnFileNew(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		CView* pView = new CView;
		pView->Create(m_TabView, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);

		m_TabView.AddPage(pView->m_hWnd, _T("normal.tiff"));

		// TODO: add code to initialize document
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
			m_TabView.RemovePage(nActivePage);
		else
			::MessageBeep((UINT)-1);

	}

	// Menu -> Window -> Close All
	void OnWindowCloseAll(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_TabView.RemoveAllPages();
	}

	// 0xFF00 - 0xFFFD = 254
	void OnWindowActivate(UINT /*uNotifyCode*/, int nID, CWindow /*wndCtl*/)
	{
		int nPage = nID - ID_WINDOW_TABFIRST;
		m_TabView.SetActivePage(nPage);
	}

	void CreatePaneContainer()
	{
		m_TabView.Create(
			this->m_hWndClient,
			rcDefault,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			WS_EX_CLIENTEDGE
		);
		m_PaneContainer.Create(this->m_hWndClient, L"Container");

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
