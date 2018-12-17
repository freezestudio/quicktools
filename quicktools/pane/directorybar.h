#pragma once

#define ID_DIRECTORY 0xEE10
#define ID_DIRECTORY_EDIT 0xEE11
#define ID_DIRECTORY_BUTTON 0xEE12
#define WM_DIRECTORY_CHANGED WM_USER+100
#define WM_RESET_LISTVIEW    WM_USER+101

namespace freeze {

	class CDirBar :
		public ATL::CWindowImpl<CDirBar>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_DirBar");

		WTL::CStatic m_Caption;
		WTL::CEdit m_DirEdit;
		WTL::CButton m_DirBtn;

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

		void ResetListView(std::wstring const& dir)
		{
			auto parent = GetParent();
			parent.SendMessage(WM_RESET_LISTVIEW, dir.size(), reinterpret_cast<LPARAM>(dir.c_str()));
		}

		BEGIN_MSG_MAP_EX(CDirBar)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_COMMAND(OnCommand)
			MESSAGE_HANDLER_EX(WM_DIRECTORY_CHANGED, OnDirectoryChanged)
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
			SetMsgHandled(FALSE);
			m_DirEdit.Create(this->m_hWnd,
				CRect{61, 0, 300, 20},
				L"请从右侧按钮选择图像文件目录",
				WS_CHILD | WS_VISIBLE | WS_DISABLED | WS_CLIPSIBLINGS | ES_LEFT,
				0UL,
				ID_DIRECTORY_EDIT);
			m_DirBtn.Create(this->m_hWnd,
				CRect{301, 0, 340, 20},
				L"...",
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_PUSHBUTTON,
				0UL,
				ID_DIRECTORY_BUTTON);

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
	};
}