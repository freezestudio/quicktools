#pragma once

namespace freeze {
	class CNormalOperaForm
		: public ATL::CWindowImpl<CNormalOperaForm, WTL::CStatic>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_Normal_Opera_Form")

		BEGIN_MSG_MAP_EX(CNormalOperaForm)
		END_MSG_MAP()
	};
}