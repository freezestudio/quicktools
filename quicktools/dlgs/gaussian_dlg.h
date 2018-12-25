#pragma once

namespace freeze {

	class CGaussianDlg : public ATL::CDialogImpl<CGaussianDlg>
	{
	public:
		enum { IDD = IDD_GAUSSIAN };

		// Kernel
		WTL::CEdit m_KernelX;
		WTL::CEdit m_KernelY;
		WTL::CButton m_CheckKernelY;

		// Sigma
		WTL::CEdit m_SigmaX;
		WTL::CEdit m_SigmaY;
		WTL::CButton m_CheckSigmaY;

		// BorderType
		WTL::CComboBox m_ComboBorderType;

		// 接收此对话框发出的消息的窗口
		HWND m_RecvMsgWnd = nullptr;

		BEGIN_MSG_MAP_EX(CCannyDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
			COMMAND_ID_HANDLER(IDC_BUTTON_RESET_GAUSSIAN, OnReset)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_KERNEL_Y, OnKernelYCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_SIGMA_Y, OnSigmaYCheck)
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


		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			CenterWindow(GetParent());

			m_KernelX.Attach(GetDlgItem(IDC_EDIT_KERNEL_X));
			m_KernelY.Attach(GetDlgItem(IDC_EDIT_KERNEL_Y));
			m_CheckKernelY.Attach(GetDlgItem(IDC_CHECK_KERNEL_Y));

			m_SigmaX.Attach(GetDlgItem(IDC_EDIT_SIGMA_X));
			m_SigmaY.Attach(GetDlgItem(IDC_EDIT_SIGMA_Y));
			m_CheckSigmaY.Attach(GetDlgItem(IDC_CHECK_SIGMA_Y));

			m_ComboBorderType.Attach(GetDlgItem(IDC_COMBO_BORDER_TYPE));

			SetDlgItemInt(IDC_EDIT_KERNEL_X, 3);
			SetDlgItemInt(IDC_EDIT_KERNEL_Y, 3);

			SetDlgItemInt(IDC_EDIT_SIGMA_X, 0);
			SetDlgItemInt(IDC_EDIT_SIGMA_Y, 0);

			//默认;特定值;复制;镜像;回卷;镜像缩进;透明;隔离
			wchar_t combo_item[][5] = {
				L"默认",
				L"特定值",
				L"复制",
				L"镜像",
				L"回卷",
				L"镜像缩进",
				L"透明",
				L"隔离",
			};
			std::for_each(std::begin(combo_item), std::end(combo_item), [this](auto item) {
				this->m_ComboBorderType.AddString(item);
			});
			m_ComboBorderType.SetCurSel(0);

			return TRUE;
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			switch (nID)
			{
			default:
				break;
			case IDC_EDIT_KERNEL_X: // X核值
				OnKernelXChanged(uNotifyCode);
				break;
			case IDC_EDIT_KERNEL_Y: // Y核值

				break;
			case IDC_EDIT_SIGMA_X: // X标准偏差
				OnSigmaXChanged(uNotifyCode);
				break;
			case IDC_EDIT_SIGMA_Y: // Y标准偏差
				break;
			case IDC_COMBO_BORDER_TYPE: // 边框类型
				OnBorderTypeChanged(uNotifyCode);
				break;
			}
		}

		void OnBorderTypeChanged(UINT uNotifyCode)
		{
			int selected_index = -1;
			switch (uNotifyCode)
			{
			default:
				break;
			case CBN_SELCHANGE:
				selected_index = m_ComboBorderType.GetCurSel();
				break;
			}

			if (m_RecvMsgWnd)
			{
				auto w = MAKEWPARAM(IDC_COMBO_BORDER_TYPE, selected_index);
				::PostMessage(m_RecvMsgWnd, WM_GAUSSIAN, w, 0);
			}
		}

		void OnKernelXChanged(UINT uNotifyCode)
		{
			IDC_EDIT_KERNEL_X;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_KERNEL_X);
				if (value % 2 == 0)
				{
					value += 1;
					SetDlgItemInt(IDC_EDIT_KERNEL_X, value);
				}
				SetDlgItemInt(IDC_EDIT_KERNEL_Y, value);

				if (m_RecvMsgWnd)
				{
					// 注意：默认X,Y值相等，如果不相等，则特殊处理Y值
					auto w = MAKEWPARAM(IDC_EDIT_KERNEL_X, value);
					::PostMessage(m_RecvMsgWnd, WM_GAUSSIAN, w, 0);
				}
			}
		}

		void OnSigmaXChanged(UINT uNotifyCode)
		{
			IDC_EDIT_SIGMA_X;
			if (uNotifyCode == EN_CHANGE)
			{
				if (m_RecvMsgWnd)
				{
					auto value = GetDlgItemInt(IDC_EDIT_SIGMA_X);
					auto w = MAKEWPARAM(IDC_EDIT_SIGMA_X, value);
					::PostMessage(m_RecvMsgWnd,WM_GAUSSIAN, w, 0);
				}
			}
		}

		void OnKernelYCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			if (!m_RecvMsgWnd)return;

			IDC_CHECK_KERNEL_Y;
			auto checked = m_CheckKernelY.GetCheck() == BST_CHECKED;
			m_KernelY.EnableWindow(checked);
		}

		void OnSigmaYCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			if (!m_RecvMsgWnd)return;
			IDC_CHECK_SIGMA_Y;
			auto checked = m_CheckSigmaY.GetCheck() == BST_CHECKED;
			m_SigmaY.EnableWindow(checked);
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}

		LRESULT OnReset(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if (!m_RecvMsgWnd)return 0;

			//m_Track1.SetPos(V_THRESHOLD_1);
			//m_Track2.SetPos(V_THRESHOLD_2);
			//SetDlgItemInt(IDC_THRESHOLD_EDIT_1, V_THRESHOLD_1);
			//SetDlgItemInt(IDC_THRESHOLD_EDIT_2, V_THRESHOLD_2);
			//SetDlgItemInt(IDC__APERTURE_EDIT, V_APERTURE);
			//m_L2Gradient.SetCheck(BST_UNCHECKED);

			//auto w = MAKEWPARAM(ID_RESET_BTN, 0);
			//::PostMessage(m_RecvMsgWnd, WM_CANNY, w, 0);

			return 0;
		}
	};
}
