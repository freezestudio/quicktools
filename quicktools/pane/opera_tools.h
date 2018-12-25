#pragma once

namespace freeze {
	class COperaTools : public ATL::CWindowImpl<COperaTools, WTL::CStatic>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_Opera_Tools")

		BEGIN_MSG_MAP_EX(COperaTools)
		END_MSG_MAP()
	};
}