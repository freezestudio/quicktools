#pragma once

#define ID_OPERATOOLBAR 0xEE20

namespace freeze {
	//
	// 可视化操作组合
	//
	class CDockingOperaContainer :
		public WTL::CPaneContainerImpl<CDockingOperaContainer>
	{
	public:
		DECLARE_WND_CLASS_EX(_T("WTL_Docking_Opera_Container"), 0, -1)

		CDockOperaItem m_DockOperaItem;

		BEGIN_MSG_MAP_EX(CDockingOperaContainer)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(WTL::CPaneContainerImpl<CDockingOperaContainer>)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_DockOperaItem.Create(
			this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
				0UL
			);
			SetClient(m_DockOperaItem);

			SetMsgHandled(FALSE);
			return 0;
		}
	};
}
