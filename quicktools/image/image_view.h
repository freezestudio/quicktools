#pragma once

namespace freeze
{
	// Not Used
	class CVImageView :
		public WTL::CZoomScrollWindowImpl<CVImageView>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_CVImageView");

		WTL::CBitmap m_Bitmap;

		BEGIN_MSG_MAP_EX(CVImageView)
			MSG_WM_CREATE(OnCreate)
			MSG_WM_PAINT(OnPaint)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			cv::Mat cvmat = freeze::load_image("E:/image_data/xxx/normal.tiff");
			auto channels = cvmat.channels();
			auto width = cvmat.rows;
			auto height = cvmat.cols;
			auto data = static_cast<unsigned char*>(cvmat.data);
			auto bpp = channels * CHAR_BIT;
			unsigned char buffer[sizeof(BITMAPINFO) + static_cast<int>(std::numeric_limits<byte>::max()) * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width, height, bpp, 1);
			m_Bitmap.CreateDIBitmap(this->GetDC(), &bitmap_info->bmiHeader, CBM_INIT, data, bitmap_info, DIB_RGB_COLORS);

			SetMsgHandled(FALSE);
			return 0;
		}

		void OnPaint(WTL::CDCHandle /*dc*/)
		{
			WTL::CPaintDC dc(m_hWnd);

			CRect rc;
			GetClientRect(rc);
			WTL::CMemoryDC memDC(GetDC(), rc);
			memDC.SelectBitmap(m_Bitmap);
			dc.BitBlt(0, 0, rc.Width(), rc.Height(), memDC, 0, 0, SRCCOPY);
		}
	};
}