// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

inline freeze::CannyParam g_CannyParam{
	V_THRESHOLD_1,
	V_THRESHOLD_2,
	V_APERTURE,
	false,
};

inline freeze::GaussianParam g_GaussianParam{
	V_KERNEL_X,
	V_KERNEL_Y,
	V_SIGMA_X,
	V_SIGMA_X,
	V_BORDER_TYPE,
};

inline freeze::ThresholdParam g_ThresholdParam{
	128,
	255,
	0,
};

class CView :
	public WTL::CScrollWindowImpl<CView>
{
public:
	DECLARE_WND_CLASS(L"OpenCV_ImageView")

	freeze::bmp_image m_Bitmap;

	// TODO: �Ƿ��Զ���ʾ�ο�ͼ��
	bool m_ShowRefImage = false;
	// ����|���ü�Ӱ
	bool m_EnableMinus = false;

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
		//MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER_EX(WM_CANNY, OnCanny)
		MESSAGE_HANDLER_EX(WM_GAUSSIAN, OnGaussian)
		MSG_WM_DROPFILES(OnDropFiles)
		CHAIN_MSG_MAP(WTL::CScrollWindowImpl<CView>)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void SetCannyParam()
	{
		//m_CannyParam;
	}

	// �����ڲο�ͼ��(�ο�ͼ����һ��ָ���ı�׼ͼ��)
	void ShowRefImage(bool show = false)
	{
		m_ShowRefImage = show;
		m_Bitmap.set_use_ref_data(m_ShowRefImage);
	}

	// ���ü�Ӱ����(��˹ģ��ʱ)
	void EnableMinus(bool enable = false)
	{
		m_EnableMinus = enable;
		m_Bitmap.set_use_threshold(enable);
	}

	void SetBitmap2(std::wstring const& file, std::wstring const& detectfile = L"", bool auto_scroll = true)
	{
		m_Bitmap.raw_file(file);

		if (!detectfile.empty())
		{
			m_Bitmap.defect_file(detectfile);
			//m_Bitmap.create_defect_bitmap(GetDC());
		}

		if (m_Bitmap.is_auto_use_operator())
		{
			m_Bitmap.canny(
				g_CannyParam.threshold1,
				g_CannyParam.threshold2,
				g_CannyParam.aperture,
				g_CannyParam.l2
			);
		}

		if (m_Bitmap.is_auto_use_gaussian())
		{
			m_Bitmap.gaussian_blur(
				g_GaussianParam.x,
				g_GaussianParam.y,
				g_GaussianParam.sx,
				g_GaussianParam.sy,
				g_GaussianParam.type
			);

			if (m_EnableMinus)
			{
				m_Bitmap.minus_image();
				if (m_Bitmap.is_use_threshold())
				{
					m_Bitmap.threshold(
						g_ThresholdParam.threshold,
						g_ThresholdParam.max_value,
						g_ThresholdParam.type
					);
				}
			}
		}

		ResetBitmap();

		if (auto_scroll)
		{
			SetScroll();
		}
	}

	// ���òο�ͼ��
	void SetRefBitmap(std::wstring const& ref_file)
	{
		m_Bitmap.reference_file(ref_file);
		//m_Bitmap.create_ref_opera_bitmap(GetDC());
	}

	// ��������ͼ��
	void ResetBitmap()
	{
		m_Bitmap.create_all_image(GetDC());
		m_Bitmap.set_use_ref_data(m_ShowRefImage);

		InvalidateRect(nullptr);
		UpdateWindow();
	}

	// �Ϸ�ȱ��ͼ���ļ�
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

		// �����Ϸ�֧��
		::DragAcceptFiles(this->m_hWnd, TRUE);
		// �����Զ�Ӧ������
		m_Bitmap.set_auto_use_operator();
		// �����Զ�Ӧ�ø�˹ģ��
		m_Bitmap.set_auto_use_gaussian();

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
			m_Bitmap.draw/*_ex*/(dc, left);
		}
	}

	// Canny���Ӳ���
	LRESULT OnCanny(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		AdjustCannyParam((UINT)HIWORD(wParam), (UINT)LOWORD(wParam));
		return 0;
	}

	// Gaussian���Ӳ���
	LRESULT OnGaussian(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		AdjustGaussianParam((UINT)HIWORD(wParam), (UINT)LOWORD(wParam));
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
		case IDC_THRESHOLD_EDIT_1: // ��ֵ1
			g_CannyParam.threshold1 = value;
			break;
		case IDC_THRESHOLD_EDIT_2: // ��ֵ2
			g_CannyParam.threshold2 = value;
			break;
		case IDC__APERTURE_EDIT: // �׶�
			g_CannyParam.aperture = value;
			break;
		case IDC_L2GRADIENT_CHECK: // L2
			g_CannyParam.l2 = value == 1 ? true : false;
			break;
		case ID_RESET_BTN: // ����Canny����
			g_CannyParam.threshold1 = threshold1;
			g_CannyParam.threshold2 = threshold2;
			g_CannyParam.aperture = aperture;
			g_CannyParam.l2 = l2;
			break;
		case IDC_CHECK_RESET_RAW://ԭʼͼ��
			if (m_Bitmap)
			{
				m_Bitmap.exclude_other_image(value);
			}
			break;
		case IDC_CHECK_NO_DEFECT://ȱ��ͼ��ͼ��
			if (m_Bitmap)
			{
				m_Bitmap.exclude_defect_image(!value);
			}
			break;
		case IDC_CHECK_REF_IMAGE://ʹ�òο�ͼ��(����)
			ShowRefImage(value);
			break;
		}

		if (m_Bitmap)
		{
			if (!m_Bitmap.show_raw_only())
			{
				m_Bitmap.canny(
					g_CannyParam.threshold1,
					g_CannyParam.threshold2,
					g_CannyParam.aperture,
					g_CannyParam.l2
				);
			}

			ResetBitmap();
		}
	}

	void AdjustGaussianParam(UINT value, UINT nID)
	{
		int x = 3;
		int y = 3;
		int sx = 0;
		int type = 4;

		unsigned char threshold = 0;

		switch (nID)
		{
		default:
			break;
		case IDC_EDIT_KERNEL_X: // x
			g_GaussianParam.x = value;
			g_GaussianParam.y = value;
			break;
		case IDC_EDIT_KERNEL_Y: // y
			g_GaussianParam.y = value;
			break;
		case IDC_EDIT_SIGMA_X: // sx
			g_GaussianParam.sx = value;
			g_GaussianParam.sy = value;
			break;
		case IDC_EDIT_SIGMA_Y: // L2
			g_GaussianParam.sy = value;
			break;
		case IDC_COMBO_BORDER_TYPE:
			g_GaussianParam.type = freeze::convert_type(value);
			break;
		case IDC_CHECK_MINUS: // ����|���ü�Ӱ
			EnableMinus(value ? true : false);
			break;
		case IDC_EDIT_THRESHOLD_MID: // ��Ӱ��ֵ
			g_ThresholdParam.threshold = static_cast<unsigned char>(value);
			break;
		case ID_RESET_BTN: // ����Gaussian����
			g_GaussianParam.x = x;
			g_GaussianParam.y = y;
			g_GaussianParam.sx = g_GaussianParam.sy = sx;
			g_GaussianParam.type = type;
			break;
		//case IDC_CHECK_RESET_RAW://ԭʼͼ��
		//	if (m_Bitmap)
		//	{
		//		m_Bitmap.exclude_other_image(value);
		//	}
		//	break;
		//case IDC_CHECK_NO_DEFECT://ȱ��ͼ��ͼ��
		//	if (m_Bitmap)
		//	{
		//		m_Bitmap.exclude_defect_image(!value);
		//	}
		//	break;
		//case IDC_CHECK_REF_IMAGE://ʹ�òο�ͼ��(����)
		//	ShowRefImage(value);
		//	break;
		}

		if (m_Bitmap)
		{
			if (!m_Bitmap.show_raw_only())
			{
				m_Bitmap.gaussian_blur(
					g_GaussianParam.x,
					g_GaussianParam.y,
					g_GaussianParam.sx,
					g_GaussianParam.sy,
					g_GaussianParam.type
				);

				if (m_EnableMinus)
				{
					m_Bitmap.minus_image();
					if (m_Bitmap.is_use_threshold())
					{
						m_Bitmap.threshold(
							g_ThresholdParam.threshold,
							g_ThresholdParam.max_value,
							g_ThresholdParam.type
						);
					}
				}
			}

			ResetBitmap();
		}
	}

};
