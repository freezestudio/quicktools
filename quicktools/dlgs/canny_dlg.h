#pragma once

#define WM_CANNY WM_USER + 103

//double threshold1 = 106.0;
//double threshold2 = 66.0;
//int    aperture = 3;
//bool   l2 = false;

#define V_THRESHOLD_1 106
#define V_THRESHOLD_2 66
#define V_APERTURE 3
#define V_L2 false

namespace freeze {

	class CCannyDlg : public ATL::CDialogImpl<CCannyDlg>
	{
	public:
		enum { IDD = IDD_CANNY };

		WTL::CTrackBarCtrl m_Track1;
		WTL::CTrackBarCtrl m_Track2;
		WTL::CEdit m_Threshold1;
		WTL::CEdit m_Threshold2;
		WTL::CEdit m_Aperture;
		WTL::CButton m_L2Gradient;
		WTL::CButton m_ResetBtn;
		WTL::CButton m_ResetRawBtn;
		WTL::CButton m_NoDefectBtn;

		BEGIN_MSG_MAP_EX(CCannyDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
			COMMAND_ID_HANDLER(ID_RESET_BTN, OnReset)
			COMMAND_ID_HANDLER_EX(IDC_L2GRADIENT_CHECK, OnGradientCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_RESET_RAW,OnResetRawCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_NO_DEFECT, OnNoDefectCheck)
			MSG_WM_HSCROLL(OnHScroll)
			MSG_WM_COMMAND(OnCommand)
		END_MSG_MAP()

		// Handler prototypes (uncomment arguments if needed):
		//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			CenterWindow(GetParent());

			m_Track1.Attach(GetDlgItem(IDC_THRESHOLD_SLIDER_1));
			m_Track1.SetRange(1, 400);
			m_Track1.SetPos(V_THRESHOLD_1);
			m_Track2.Attach(GetDlgItem(IDC_THRESHOLD_SLIDER_2));
			m_Track2.SetRange(1, 500);
			m_Track2.SetPos(V_THRESHOLD_2);
			m_Threshold1.Attach(GetDlgItem(IDC_THRESHOLD_EDIT_1));
			SetDlgItemInt(IDC_THRESHOLD_EDIT_1, V_THRESHOLD_1);
			m_Threshold2.Attach(GetDlgItem(IDC_THRESHOLD_EDIT_2));
			SetDlgItemInt(IDC_THRESHOLD_EDIT_2, V_THRESHOLD_2);
			m_Aperture.Attach(GetDlgItem(IDC__APERTURE_EDIT));
			SetDlgItemInt(IDC__APERTURE_EDIT, 3);
			m_L2Gradient.Attach(GetDlgItem(IDC_L2GRADIENT_CHECK));

			m_ResetBtn.Attach(GetDlgItem(ID_RESET_BTN));
			m_ResetRawBtn.Attach(GetDlgItem(IDC_CHECK_RESET_RAW));
			m_NoDefectBtn.Attach(GetDlgItem(IDC_CHECK_NO_DEFECT));

			return TRUE;
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			switch (nID)
			{
			default:
				break;
			case IDC_THRESHOLD_EDIT_1: // ��ֵ1
				OnThreshold1(uNotifyCode);
				break;
			case IDC_THRESHOLD_EDIT_2: // ��ֵ2
				OnThreshold2(uNotifyCode);
				break;
			case IDC__APERTURE_EDIT: // �׶�
				OnAperture(uNotifyCode);
				break;
			}
		}

		void OnHScroll(UINT nSBCode, UINT nPos, WTL::CTrackBarCtrl pTrackBar)
		{
			switch (nSBCode)
			{
			default:
				break;
			case TB_ENDTRACK: // TB_THUMBTRACK
			{
				auto pos = pTrackBar.GetPos();
				auto id = pTrackBar.GetDlgCtrlID();
				switch (id)
				{
				default:
					break;
				case IDC_THRESHOLD_SLIDER_1:
					SetDlgItemInt(IDC_THRESHOLD_EDIT_1, pos);
					break;
				case IDC_THRESHOLD_SLIDER_2:
					SetDlgItemInt(IDC_THRESHOLD_EDIT_2, pos);
					break;
				}
			}
			break;
			}
		}

		void OnGradientCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_L2GRADIENT_CHECK;
			auto checked = m_L2Gradient.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_L2GRADIENT_CHECK, checked ? TRUE : FALSE);
			GetParent().PostMessage(WM_CANNY, w, 0);
		}

		void OnResetRawCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_CHECK_RESET_RAW;
			auto checked = m_ResetRawBtn.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_CHECK_RESET_RAW, checked ? TRUE : FALSE);
			GetParent().PostMessage(WM_CANNY, w, 0);
		}

		void OnNoDefectCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_CHECK_NO_DEFECT;
			auto checked = m_NoDefectBtn.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_CHECK_NO_DEFECT, checked ? TRUE : FALSE);
			GetParent().PostMessage(WM_CANNY, w, 0);
		}

		void OnThreshold1(UINT uNotifyCode)
		{
			IDC_THRESHOLD_EDIT_1;
			if (EN_CHANGE == uNotifyCode)
			{
				auto value = GetDlgItemInt(IDC_THRESHOLD_EDIT_1);
				auto w = MAKEWPARAM(IDC_THRESHOLD_EDIT_1, value);
				GetParent().PostMessage(WM_CANNY, w, 0);
			}
		}

		void OnThreshold2(UINT uNotifyCode)
		{
			IDC_THRESHOLD_EDIT_2;
			if (EN_CHANGE == uNotifyCode)
			{
				auto value = GetDlgItemInt(IDC_THRESHOLD_EDIT_2);
				auto w = MAKEWPARAM(IDC_THRESHOLD_EDIT_2, value);
				GetParent().PostMessage(WM_CANNY, w, 0);
			}
		}

		void OnAperture(UINT uNotifyCode)
		{
			IDC__APERTURE_EDIT;
			if (EN_CHANGE == uNotifyCode)
			{
				auto value = GetDlgItemInt(IDC__APERTURE_EDIT);
				auto w = MAKEWPARAM(IDC__APERTURE_EDIT, value);
				GetParent().PostMessage(WM_CANNY, w, 0);
			}
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}

		LRESULT OnReset(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			m_Track1.SetPos(V_THRESHOLD_1);
			m_Track2.SetPos(V_THRESHOLD_2);
			SetDlgItemInt(IDC_THRESHOLD_EDIT_1, V_THRESHOLD_1);
			SetDlgItemInt(IDC_THRESHOLD_EDIT_2, V_THRESHOLD_2);
			SetDlgItemInt(IDC__APERTURE_EDIT, V_APERTURE);
			m_L2Gradient.SetCheck(BST_UNCHECKED);

			auto w = MAKEWPARAM(ID_RESET_BTN, 0);
			GetParent().PostMessage(WM_CANNY, w, 0);

			return 0;
		}
	};

}