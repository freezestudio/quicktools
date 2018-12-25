#pragma once

class CDragCheckLisView : public WTL::CCheckListViewCtrlImpl<CDragCheckLisView>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, WTL::CCheckListViewCtrl::GetWndClassName())

	WTL::CImageList m_ImageList;
	bool m_CanMovable = false;
	int m_iMovedItem = -1;

	//BOOL PreTranslateMessage(MSG* pMsg)
	//{
	//	pMsg;
	//	return FALSE;
	//}

	BEGIN_MSG_MAP_EX(CView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		m_ImageList.Create(32, 32, ILC_COLOR32, 2, 0);
		auto bmp = ::LoadBitmap(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_BITMAP_TOOLS));
		auto err = ::GetLastError();
		m_ImageList.Add(bmp);

		SetImageList(m_ImageList, LVSIL_NORMAL);

		SetMsgHandled(FALSE);
		return 0;
	}

	void OnDestroy()
	{
		m_ImageList.Destroy();
		SetMsgHandled(FALSE);
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
		LVHITTESTINFO lvh = {};
		lvh.pt = point;
		if ((this->HitTest(&lvh) != -1))
		{
			SetCapture();
			m_CanMovable = true;
			m_iMovedItem = lvh.iItem;
			//GetItemPosition(m_iMovedItem, &pt_last);
		}
		SetMsgHandled(FALSE);
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		ReleaseCapture();
		if (m_CanMovable)
		{
			LVITEM item = {};
			item.iItem = m_iMovedItem;
			item.mask = LVIF_PARAM;
			GetItem(&item);

			item.lParam = static_cast<LONG_PTR>(point.x);
			SetItem(&item);

			SetItemPosition(m_iMovedItem, point);
			m_CanMovable = false;
			Invalidate();
		}
		SetMsgHandled(FALSE);
	}

	//CPoint pt_last;
	void OnMouseMove(UINT nFlags, CPoint point)
	{
		//if (m_CanMovable)
		//{
		//	WTL::CClientDC dc{ this->m_hWnd };
		//	CRect rc;
		//	GetItemRect(m_iMovedItem, rc, LVIR_BOUNDS);
		//	auto width = rc.Width();
		//	auto heith = rc.Height();
		//	rc.left = pt_last.x;
		//	rc.top = pt_last.y;
		//	rc.right = rc.left + width;
		//	rc.bottom = rc.top + heith;
		//	dc.FillSolidRect(rc, RGB(255, 255, 255));
		//	
		//	m_ImageList.Draw(dc, 0, point, ILD_BLEND);
		//	pt_last = point;
		//}
		SetMsgHandled(FALSE);
	}

	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		LVHITTESTINFO lvh = {};
		lvh.pt = point;
		if ((this->HitTest(&lvh) != -1))
		{
			switch (lvh.iItem)
			{
			default:
				break;
			case 0:
				MessageBox(L"Canny");
				break;
			case 1:
				MessageBox(L"Gaussian");
				break;
			}
		}
		SetMsgHandled(FALSE);
	}
};
