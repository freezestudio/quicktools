#pragma once

#define ID_SEARCH_EDIT 0xEE20
#define ID_SEARCH_BUTTON 0xEE21
#define ID_SEARCH_TYPE_BUTTON 0xEE22

namespace freeze {
	class CSearchBar :
		public ATL::CWindowImpl<CSearchBar>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_SearchBar");

		WTL::CEdit m_SearchEdit;
		WTL::CBitmapButton m_SearchButton;
		WTL::CBitmapButton m_SearchTypeButton;

		BEGIN_MSG_MAP_EX(CSearchBar)
			MSG_WM_CREATE(OnCreate)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_SearchEdit.Create(this->m_hWnd,
				CRect{2, 0, 202, 20},
				L"¹Ø¼ü×ÖÉ¸Ñ¡",
				WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_LEFT,
				0UL,
				ID_SEARCH_EDIT );
			SetMsgHandled(FALSE);
			return 0;
		}
	};
}