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

	WTL::CCommandBarCtrl m_CmdBar;

	// splitter[tabview,splitter[listview_pane,opear_pane]]
	WTL::CSplitterWindow m_SplitterWindow;
	WTL::CTabView m_TabView;
	WTL::CHorSplitterWindow m_PaneSplitter;
	freeze::CDockingListViewContainer m_PaneListViewContainer;
	freeze::CDockingOperaContainer    m_PaneOperaContainer;

	std::map<std::wstring, int> m_mapImageFileView;
	std::wstring m_ActivedImage;   // 活动视图关联的原始图像
	std::wstring m_DetectImage;    // 关联的检测图像
	std::wstring m_RefImage;       // 参考图像
	bool m_RefImageInited = false; // 参考图像已经初始化

	DWORD m_dwDock = PANE_DOCK_RIGHT;
	int m_PaneWidth = DEFAULT_PANE_WIDTH;

	// TODO: 当用户激活时的处理
	CView* m_pActiveView = nullptr;

	// 对话框
	freeze::CCannyDlg m_CannyDlg;
	freeze::CGaussianDlg m_GaussianDlg;
	freeze::CLaplacianOfGaussianDlg m_LaplacianOfGaussianDlg;

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
		UPDATE_ELEMENT(ID_VIEW_LISTVIEWBAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER_EX(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER_EX(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER_EX(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER_EX(ID_VIEW_LISTVIEWBAR, OnViewListviewBar)
		COMMAND_ID_HANDLER_EX(ID_PANE_CLOSE, OnListviewPaneClose)
		COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_CLOSE, OnWindowClose)
		COMMAND_ID_HANDLER_EX(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_CANNY, OnCanny)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_LAPLACIAN, OnLaplacian)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_SOBEL, OnSobel)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_GAUSSIAN, OnGaussion)
		COMMAND_ID_HANDLER_EX(ID_OPERATOR_LAPLACIANOFGAUSSIAN, OnLaplacianOfGaussion)
		COMMAND_RANGE_HANDLER_EX(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
		MESSAGE_HANDLER_EX(WM_OPEN_IMAGE, OnOpenImage)
		MESSAGE_HANDLER_EX(WM_OPEN_REF_IMAGE, OnOpenRefImage)
		MESSAGE_HANDLER_EX(WM_OPEN_IMAGE_WITH_DETECT, OnOpenImageWithDetect)
		MESSAGE_HANDLER_EX(WM_CANNY, OnCannyHandler)
		MESSAGE_HANDLER_EX(WM_GAUSSIAN, OnGaussianHandler)
		MESSAGE_HANDLER_EX(WM_LAPLACIAN_OF_GAUSSIAN, OnLaplacianOfGaussianHandler)
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
		UISetCheck(ID_VIEW_TOOLBAR, TRUE);
		UISetCheck(ID_VIEW_STATUS_BAR, TRUE);
		UISetCheck(ID_VIEW_LISTVIEWBAR, TRUE);

		// register object for message filtering and idle updates
		WTL::CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		WTL::CMenuHandle menuMain = m_CmdBar.GetMenu();
		m_TabView.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));

		// 算子对话框
		m_CannyDlg.Create(this->m_hWnd);
		m_CannyDlg.SetRecvMessageWindow(this->m_hWnd);

		// 高斯模糊对话框
		m_GaussianDlg.Create(this->m_hWnd);
		m_GaussianDlg.SetRecvMessageWindow(this->m_hWnd);

		// LoG对话框
		m_LaplacianOfGaussianDlg.Create(this->m_hWnd);
		m_LaplacianOfGaussianDlg.SetRecvMessageWindow(this->m_hWnd);

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
	LRESULT OnCannyHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto id = LOWORD(wParam);
		if ((id == IDOK) || (id == IDCANCEL))
		{
			if (m_pActiveView)
			{
				m_pActiveView->AutoUseSome(AUTO_USE_LOG, false);
				return 0;
			}
		}

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

	// Gaussian算子参数
	LRESULT OnGaussianHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto id = LOWORD(wParam);
		if ((id == IDOK) || (id == IDCANCEL))
		{
			if (m_pActiveView)
			{
				m_pActiveView->AutoUseSome(AUTO_USE_GAUSSIAN, false);
				return 0;
			}
		}

		if (m_pActiveView)
		{
			m_pActiveView->PostMessage(WM_GAUSSIAN, wParam, lParam);
		}
		else
		{
			//WTL::AtlMessageBox(this->m_hWnd, L"没有活动视图", L"警告", MB_OK | MB_ICONWARNING);
		}
		SetMsgHandled(FALSE);
		return 0;
	}

	// LoG算子参数
	LRESULT OnLaplacianOfGaussianHandler(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto id = LOWORD(wParam);
		if ((id == IDOK) || (id == IDCANCEL))
		{
			if (m_pActiveView)
			{
				m_pActiveView->AutoUseSome(AUTO_USE_LOG, false);
				return 0;
			}
		}

		if (m_pActiveView)
		{
			m_pActiveView->PostMessage(WM_LAPLACIAN_OF_GAUSSIAN, wParam, lParam);
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

	// 请求打开一个图像文件
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
		}

		OnFileNew(0, 0, nullptr);

		return 0;
	}

	// 请求打开一个图像文件并且附带了检测图像
	LRESULT OnOpenImageWithDetect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
	{
		m_DetectImage = reinterpret_cast<wchar_t*>(lParam);

		// 重新设置对话框对应的复选框
		if (m_CannyDlg)
		{
			m_CannyDlg.SetDetectChecked(!m_DetectImage.empty());
		}

		// 如果有活动视图，则重新设置图像
		if (m_pActiveView)
		{
			m_pActiveView->SetBitmap2(m_ActivedImage, m_DetectImage);
		}

		return 0;
	}

	// 参考图像文件
	LRESULT OnOpenRefImage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
	{
		if (wParam)
		{
			m_RefImage = reinterpret_cast<wchar_t*>(lParam);
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

		bool show_ref_image = m_CannyDlg.IsRefImageShow();
		pView->ShowRefImage(show_ref_image);

		bool enable_minus = m_GaussianDlg.CanUseMinus();
		pView->EnableMinus(enable_minus);

		// 检查运算对话框的显示状态，并设置相应自动启用标志
		pView->AutoUseSome(AUTO_USE_OPERA, m_CannyDlg.IsWindowVisible() ? true : false);
		pView->AutoUseSome(AUTO_USE_GAUSSIAN, m_GaussianDlg.IsWindowVisible() ? true : false);
		pView->AutoUseSome(AUTO_USE_LOG, m_LaplacianOfGaussianDlg.IsWindowVisible() ? true : false);

		if (m_RefImage.empty())
		{
			m_RefImageInited = false;
		}

		if (!m_RefImage.empty())
		{
			pView->SetRefBitmap(m_RefImage);
			m_RefImageInited = true;
		}

		pView->SetBitmap2(m_ActivedImage, m_DetectImage);

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

	// Menu -> View -> Listview Bar
	void OnViewListviewBar(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		BOOL bShow = (m_SplitterWindow.GetSinglePaneMode() != SPLIT_PANE_NONE);
		m_SplitterWindow.SetSinglePaneMode(bShow ? SPLIT_PANE_NONE : SPLIT_PANE_LEFT);
		UISetCheck(ID_VIEW_LISTVIEWBAR, bShow);
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
			m_pActiveView->AutoUseSome(AUTO_USE_OPERA, true);
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

	// Menu -> Operator -> Gaussion
	void OnGaussion(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_GaussianDlg.ShowWindow(SW_SHOW);

		if (m_pActiveView)
		{
			m_pActiveView->AutoUseSome(AUTO_USE_GAUSSIAN, true);
			m_pActiveView->PostMessage(WM_GAUSSIAN, 0, 0);
		}
	}

	// Menu -> Operator -> L-o-G
	void OnLaplacianOfGaussion(UINT /*uNotifyCode*/, int /*nID*/, CWindow /*wndCtl*/)
	{
		m_LaplacianOfGaussianDlg.ShowWindow(SW_SHOW);

		if (m_pActiveView)
		{
			m_pActiveView->AutoUseSome(AUTO_USE_LOG, true);
			m_pActiveView->PostMessage(WM_LAPLACIAN_OF_GAUSSIAN, 0, 0);
		}
	}

	// Listview Pane -> “X”
	void OnListviewPaneClose(UINT /*uNotifyCode*/, int /*nID*/, CWindow wndCtl)
	{
		if (wndCtl.m_hWnd == m_PaneListViewContainer.m_hWnd)
		{
			m_SplitterWindow.SetSinglePaneMode(SPLIT_PANE_LEFT);
			UISetCheck(ID_VIEW_LISTVIEWBAR, FALSE);
		}
	}

	void CreatePaneContainer()
	{
		// 默认入到左侧分割窗口
		m_TabView.Create(
			this->m_hWndClient,
			rcDefault,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0//WS_EX_CLIENTEDGE
		);

		// 默认放到右侧分割窗口

		// 再次分割(垂直)
		m_PaneSplitter.Create(
			this->m_hWndClient,
			rcDefault,
			NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		);

		// 两容器放置到垂直分割窗口中

		m_PaneListViewContainer.SetPaneContainerExtendedStyle(PANECNT_NOBORDER);
		m_PaneListViewContainer.Create(this->m_PaneSplitter, L"图像分类");

		m_PaneOperaContainer.SetPaneContainerExtendedStyle(PANECNT_NOBORDER);
		m_PaneOperaContainer.Create(this->m_PaneSplitter, L"运算组合");

		m_PaneSplitter.SetSplitterPanes(m_PaneListViewContainer, m_PaneOperaContainer);
		m_PaneSplitter.SetSplitterPosPct(50); // 占50%

		if (m_dwDock == PANE_DOCK_LEFT)
		{
			m_SplitterWindow.SetSplitterPanes(m_PaneSplitter, m_TabView);
		}
		else if (m_dwDock == PANE_DOCK_RIGHT)
		{
			m_SplitterWindow.SetSplitterPanes(m_TabView, m_PaneSplitter);
		}

		UpdateLayout();
		m_SplitterWindow.SetSplitterPosPct(70); // 主视图占70%
	}
};
