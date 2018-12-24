#pragma once

namespace freeze
{
	// Not Used
	class CVImageView :
		public WTL::CZoomScrollWindowImpl<CVImageView>
	{
	public:
		DECLARE_WND_CLASS(L"WTL_CVImageView");

		bmp_image m_Image;

		BEGIN_MSG_MAP_EX(CVImageView)
			MSG_WM_CREATE(OnCreate)
		END_MSG_MAP()

		void SetBitmap(std::wstring const& file)
		{
			m_Image.raw_file(file);
			m_Image.create_all_image(GetDC());
		}

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			SetMsgHandled(FALSE);
			return 0;
		}

		void DoPaint(WTL::CDCHandle dc)
		{
			if (!m_Image)return;

			CRect rc;
			GetClientRect(rc);
		}
	};
}
