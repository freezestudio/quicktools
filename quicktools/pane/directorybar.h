#pragma once

#define ID_DIRECTORY 0xEE10
#define ID_DIRECTORY_EDIT 0xEE11
#define ID_DIRECTORY_BUTTON 0xEE12
#define ID_DETECTDIRECTORY_EDIT 0xEE13
#define ID_DETECTDIRECTORY_BUTTON 0xEE14

// 重置原始图像目录
#define RESET_LISTVIEW_DIR 0
// 重置检测图像目录
#define RESET_LISTVIEW_DIR_DETECT 1

namespace freeze {
	class CStaticImpl : public ATL::CWindowImpl<CStaticImpl, WTL::CStatic>
	{
	public:
		DECLARE_WND_SUPERCLASS(NULL, WTL::CStatic::GetWndClassName())

		BEGIN_MSG_MAP_EX(CStaticImpl)
			MSG_WM_PAINT(OnPaint)
		END_MSG_MAP()

		void OnPaint(WTL::CDCHandle /*dc*/)
		{
			WTL::CPaintDC dc{ this->m_hWnd };
			dc.SaveDC();

			CRect rc;
			this->GetClientRect(rc);
			
			auto hFont = WTL::AtlCreateControlFont();
			dc.SelectFont(hFont);

			dc.DrawText(L"目录", -1, &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

			dc.RestoreDC(-1);
		}
	};

	template<typename T,typename TBase = WTL::CEdit,typename TWinTraits = ATL::CControlWinTraits>
	class CEditImpl : public ATL::CWindowImpl<T, TBase, TWinTraits>
	{
	public:
		DECLARE_WND_SUPERCLASS2(NULL, T, TBase::GetWndClassName())

		BEGIN_MSG_MAP_EX(CStaticImpl)
			MSG_WM_PAINT(OnPaint)
		END_MSG_MAP()

		void OnPaint(WTL::CDCHandle /*dc*/)
		{
			WTL::CPaintDC dc{ this->m_hWnd };
			dc.SaveDC();

			CRect rc;
			this->GetClientRect(rc);

			auto hFont = WTL::AtlCreateControlFont();
			dc.SelectFont(hFont);
			ATL::CString text;
			TBase::GetWindowText(text);
			dc.DrawText(text.GetBuffer(), -1, &rc, DT_VCENTER | DT_LEFT | DT_SINGLELINE);
			
			dc.RestoreDC(-1);
		}
	};

	class CMyEdit : public CEditImpl<CMyEdit>
	{
	public:
		DECLARE_WND_SUPERCLASS(_T("WTL_MyEdit"), GetWndClassName())

		CMyEdit()
		{
		}
	};

	class CDirBar :
		public ATL::CWindowImpl<CDirBar>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_DirBar");

		//WTL::CStatic m_Caption;// 目录标签
		CStaticImpl m_Caption;
		//WTL::CEdit m_DirEdit;
		CMyEdit m_DirEdit;
		WTL::CButton m_DirBtn;

		//WTL::CEdit m_DetectDirEdit;
		CMyEdit m_DetectDirEdit;
		WTL::CButton m_DetectDirBtn;
		std::wstring m_StrDetectDir;

		std::wstring GetDirectory() const
		{
			if (m_DirEdit.IsWindow())
			{
				WCHAR dir[MAX_PATH] = {};
				auto ret = m_DirEdit.GetWindowText(dir, MAX_PATH);
				if (ret > 0)
				{
					return std::wstring{ dir };
				}
			}
			return std::wstring{};
		}

		void ResetListView(std::wstring const& dir,int dir_type = RESET_LISTVIEW_DIR)
		{
			auto w = MAKEWPARAM(dir_type, dir.size());

			auto custom_listview = GetParent();
			if (custom_listview)
			{
				custom_listview.SendMessage(WM_RESET_LISTVIEW, w, reinterpret_cast<LPARAM>(dir.c_str()));
			}
		}

		BEGIN_MSG_MAP_EX(CDirBar)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_COMMAND(OnCommand)
			MESSAGE_HANDLER_EX(WM_DIRECTORY_CHANGED, OnDirectoryChanged)
			MESSAGE_HANDLER_EX(WM_DETECTDIRECTORY_CHANGED, OnDetectDirectoryChanged)
			//MSG_WM_SIZE(OnSize)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_Caption.Create(this->m_hWnd,
				CRect{0, 0, 60, 20},
				L"目录",
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | SS_LEFT,
				0UL,
				ID_DIRECTORY);
			
			m_DirEdit.Create(this->m_hWnd,
				CRect{61, 0, 240, 20},
				L"图像文件目录",
				WS_CHILD | WS_VISIBLE | WS_DISABLED | WS_CLIPSIBLINGS | ES_LEFT,
				0UL,
				ID_DIRECTORY_EDIT);
			m_DirBtn.Create(this->m_hWnd,
				CRect{241, 0, 280, 20},
				L"...",
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON,
				0UL,
				ID_DIRECTORY_BUTTON);

			m_DetectDirEdit.Create(this->m_hWnd,
				CRect{281, 0, 480, 20},
				L"检测图像文件目录",
				WS_CHILD | WS_VISIBLE | WS_DISABLED | WS_CLIPSIBLINGS | ES_LEFT,
				0UL,
				ID_DETECTDIRECTORY_EDIT);

			m_DetectDirBtn.Create(this->m_hWnd,
				CRect{481, 0, 520, 20},
				L"...",
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON,
				0UL,
				ID_DETECTDIRECTORY_BUTTON);

			SetMsgHandled(FALSE);
			return 0;
		}

		// unused
		void OnSize(UINT nType, CSize size)
		{
			// Empty
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			switch (nID)
			{
			case ID_DIRECTORY_BUTTON:
			{
				WTL::CFolderDialog folderDlg;
				auto result = folderDlg.DoModal();
				if (IDOK == result)
				{
					auto path = folderDlg.GetFolderPath();
					SendMessage(WM_DIRECTORY_CHANGED, 0, reinterpret_cast<LPARAM>(path));
				}
			}
			break;
			case ID_DETECTDIRECTORY_BUTTON:
			{
				WTL::CFolderDialog folderDlg;
				auto result = folderDlg.DoModal();
				if (IDOK == result)
				{
					auto path = folderDlg.GetFolderPath();
					SendMessage(WM_DETECTDIRECTORY_CHANGED, 0, reinterpret_cast<LPARAM>(path));
				}
			}
			break;
			default:
				break;
			}
		}

		LRESULT OnDirectoryChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam)
		{

			m_DirEdit.SetWindowText(reinterpret_cast<LPCTSTR>(lParam));
			auto dir = GetDirectory();
			ResetListView(dir);
			return 0;
		}

		LRESULT OnDetectDirectoryChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam)
		{

			m_DetectDirEdit.SetWindowText(reinterpret_cast<LPCTSTR>(lParam));
			m_StrDetectDir = reinterpret_cast<LPCWSTR>(lParam);
			ResetListView(m_StrDetectDir, RESET_LISTVIEW_DIR_DETECT);

			return 0;
		}
	};
}
