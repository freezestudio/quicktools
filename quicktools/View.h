// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CView :
	public WTL::CScrollWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(L"OpenCV_ImageView")

	WTL::CBitmap m_Bitmap;
	cv::Mat m_OriginData;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	BEGIN_MSG_MAP_EX(CView)
		MSG_WM_CREATE(OnCreate)
		//MSG_WM_PAINT(OnPaint)
		CHAIN_MSG_MAP(WTL::CScrollWindowImpl<CView>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void SetBitmap(std::string const& file)
	{
		m_OriginData = freeze::load_image(file);
		SetBitmap(m_OriginData);
	}

	void SetBitmap(cv::Mat const& origin)
	{
		if (m_Bitmap)
		{
			m_Bitmap.DeleteObject();
		}

		auto cvmat = freeze::flip(origin);
		auto channels = cvmat.channels();
		auto width = cvmat.rows;
		auto height = cvmat.cols;
		auto data = static_cast<unsigned char*>(cvmat.data);
		auto bpp = channels * 8;
		unsigned char buffer[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
		auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
		freeze::fill_bitmap_info(bitmap_info, width, height, bpp, 1);
		m_Bitmap.CreateDIBitmap(GetDC(), &bitmap_info->bmiHeader, CBM_INIT, data, bitmap_info, DIB_RGB_COLORS);

		SetScrollOffset(0, 0, FALSE);
		SetScrollSize(width, height);
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		SetMsgHandled(FALSE);
		return 0;
	}

	void DoPaint(WTL::CDCHandle dc)
	{
		if (!m_Bitmap)return;

		CRect rc;
		GetClientRect(rc);

		WTL::CDC memDC;
		memDC.CreateCompatibleDC(dc);
		auto old = memDC.SelectBitmap(m_Bitmap);
		SIZE bmpSize;
		m_Bitmap.GetSize(bmpSize);
		int left = (rc.Width() - bmpSize.cx) / 2;
		dc.BitBlt(left, 0, bmpSize.cx, bmpSize.cy, memDC, 0, 0, SRCCOPY);
		memDC.SelectBitmap(old);
	}
};
