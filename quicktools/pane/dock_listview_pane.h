#pragma once

namespace freeze
{
	class CDockingListViewContainer :
		public WTL::CPaneContainerImpl<CDockingListViewContainer>
	{
	public:
		DECLARE_WND_CLASS_EX(_T("WTL_Docking_ListView_Container"), 0, -1)

		// 自定义列表视图
		freeze::CCustomCheckedListview m_Listview;

		BEGIN_MSG_MAP_EX(CDockingContainer)
			MSG_WM_CREATE(OnCreate)
			MESSAGE_HANDLER_EX(WM_OPEN_IMAGE,OnOpenImage)
			MESSAGE_HANDLER_EX(WM_OPEN_IMAGE_WITH_DETECT, OnOpenImageWithDetect)
			CHAIN_MSG_MAP(WTL::CPaneContainerImpl<CDockingListViewContainer>)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_Listview.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|LVS_REPORT,
				WS_EX_CLIENTEDGE);
			//m_Listview.AddColumn(L"序号", 1,80);
			m_Listview.AddColumn(L"工件码", 2,120);
			m_Listview.AddColumn(L"文件名", 3, 260);
			m_Listview.AddColumn(L"日期", 4, 120);
			m_Listview.AddColumn(L"时间", 5, 80);
			m_Listview.AddColumn(L"检测位置", 6);
			SetClient(m_Listview);

			SetMsgHandled(FALSE);
			return 0;
		}

		LRESULT OnOpenImage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
		{
			auto splitter = GetParent();
			if (splitter.m_hWnd)
			{
				auto main_frame = splitter.GetParent().GetParent();
				if (main_frame)
				{
					main_frame.SendMessage(WM_OPEN_IMAGE, wParam, lParam);
				}
			}
			
			SetMsgHandled(FALSE);
			return 0;
		}

		LRESULT OnOpenImageWithDetect(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
		{
			auto splitter = GetParent();
			if (splitter.m_hWnd)
			{
				auto main_frame = splitter.GetParent();
				if (main_frame)
				{
					main_frame.SendMessage(WM_OPEN_IMAGE_WITH_DETECT, wParam, lParam);
				}
			}

			SetMsgHandled(FALSE);
			return 0;
		}

	};
}
