#pragma once

namespace freeze
{
	class CDockingContainer :
		public WTL::CPaneContainerImpl<CDockingContainer>
	{
	public:
		DECLARE_WND_CLASS_EX(_T("WTL_DockingContainer"), 0, -1)

		freeze::CCustomCheckedListview m_Listview;

		BEGIN_MSG_MAP_EX(CDockingContainer)
			MSG_WM_CREATE(OnCreate)
			CHAIN_MSG_MAP(WTL::CPaneContainerImpl<CDockingContainer>)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			m_Listview.Create(this->m_hWnd,
				rcDefault,
				nullptr,
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|LVS_REPORT,
				WS_EX_CLIENTEDGE);
			m_Listview.AddColumn(L"序号", 1,80);
			m_Listview.AddColumn(L"工件码", 2,120);
			m_Listview.AddColumn(L"文件名", 3, 260);
			m_Listview.AddColumn(L"日期", 4, 120);
			m_Listview.AddColumn(L"时间", 5, 80);
			m_Listview.AddColumn(L"检测位置", 6);
			SetClient(m_Listview);

			SetMsgHandled(FALSE);
			return 0;
		}

	};
}