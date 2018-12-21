// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CView :
	public WTL::CScrollWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(L"OpenCV_ImageView")

	freeze::bmp_image m_Bitmap;
	// TODO: 临时保存的图像文件名
	std::wstring m_RawImageFile;
	// TODO: Canny参数
	freeze::CannyParam m_CannyParam;

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
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER_EX(WM_CANNY, OnCanny)
		MSG_WM_DROPFILES(OnDropFiles)
		CHAIN_MSG_MAP(WTL::CScrollWindowImpl<CView>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void SetCannyParam()
	{

	}

	void SetBitmap2(std::wstring const& file, std::wstring const& detectfile=L"", bool auto_scroll = true)
	{
		// TODO: 企图用于各种算子时动态改变图像
		m_RawImageFile = file;

		m_Bitmap.from_file(file);
		m_Bitmap.convert(GetDC());

		if (!detectfile.empty())
		{
			m_Bitmap.defect_file(detectfile);
			m_Bitmap.create_defect_bitmap(GetDC());
		}

		if (auto_scroll)
		{
			SetScroll();
		}
	}

	// 重置算子图像
	void ResetBitmap(int action = 4)
	{
		m_Bitmap.convert(GetDC(), action);
		InvalidateRect(nullptr);
		UpdateWindow();
	}

	// 拖放缺陷图像文件
	void OnDropFiles(HDROP hDropInfo)
	{
		WCHAR szFile[MAX_PATH] = {};
		auto ret = ::DragQueryFile(hDropInfo, 0, szFile, MAX_PATH);
		if (ret)
		{
			if (m_Bitmap)
			{
				m_Bitmap.defect_file(szFile);
				m_Bitmap.create_defect_bitmap(GetDC());
				Invalidate();
				UpdateWindow();
			}
			DragFinish(hDropInfo);
		}
	}

	void SetScroll()
	{
		if (m_Bitmap)
		{
			SetScrollOffset(0, 0, FALSE);
			SetScrollSize(m_Bitmap.size());
		}
	}

	BOOL OnEraseBkgnd(WTL::CDCHandle dc)
	{
		return TRUE;
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{

		// 启用拖放支持
		::DragAcceptFiles(this->m_hWnd, TRUE);

		SetMsgHandled(FALSE);
		return 0;
	}

	void DoPaint(WTL::CDCHandle dc)
	{
		if (!m_Bitmap)return;

		CRect rc;
		GetClientRect(rc);

		if (m_Bitmap)
		{
			auto left = (rc.Width() - m_Bitmap.width()) / 2;
			m_Bitmap.draw_ex(dc, left);
		}
	}

	// Canny算子参数
	LRESULT OnCanny(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		AdjustCannyParam((UINT)HIWORD(wParam), (UINT)LOWORD(wParam));
		return 0;
	}

	void AdjustCannyParam(UINT value, UINT nID)
	{
		int threshold1 = V_THRESHOLD_1;
		int threshold2 = V_THRESHOLD_2;
		int aperture = V_APERTURE;
		bool l2 = false;
		switch (nID)
		{
		default:
			break;
		case IDC_THRESHOLD_EDIT_1: // 阈值1
			m_CannyParam.threshold1 = value;
			break;
		case IDC_THRESHOLD_EDIT_2: // 阈值2
			m_CannyParam.threshold2 = value;
			break;
		case IDC__APERTURE_EDIT: // 孔洞
			m_CannyParam.aperture = value;
			break;
		case IDC_L2GRADIENT_CHECK: // L2
			m_CannyParam.l2 = value == 1 ? true : false;
			break;
		case ID_RESET_BTN: // 重置Canny参数
			m_CannyParam.threshold1 = threshold1;
			m_CannyParam.threshold2 = threshold2;
			m_CannyParam.aperture = aperture;
			m_CannyParam.l2 = l2;
			break;
		case IDC_CHECK_RESET_RAW://原始图像
			if (m_Bitmap)
			{
				m_Bitmap.exclude_other_image(value);
			}
			
			break;
		case IDC_CHECK_NO_DEFECT://缺陷图像图像
			if (m_Bitmap)
			{
				m_Bitmap.exclude_defect_image(!value);
			}
			
			break;
		}

		if (m_Bitmap)
		{
			//auto map = m_Bitmap.mat_raw_clone();
			//cv::Mat ret_mat;
			//cv::Canny(map, ret_mat, m_CannyParam.threshold1, m_CannyParam.threshold2, m_CannyParam.aperture, m_CannyParam.l2);
			//std::vector<std::vector<cv::Point>> contours;
			//std::vector<cv::Vec4i> hierarchy;
			//cv::findContours(ret_mat, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
			//cv::Scalar color{ 0.0,0.0,255.0,0.0 };
			//cv::Mat draw_image = cv::Mat::zeros(ret_mat.size(), CV_8UC3);
			//for (auto i = 0; i < contours.size(); ++i)
			//{
			//	cv::drawContours(draw_image, contours, i, color/*, 2, cv::LINE_8, hierarchy, 0*/);
			//}
			//m_Bitmap.set_data_opera(draw_image);
			//m_Bitmap.set_data_opera(ret_mat);

			m_Bitmap.canny(m_CannyParam.threshold1, m_CannyParam.threshold2, m_CannyParam.aperture, m_CannyParam.l2);

			ResetBitmap();
		}
	}
};
