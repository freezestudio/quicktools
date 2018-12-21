#pragma once

#define ID_SEARCH_BAR 0xEE01
#define ID_DIRECTORY_BAR 0xEE02
#define ID_CHECKED_LISTVIEW 0xEE03
#define ID_CHECKED_HEADER 0xEE04

#define INNER_BAR_HEIGHT 20
#define NORMAL_IMAGE_FILE L"normal.tiff"
#define IMAGE_FILE_MINIMIZE_SIZE 32

// 打开图像文件时替换当前活动视图
#define OPEN_IMAGE_THIS 0
// 在新视图中打开图像文件
#define OPEN_IMAGE_NEW 1

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

	class CMyCheckedListViewCtrl :
		public WTL::CCheckListViewCtrlImpl<CMyCheckedListViewCtrl>
	{
	public:
		DECLARE_WND_SUPERCLASS(_T("WTL_CheckListView"), GetWndClassName())

		BEGIN_MSG_MAP_EX(CMyCheckedListViewCtrl)
			CHAIN_MSG_MAP(WTL::CCheckListViewCtrlImpl<CMyCheckedListViewCtrl>)
		END_MSG_MAP()

		void CheckSelectedItems(int nCurrItem)
		{
			// 用户勾选/取消勾选了此选项
			GetParent().PostMessage(WM_CHECKED_ITEM, nCurrItem, 0);

			// first check if this item is selected
			LVITEM lvi = {};
			lvi.iItem = nCurrItem;
			lvi.iSubItem = 0;
			lvi.mask = LVIF_STATE;
			lvi.stateMask = LVIS_SELECTED;
			this->GetItem(&lvi);
			// if item is not selected, don't do anything
			if (!(lvi.state & LVIS_SELECTED))
				return;
			// new check state will be reverse of the current state,
			BOOL bCheck = !this->GetCheckState(nCurrItem);
			int nItem = -1;
			int nOldItem = -1;
			while ((nItem = this->GetNextItem(nOldItem, LVNI_SELECTED)) != -1)
			{
				if (nItem != nCurrItem)
					this->SetCheckState(nItem, bCheck);
				nOldItem = nItem;
			}
		}
	};

	//
	// +---------------------+
	// |       搜索栏         |
	// +---------------------+
	// |     自定义列表视图     |
	// +---------------------+
	// |      目录浏览栏       |
	// +---------------------+
	//
	// 用于: CDockingContainer
	//
	class CCustomCheckedListview :
		public ATL::CWindowImpl<CCustomCheckedListview>
	{
	public:
		DECLARE_WND_SUPERCLASS(_T("WTL_CustomCheckedListview"), GetWndClassName())

		//
		// +---------------------+
		// |       搜索栏         |
		// +---------------------+
		// |     自定义列表视图     |
		// +---------------------+
		// |      目录浏览栏       |
		// +---------------------+
		//
		CSearchBar m_SearchBar;
		CMyCheckedListViewCtrl m_CheckListViewCtrl;
		CDirBar m_DirBar;

		// 所有原始(待处理)图像信息容器
		std::vector<ListViewData> m_VecData;

		// 图像文件目录
		std::wstring m_CurrentImageDirectory;
		// 工作中的图像文件
		std::wstring m_CurrentImageFile;

		// 检测图像文件目录
		std::wstring m_CurrentDetectImageDirectory;
		// 工作中的检测图像文件 与工作图像一一对应
		std::wstring m_CurrentDetectImageFile;

		//TODO: 是否有参考图像
		bool m_HasRefImage = false;

		//// 设置接收打开图像消息的窗口
		//HWND m_RecvOpenImageMsgWnd = nullptr;

		//void SetRecvOpenImageMessageWindow(HWND hWnd)
		//{
		//	m_RecvOpenImageMsgWnd = hWnd;
		//}

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

		bool SetSelectedItem(int item)
		{
			if (m_CheckListViewCtrl.IsWindow())
			{
				return m_CheckListViewCtrl.SelectItem(item) == TRUE;
			}
			return false;
		}

		void ResetListView(std::wstring const& dir,int dir_type = RESET_LISTVIEW_DIR)
		{
			if (dir_type == RESET_LISTVIEW_DIR_DETECT)
			{
				m_CurrentDetectImageDirectory = dir;
				return;
			}

			m_CurrentImageDirectory = dir;
			ReadImageFiles(m_CurrentImageDirectory);

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
			MESSAGE_HANDLER_EX(WM_CHECKED_ITEM, OnListViewItemChecked)
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
				hdi.mask = HDI_WIDTH | HDI_FORMAT;
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
			auto size = HIWORD(wParam);
			auto dir_type = LOWORD(wParam);

			wchar_t* pDir = new wchar_t[size];
			wmemcpy_s(pDir, size, reinterpret_cast<wchar_t*>(lParam), size);
			auto dir = std::wstring{ pDir,size };
			delete[] pDir;

			ResetListView(dir,dir_type);

			return 0;
		}

		// 当用户勾选了某个项时
		LRESULT OnListViewItemChecked(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
		{
			if (!m_CheckListViewCtrl)return 0;

			SetSelectedItem(static_cast<int>(wParam));
			NMHDR nmh = {};
			nmh.hwndFrom = m_CheckListViewCtrl;
			nmh.idFrom = ID_CHECKED_LISTVIEW;
			nmh.code = NM_CLICK;
			OnNotify(ID_CHECKED_LISTVIEW, &nmh);

			return 0;
		}


		LRESULT OnNotify(int idCtrl, LPNMHDR pnmh)
		{
			if (!m_CheckListViewCtrl)return 0;

			// 选择列表项时
			if (ID_CHECKED_LISTVIEW == idCtrl)
			{
				if (NM_CLICK == pnmh->code || NM_RCLICK == pnmh->code)
				{
					auto index = GetSelectedIndex();
					if (index >= 0)
					{
						auto checked = IsChecked(index);
						m_HasRefImage = checked;

						auto find = std::find_if(std::begin(m_VecData), std::end(m_VecData), [&index](auto data) {
							return data.index == (index + 1);
						});

						if (m_VecData.end() != find)
						{
							m_CurrentImageFile = m_CurrentImageDirectory + L"\\" + find->file;
						}

						if (m_HasRefImage && !m_CurrentDetectImageDirectory.empty())
						{
							auto detect_file = find->file;
							// 总长度是37，返回的是36
							auto pos = detect_file.find_last_of(L".tiff");
							// 替换的起始位置，替换多长
							detect_file.replace(pos-4, 5, L".bmp");
							m_CurrentDetectImageFile = m_CurrentDetectImageDirectory + L"\\" + detect_file;
						}
						else
						{
							m_CurrentDetectImageFile = L"";
						}
					}
					else
					{
						// 可能单击的是前面的复选框
						// TODO: 重载CheckListViewCtrl
						//CPoint pt;
						//GetCursorPos(&pt);
						//m_CheckListViewCtrl.ScreenToClient(&pt);
						//LVHITTESTINFO hitTestInfo = {};
						//hitTestInfo.pt = pt;
						//m_CheckListViewCtrl.HitTest(&hitTestInfo);
						//int item = hitTestInfo.iItem;
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
							pt.x, pt.y,
							this->m_hWnd,
							nullptr);
					}
					else
					{
						if (!m_CurrentImageFile.empty())
						{
							auto docking_container = GetParent();
							if (docking_container)
							{
								docking_container.SendMessage(WM_OPEN_IMAGE_WITH_DETECT, OPEN_IMAGE_THIS, reinterpret_cast<LPARAM>(m_CurrentDetectImageFile.c_str()));
								docking_container.SendMessage(WM_OPEN_IMAGE, OPEN_IMAGE_THIS, reinterpret_cast<LPARAM>(m_CurrentImageFile.c_str()));
							}
						}
					}
				}


				// TODO: 选择列表项前的复选框时
				// ... 与菜单项"勾选"相同

			}

			// 当选择列表视图标题栏中的全选框时
			if (auto header = m_CheckListViewCtrl.GetHeader(); header.GetDlgCtrlID() == idCtrl)
			{
				HDITEM hdi;
				try
				{
					// Release版，这里会异常
					if (header && header.GetItem(0, &hdi))
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
				catch (const std::exception& e)
				{
					MessageBoxA(nullptr, e.what(),"",MB_OK);
				}
			}

			SetMsgHandled(FALSE);
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
				GetParent().SendMessage(WM_OPEN_IMAGE_WITH_DETECT, OPEN_IMAGE_THIS, reinterpret_cast<LPARAM>(m_CurrentDetectImageFile.c_str()));
				GetParent().SendMessage(WM_OPEN_IMAGE, OPEN_IMAGE_THIS, reinterpret_cast<LPARAM>(m_CurrentImageFile.c_str()));
				break;
			case ID_CLV_MENU_OPEN_NEW: // 新建窗口打开
				GetParent().SendMessage(WM_OPEN_IMAGE_WITH_DETECT, OPEN_IMAGE_NEW, reinterpret_cast<LPARAM>(m_CurrentDetectImageFile.c_str()));
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
