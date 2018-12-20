#pragma once

#define ID_SEARCH_BAR 0xEE01
#define ID_DIRECTORY_BAR 0xEE02
#define ID_CHECKED_LISTVIEW 0xEE03
#define ID_CHECKED_HEADER 0xEE04

#define INNER_BAR_HEIGHT 20
#define NORMAL_IMAGE_FILE L"normal.tiff"
#define IMAGE_FILE_MINIMIZE_SIZE 32

#define OPEN_IMAGE_THIS 0
#define OPEN_IMAGE_NEW 1
#define WM_OPEN_IMAGE WM_USER + 102

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

	// 带选择框的列表视图
	// 顶部添加了筛选框
	// 底部添加了目录框
	class CCustomCheckedListview :
		public ATL::CWindowImpl<CCustomCheckedListview>
	{
	public:
		DECLARE_WND_SUPERCLASS(_T("WTL_CustomCheckedListview"), GetWndClassName())

		CSearchBar m_SearchBar;
		WTL::CCheckListViewCtrl m_CheckListViewCtrl;
		CDirBar m_DirBar;
		std::vector<ListViewData> m_VecData;
		std::wstring m_CurrentImageDirectory;
		std::wstring m_CurrentImageFile;

		//TODO: 是否有参考图像
		bool m_HasRefImage = false;

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

		void InsertItem(int item, int id, ListViewData const& data)
		{
			LVITEM lv = { };
			lv.mask = LVIF_GROUPID;
			lv.iItem = item;
			lv.iSubItem = 0;
			lv.iGroupId = id;
			//lv.pszText = const_cast<wchar_t*>(std::to_wstring(data.index).c_str());

			if (m_CheckListViewCtrl.IsWindow())
			{
				m_CheckListViewCtrl.InsertItem(&lv);

				m_CheckListViewCtrl.SetItemText(item, 0, std::to_wstring(data.index).c_str());
				m_CheckListViewCtrl.SetItemText(item, 1, data.title.c_str());
				m_CheckListViewCtrl.SetItemText(item, 2, data.file.c_str());
				m_CheckListViewCtrl.SetItemText(item, 3, data.date.c_str());
				m_CheckListViewCtrl.SetItemText(item, 4, data.time.c_str());
				m_CheckListViewCtrl.SetItemText(item, 5, data.loc.c_str());
			}

		}

		int GetSelectedIndex() const
		{
			if (m_CheckListViewCtrl.IsWindow())
			{
				return m_CheckListViewCtrl.GetSelectedIndex();
			}
			return -1;
		}

		//ListViewData GetSelectedItem() const
		//{
		//	if (m_CheckListViewCtrl.IsWindow())
		//	{
		//		LVITEM lvd;
		//		auto ok = m_CheckListViewCtrl.GetSelectedItem(&lvd);
		//		if (ok)
		//		{
		//			int a = 0;
		//		}
		//	}
		//	return ListViewData{};
		//}

		void Checked(int index)
		{
			if (m_CheckListViewCtrl.IsWindow())
			{
				m_CheckListViewCtrl.CheckSelectedItems(index);
			}
		}

		bool IsChecked(int index) const
		{
			if (m_CheckListViewCtrl.IsWindow())
			{
				return m_CheckListViewCtrl.GetCheckState(index) == TRUE;
			}
			return false;
		}

		void ResetListView(std::wstring const& dir)
		{
			ReadImageFiles(dir);
			m_CheckListViewCtrl.DeleteAllItems();

			if (!m_CheckListViewCtrl.IsGroupViewEnabled())
			{
				m_CheckListViewCtrl.EnableGroupView(TRUE);
			}

			if (m_HasRefImage)
			{
				LVGROUP group = { sizeof(LVGROUP) };
				group.pszHeader = L"参考图像";
				group.iGroupId = 1;
				group.mask = LVGF_HEADER | LVGF_GROUPID;
				auto success = m_CheckListViewCtrl.AddGroup(&group);
				//auto dwErr = GetLastError();

				group.pszHeader = L"检测图像";
				group.iGroupId = 2;
				success = m_CheckListViewCtrl.AddGroup(&group);

				InsertItem(0, 1, m_VecData[0]);

				for (auto i = 1; i < m_VecData.size() - 1; ++i)
				{
					InsertItem(i, 2, m_VecData[i]);
				}
			}
			else
			{
				LVGROUP group = { sizeof(LVGROUP) };
				group.pszHeader = L"检测图像";
				group.iGroupId = 1;
				group.mask = LVGF_HEADER | LVGF_GROUPID;
				auto success = m_CheckListViewCtrl.AddGroup(&group);
				
				for (auto i = 0; i < m_VecData.size() - 1; ++i)
				{
					InsertItem(i, 1, m_VecData[i]);
				}
			}

		}

		void ReadImageFiles(std::wstring const& dir)
		{
			m_VecData.clear();
			m_HasRefImage = false;

			auto path{ std::filesystem::path{dir} };
			auto begin = std::filesystem::directory_iterator{ path };
			auto end = std::filesystem::directory_iterator{};
			int index = 1;
			for (auto iter = begin; iter != end; ++iter)
			{
				if (iter->is_regular_file())
				{
					auto file = iter->path().wstring();
					auto file_name = iter->path().filename();
					if (_wcsicmp(file_name.wstring().c_str(), NORMAL_IMAGE_FILE) == 0)
					{
						auto list_data = ListViewData{
							index++,
							L"参考图像",
							file_name.wstring(),
							L"NA",
							L"NA",
							L"NA",
						};
						m_VecData.emplace_back(list_data);
						m_HasRefImage = true;
					}
					else
					{
						AddToVector(index, file_name);
						index++;
					}

				}
			}
		}

		void AddToVector(int index, std::wstring const file)
		{
			assert(file.size() >= IMAGE_FILE_MINIMIZE_SIZE);

			auto title = file.substr(0, 14);
			auto date = file.substr(14, 8);
			auto format_date = date.substr(0, 4) + L"年" + date.substr(4, 2) + L"月" + date.substr(6, 2) + L"日";
			auto time = file.substr(22, 6);
			auto format_time = time.substr(0, 2) + L":" + time.substr(2, 2) + L":" + time.substr(4, 2);
			auto loc = file.substr(29, 3);
			auto list_data = ListViewData{
							index,
							title,//
							file,
							format_date,
							format_time,
							loc,
			};
			m_VecData.emplace_back(list_data);
		}

		BEGIN_MSG_MAP_EX(CCustomCheckedListview)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_SIZE(OnSize)
			MESSAGE_HANDLER_EX(WM_RESET_LISTVIEW, OnResetListView)
			MSG_WM_NOTIFY(OnNotify)
			MSG_WM_DROPFILES(OnDropFiles)
			MSG_WM_COMMAND(OnCommand)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_SearchBar.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD | WS_VISIBLE,
				0UL,
				ID_SEARCH_BAR);
			m_CheckListViewCtrl.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_ALIGNTOP | LVS_SINGLESEL, WS_EX_CLIENTEDGE,
				ID_CHECKED_LISTVIEW);
			if (m_CheckListViewCtrl.IsWindow())
			{
				auto header = m_CheckListViewCtrl.GetHeader();
				header.ModifyStyle(0, HDS_CHECKBOXES);
				header.SetDlgCtrlID(ID_CHECKED_HEADER);
				HDITEM hdi = {};
				hdi.mask = HDI_WIDTH|HDI_FORMAT;
				hdi.fmt = HDF_CHECKBOX;
				hdi.cxy = 80;
				header.AddItem(&hdi);
			}
			m_DirBar.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD | WS_VISIBLE,
				0UL,
				ID_DIRECTORY_BAR);
			SetMsgHandled(FALSE);


			// 拖放支持
			::DragAcceptFiles(this->m_hWnd, TRUE);

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
			m_CurrentImageDirectory = std::wstring{ pDir,size };
			delete[] pDir;

			ResetListView(m_CurrentImageDirectory);

			return 0;
		}

		LRESULT OnNotify(int idCtrl, LPNMHDR pnmh)
		{
			// 选择列表项时
			if (ID_CHECKED_LISTVIEW == idCtrl)
			{
				if (NM_CLICK == pnmh->code || NM_RCLICK == pnmh->code)
				{
					auto index = GetSelectedIndex() + 1;
					auto find = std::find_if(std::begin(m_VecData), std::end(m_VecData), [&index](auto data) {
						return data.index == index;
					});

					if (m_VecData.end() != find)
					{
						m_CurrentImageFile = m_CurrentImageDirectory + L"\\" + find->file;
					}

					// 弹出菜单
					if (NM_RCLICK == pnmh->code)
					{
						auto menu = LoadMenu(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDR_CLV_CONTEXT_MENU));
						auto pop = GetSubMenu(menu, 0);
						auto pt = reinterpret_cast<LPNMITEMACTIVATE>(pnmh)->ptAction;
						ClientToScreen(&pt);
						TrackPopupMenuEx(pop,
							TPM_NOANIMATION,
							pt.x,pt.y,
							this->m_hWnd,
							nullptr);
					}
					else
					{
						GetParent().SendMessage(WM_OPEN_IMAGE, OPEN_IMAGE_THIS, reinterpret_cast<LPARAM>(m_CurrentImageFile.c_str()));
					}
				}
			}

			// TODO: 选择列表项前的复选框时
			// ... 与菜单项"勾选"相同

			// 当选择列表视图中的全选框时
			if (auto header = m_CheckListViewCtrl.GetHeader(); header.GetDlgCtrlID()==idCtrl)
			{
				HDITEM hdi;
				if (header.GetItem(0, &hdi))
				{
					if ((hdi.fmt & HDF_CHECKED) != 0)
					{
						// 全选
					}
					else
					{
						// 取消全选
					}
				}
			}

			return 0;
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			switch (nID)
			{
			default:
				break;
			case ID_CLV_MENU_SELECT_ALL: // 全选
				break;
			case ID_CLV_MENU_SELECT_ONE: // 勾选
				break;
			case ID_CLV_MENU_OPEN_THIS: // 当前窗口打开
				GetParent().SendMessage(WM_OPEN_IMAGE, OPEN_IMAGE_THIS, reinterpret_cast<LPARAM>(m_CurrentImageFile.c_str()));
				break;
			case ID_CLV_MENU_OPEN_NEW: // 新建窗口打开
				GetParent().SendMessage(WM_OPEN_IMAGE, OPEN_IMAGE_NEW, reinterpret_cast<LPARAM>(m_CurrentImageFile.c_str()));
				break;
			}
		}

		void OnDropFiles(HDROP hDropInfo)
		{
			WCHAR szFile[MAX_PATH] = {};
			auto ret = ::DragQueryFile(hDropInfo, 0, szFile, MAX_PATH);
			if (ret)
			{
				m_DirBar.SendMessage(WM_DIRECTORY_CHANGED, 0, reinterpret_cast<LPARAM>(szFile));
				DragFinish(hDropInfo);
			}
		}

		void UpdateLayout(int cx, int cy)
		{
			CRect sbRect = { 0,0,cx,INNER_BAR_HEIGHT };
			CRect dbRect = { 0,cy - INNER_BAR_HEIGHT,cx,cy };
			CRect lvRect = { 0,INNER_BAR_HEIGHT,cx,cy - INNER_BAR_HEIGHT };

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