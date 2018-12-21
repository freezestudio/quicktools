#pragma once

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

		WTL::CButton m_L2Gradient; //L2梯度
		WTL::CButton m_ResetBtn; // 重置
		WTL::CButton m_ResetRawBtn; // 只显示原始图像
		WTL::CButton m_NoDefectBtn; // 显示检测图像
		WTL::CButton m_ResetRefBtn; // 参考图像

		// 接收此对话框发出的消息的窗口
		HWND m_RecvMsgWnd = nullptr;

		BEGIN_MSG_MAP_EX(CCannyDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
			COMMAND_ID_HANDLER(ID_RESET_BTN, OnReset)
			COMMAND_ID_HANDLER_EX(IDC_L2GRADIENT_CHECK, OnGradientCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_RESET_RAW, OnResetRawCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_NO_DEFECT, OnNoDefectCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_REF_IMAGE, OnResetRefCheck)
			MSG_WM_HSCROLL(OnHScroll)
			MSG_WM_COMMAND(OnCommand)
		END_MSG_MAP()

		// Handler prototypes (uncomment arguments if needed):
		//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

		void SetRecvMessageWindow(HWND hwnd)
		{
			m_RecvMsgWnd = hwnd;
		}

		void SetDetectChecked(bool check = true)
		{
			m_NoDefectBtn.SetCheck(check ? BST_CHECKED:BST_UNCHECKED);
		}

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
			m_ResetRefBtn.Attach(GetDlgItem(IDC_CHECK_REF_IMAGE));

			return TRUE;
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			switch (nID)
			{
			default:
				break;
			case IDC_THRESHOLD_EDIT_1: // 阈值1
				OnThreshold1(uNotifyCode);
				break;
			case IDC_THRESHOLD_EDIT_2: // 阈值2
				OnThreshold2(uNotifyCode);
				break;
			case IDC__APERTURE_EDIT: // 孔洞
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
			if (!m_RecvMsgWnd)return;

			IDC_L2GRADIENT_CHECK;
			auto checked = m_L2Gradient.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_L2GRADIENT_CHECK, checked ? TRUE : FALSE);
			::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
		}

		void OnResetRawCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			if (!m_RecvMsgWnd)return;
			IDC_CHECK_RESET_RAW;
			auto checked = m_ResetRawBtn.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_CHECK_RESET_RAW, checked ? TRUE : FALSE);
			::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
		}

		void OnNoDefectCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			if (!m_RecvMsgWnd)return;
			IDC_CHECK_NO_DEFECT;
			auto checked = m_NoDefectBtn.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_CHECK_NO_DEFECT, checked ? TRUE : FALSE);
			::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
		}

		void OnResetRefCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			if (!m_RecvMsgWnd)return;
			IDC_CHECK_REF_IMAGE;
			auto checked = m_ResetRefBtn.GetCheck() == BST_CHECKED;
			auto w = MAKEWPARAM(IDC_CHECK_REF_IMAGE, checked ? TRUE : FALSE);
			::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
		}

		void OnThreshold1(UINT uNotifyCode)
		{
			if (!m_RecvMsgWnd)return;

			IDC_THRESHOLD_EDIT_1;
			if (EN_CHANGE == uNotifyCode)
			{
				auto value = GetDlgItemInt(IDC_THRESHOLD_EDIT_1);
				auto w = MAKEWPARAM(IDC_THRESHOLD_EDIT_1, value);
				::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
			}
		}

		void OnThreshold2(UINT uNotifyCode)
		{
			if (!m_RecvMsgWnd)return;

			IDC_THRESHOLD_EDIT_2;
			if (EN_CHANGE == uNotifyCode)
			{
				auto value = GetDlgItemInt(IDC_THRESHOLD_EDIT_2);
				auto w = MAKEWPARAM(IDC_THRESHOLD_EDIT_2, value);
				::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
			}
		}

		void OnAperture(UINT uNotifyCode)
		{
			if (!m_RecvMsgWnd)return;

			IDC__APERTURE_EDIT;
			if (EN_CHANGE == uNotifyCode)
			{
				auto value = GetDlgItemInt(IDC__APERTURE_EDIT);
				auto w = MAKEWPARAM(IDC__APERTURE_EDIT, value);
				::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);
			}
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}

		LRESULT OnReset(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if (!m_RecvMsgWnd)return 0;

			m_Track1.SetPos(V_THRESHOLD_1);
			m_Track2.SetPos(V_THRESHOLD_2);
			SetDlgItemInt(IDC_THRESHOLD_EDIT_1, V_THRESHOLD_1);
			SetDlgItemInt(IDC_THRESHOLD_EDIT_2, V_THRESHOLD_2);
			SetDlgItemInt(IDC__APERTURE_EDIT, V_APERTURE);
			m_L2Gradient.SetCheck(BST_UNCHECKED);

			auto w = MAKEWPARAM(ID_RESET_BTN, 0);
			::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);

			return 0;
		}
	};
}
