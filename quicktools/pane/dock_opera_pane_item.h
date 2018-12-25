#pragma once

namespace freeze {
	class CDockOperaItem : public ATL::CWindowImpl<CDockOperaItem>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_DockPane_Opera_Item")

		CNormalOperaForm m_NormalOperaForm; // 顶部一般选项面板
		COperaTools m_OperaTools;// 左侧工具箱
		CDragCheckLisView m_DragCheckListView; //右侧算子列表

		BEGIN_MSG_MAP_EX(CDockOperaItem)
			MSG_WM_CREATE(OnCreate)
			END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			//m_OperaToolBar.Create(
			//	this->m_hWnd,
			//	rcDefault,
			//	L"tools",
			//	WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			//	0UL,
			//	ID_OPERATOOLBAR
			//);

			/*m_DragCheckListView.Create(
				this->m_hWnd,
				rcDefault,
				L"drag-check-listview",
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_ICON | LVS_ALIGNLEFT | LVS_SINGLESEL,
				WS_EX_CLIENTEDGE
			);

			m_DragCheckListView.AddItem(0, 0, L"Canny", 0);
			m_DragCheckListView.AddItem(1, 0, L"Gaussian", 1);

			SetClient(m_DragCheckListView);*/

			SetMsgHandled(FALSE);
			return 0;
		}
	};
}
