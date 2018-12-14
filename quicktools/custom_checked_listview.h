#pragma once

namespace freeze {
	class CCustomCheckedListview :
		public WTL::CCheckListViewCtrlImpl<CCustomCheckedListview>
	{
	public:
		DECLARE_WND_SUPERCLASS(_T("WTL_CustomCheckedListview"), GetWndClassName())

		BEGIN_MSG_MAP_EX(CCustomCheckedListview)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(WTL::CCheckListViewCtrlImpl<CCustomCheckedListview>)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			
			SetMsgHandled(FALSE);
			return 0;
		}

	};
}