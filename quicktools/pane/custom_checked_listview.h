#pragma once

namespace freeze {
	struct ListViewData
	{
		int index;
		std::wstring title;
		std::wstring file;
		std::wstring date;
		std::wstring time;
		std::wstring loc;
	};

	class CCustomCheckedListview :
		public ATL::CWindowImpl<CCustomCheckedListview>
	{
	public:
		DECLARE_WND_SUPERCLASS(_T("WTL_CustomCheckedListview"), GetWndClassName())

		CSearchBar m_SearchBar;
		WTL::CCheckListViewCtrl m_CheckListViewCtrl;
		CDirBar m_DirBar;
		std::vector<ListViewData> m_VecData;

		void AddColumn(std::wstring const& label, int index, int width = 0)
		{
			if (m_CheckListViewCtrl.IsWindow())
			{
				auto added = m_CheckListViewCtrl.AddColumn(label.c_str(), index);
				if (width > 0)
				{
					m_CheckListViewCtrl.SetColumnWidth(added, width);
				}
			}
		}

		void AddItem(int item, int sub, std::wstring const& label)
		{
			if (m_CheckListViewCtrl.IsWindow())
			{
				m_CheckListViewCtrl.AddItem(item, sub, label.c_str());
			}
		}

		void ResetListView(std::wstring const& dir)
		{
			ReadImageFiles(dir);
			m_CheckListViewCtrl.AddItem(0, 0, L"1");
			m_CheckListViewCtrl.AddItem(0, 1, L"RE04FB5U084509");
			m_CheckListViewCtrl.AddItem(0, 2, L"RE04FB5U08450920180505114229_001.tiff");
			m_CheckListViewCtrl.AddItem(0, 3, L"20180505");
			m_CheckListViewCtrl.AddItem(0, 4, L"11:42:29");
			m_CheckListViewCtrl.AddItem(0, 5, L"001");
			m_CheckListViewCtrl.AddItem(1, 0, L"2");
		}

		void ReadImageFiles(std::wstring const& dir)
		{
			m_VecData.clear();
			auto path{ std::filesystem::path{dir} };
			auto begin = std::filesystem::directory_iterator{ path };
			auto end = std::filesystem::directory_iterator{};
			for (auto iter = begin; iter != end; ++iter)
			{
				if (iter->is_regular_file())
				{
					auto file = iter->path().wstring();
					AddToVector(file);
				}
			}
		}

		void AddToVector(std::wstring const file)
		{

		}

		BEGIN_MSG_MAP_EX(CCustomCheckedListview)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_SIZE(OnSize)
			MESSAGE_HANDLER_EX(WM_RESET_LISTVIEW, OnResetListView)
			END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_SearchBar.Create(this->m_hWnd);
			m_CheckListViewCtrl.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
			m_DirBar.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD | WS_VISIBLE);
			SetMsgHandled(FALSE);

			return 0;
		}

		void OnSize(UINT nType, CSize size)
		{
			if (size.cx > 0 && size.cy > 0)
			{
				UpdateLayout(size.cx, size.cy);
			}
		}

		LRESULT OnResetListView(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			auto size = static_cast<unsigned>(wParam);
			wchar_t* pDir = new wchar_t[size];
			wmemcpy_s(pDir, size, reinterpret_cast<wchar_t*>(lParam), size);
			std::wstring dir{ pDir,size };
			delete[] pDir;

			ResetListView(dir);

			return 0;
		}

		void UpdateLayout(int cx, int cy)
		{
			CRect sbRect = { 0,0,cx,20 };
			CRect dbRect = { 0,cy - 20,cx,cy };
			CRect lvRect = { 0,20,cx,cy - 20 };

			if (m_SearchBar.IsWindow())
			{
				m_SearchBar.SetWindowPos(nullptr, sbRect, SWP_NOZORDER);
			}

			if (m_CheckListViewCtrl.IsWindow())
			{
				m_CheckListViewCtrl.SetWindowPos(nullptr, lvRect, SWP_NOZORDER);
			}

			if (m_DirBar.IsWindow())
			{
				m_DirBar.SetWindowPos(nullptr, dbRect, SWP_NOZORDER);
			}
		}
	};
}