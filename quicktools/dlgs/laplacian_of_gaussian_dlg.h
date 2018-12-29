#pragma once

namespace freeze {

	class CLaplacianOfGaussianDlg : public ATL::CDialogImpl<CLaplacianOfGaussianDlg>
	{
	public:
		enum { IDD = IDD_DIALOG_LOG };

		// Gaussian Kernel

		WTL::CTrackBarCtrl m_TrackGX; //IDC_SLIDER_G_X

		WTL::CEdit m_EditGX; //IDC_EDIT_G_KERNEL_X
		WTL::CButton m_CheckGY; //IDC_CHECK_G_KERNEL_Y
		WTL::CEdit m_EditGY;//IDC_EDIT_G_KERNEL_Y

		WTL::CEdit m_EditSigmaX;//IDC_EDIT_G_SIGMA_X
		WTL::CButton m_CheckSigmaY;//IDC_CHECK_G_SIGMA_Y
		WTL::CEdit m_EditSigmaY;//IDC_EDIT_G_SIGMA_Y

		// BorderType
		WTL::CComboBox m_ComboBorderType; //IDC_COMBO_LOG_BORDER_TYPE
		// ThresholdType
		WTL::CComboBox m_ComboThresholdType; //IDC_COMBO_THRESHOLD_TYPE_LOG

		// Laplacian Kernel

		WTL::CTrackBarCtrl m_TrackKernelSize; // IDC_SLIDER_LOG_KERNEL
		WTL::CEdit m_KernelSize;//IDC_EDIT_LOG_KERNEL
		WTL::CEdit m_EditAlpha;//IDC_EDIT_LOG_ALPHA
		WTL::CEdit m_editBeta;//IDC_EDIT_LOG_BETA

		// 减影
		WTL::CButton m_CheckMinusLoG;//IDC_CHECK_MINUS_LOG
		WTL::CEdit m_ThresholdLoG;//阈值 IDC_EDIT_THRESHOLD_LOG
		WTL::CTrackBarCtrl m_TrackThresholdLoG;// IDC_SLIDER_THRESHOLD_LOG

		// 接收此对话框发出的消息的窗口
		HWND m_RecvMsgWnd = nullptr;

		BEGIN_MSG_MAP_EX(CLaplacianOfGaussianDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
			COMMAND_ID_HANDLER(ID_RESET_LOG, OnLoGReset)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_G_KERNEL_Y, OnGYCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_G_SIGMA_Y, OnSYCheck)
			COMMAND_ID_HANDLER_EX(IDC_CHECK_LOG_MINUS, OnMinusLoGCheck)
			MSG_WM_COMMAND(OnCommand)
			MSG_WM_HSCROLL(OnHScroll)
		END_MSG_MAP()

		// Handler prototypes (uncomment arguments if needed):
		//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

		void SetRecvMessageWindow(HWND hwnd)
		{
			m_RecvMsgWnd = hwnd;
		}

		bool CanUseLoG() const
		{
			return m_CheckMinusLoG.GetCheck() == BST_CHECKED;
		}

		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			CenterWindow(GetParent());

			// Gaussian
			m_TrackGX.Attach(GetDlgItem(IDC_SLIDER_G_X)); //IDC_SLIDER_G_X
			m_TrackGX.SetRange(1, 31);
			m_TrackGX.SetPos(3);

			m_EditGX.Attach(GetDlgItem(IDC_EDIT_G_KERNEL_X)); //IDC_EDIT_G_KERNEL_X
			SetDlgItemInt(IDC_EDIT_G_KERNEL_X, 3);
			m_CheckGY.Attach(GetDlgItem(IDC_CHECK_G_KERNEL_Y)); //IDC_CHECK_G_KERNEL_Y
			m_EditGY.Attach(GetDlgItem(IDC_EDIT_G_KERNEL_Y));//IDC_EDIT_G_KERNEL_Y
			SetDlgItemInt(IDC_EDIT_G_KERNEL_Y, 3);

			m_EditSigmaX.Attach(GetDlgItem(IDC_EDIT_G_SIGMA_X));//IDC_EDIT_G_SIGMA_X
			SetDlgItemInt(IDC_EDIT_G_SIGMA_X, 0);
			m_CheckSigmaY.Attach(GetDlgItem(IDC_CHECK_G_SIGMA_Y));//IDC_CHECK_G_SIGMA_Y
			m_EditSigmaY.Attach(GetDlgItem(IDC_EDIT_G_SIGMA_Y));//IDC_EDIT_G_SIGMA_Y
			SetDlgItemInt(IDC_EDIT_G_SIGMA_Y, 0);

			m_ComboBorderType.Attach(GetDlgItem(IDC_COMBO_LOG_BORDER_TYPE));
			m_ComboThresholdType.Attach(GetDlgItem(IDC_COMBO_THRESHOLD_TYPE_LOG)); //IDC_COMBO_THRESHOLD_TYPE_LOG
			// Laplace

			m_TrackKernelSize.Attach(GetDlgItem(IDC_SLIDER_LOG_KERNEL));
			m_TrackKernelSize.SetRange(1, 11);
			m_TrackKernelSize.SetPos(V_KERNEL_X);

			m_KernelSize.Attach(GetDlgItem(IDC_EDIT_LOG_KERNEL));
			SetDlgItemInt(IDC_EDIT_LOG_KERNEL, V_KERNEL_X);

			m_EditAlpha.Attach(GetDlgItem(IDC_EDIT_LOG_ALPHA));//IDC_EDIT_LOG_ALPHA
			SetDlgItemText(IDC_EDIT_LOG_ALPHA, L"1.0");
			m_editBeta.Attach(GetDlgItem(IDC_EDIT_LOG_BETA));//IDC_EDIT_LOG_BETA
			SetDlgItemText(IDC_EDIT_LOG_BETA, L"0.0");

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

			wchar_t combo_threshold[][11] = {
				L"BINARY",
				L"BINARY_INV",
				L"TRUNC",
				L"TOZERO",
				L"TOZERO_INV",
				L"MASK",
				//L"OTSU",
				//L"TRIANGLE",
			};
			std::for_each(std::begin(combo_threshold), std::end(combo_threshold), [this](auto item) {
				this->m_ComboThresholdType.AddString(item);
			});
			m_ComboThresholdType.SetCurSel(0);

			// 减影
			m_TrackThresholdLoG.Attach(GetDlgItem(IDC_SLIDER_THRESHOLD_LOG));
			m_TrackThresholdLoG.SetRange(0, 255);
			m_TrackThresholdLoG.SetPos(128);

			m_CheckMinusLoG.Attach(GetDlgItem(IDC_CHECK_LOG_MINUS));

			m_ThresholdLoG.Attach(GetDlgItem(IDC_EDIT_THRESHOLD_LOG));
			SetDlgItemInt(IDC_EDIT_THRESHOLD_LOG, 128);

			return TRUE;
		}

		void OnHScroll(UINT nSBCode, UINT nPos, WTL::CTrackBarCtrl pTrackBar)
		{
			switch (nSBCode)
			{
			default:
				break;
			case TB_THUMBPOSITION:
			case TB_THUMBTRACK: // TB_ENDTRACK
			{
				auto pos = pTrackBar.GetPos();
				auto id = pTrackBar.GetDlgCtrlID();
				switch (id)
				{
				default:
					break;
				case IDC_SLIDER_G_X:
					SetDlgItemInt(IDC_EDIT_G_KERNEL_X, pos % 2 == 0 ? pos + 1 : pos);
					break;
				case IDC_SLIDER_LOG_KERNEL:
					SetDlgItemInt(IDC_EDIT_LOG_KERNEL, pos % 2 == 0 ? pos + 1 : pos);
					break;
				case IDC_SLIDER_THRESHOLD_LOG:
					SetDlgItemInt(IDC_EDIT_THRESHOLD_LOG, pos);
					break;
				}
			}
			break;
			}
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			//WTL::CEdit m_EditGX; //IDC_EDIT_G_KERNEL_X
			//WTL::CEdit m_EditGY;//IDC_EDIT_G_KERNEL_Y

			//WTL::CEdit m_EditSigmaX;//IDC_EDIT_G_SIGMA_X
			//WTL::CEdit m_EditSigmaY;//IDC_EDIT_G_SIGMA_Y

			//// BorderType
			//WTL::CComboBox m_ComboBorderType; //IDC_COMBO_LOG_BORDER_TYPE

			//// Laplacian Kernel

			//WTL::CEdit m_KernelSize;//IDC_EDIT_LOG_KERNEL
			//WTL::CEdit m_EditAlpha;//IDC_EDIT_LOG_ALPHA
			//WTL::CEdit m_editBeta;//IDC_EDIT_LOG_BETA

			switch (nID)
			{
			default:
				break;
			case IDC_EDIT_G_KERNEL_X: // 高斯核X
				OnGXChanged(uNotifyCode);
				break;
			case IDC_EDIT_G_KERNEL_Y: // 高斯核Y
				OnGYChanged(uNotifyCode);
				break;
			case IDC_EDIT_G_SIGMA_X: // 标准方差X
				OnSXChanged(uNotifyCode);
				break;
			case IDC_EDIT_G_SIGMA_Y: // 标准方差Y
				OnSYChanged(uNotifyCode);
				break;
			case IDC_EDIT_LOG_ALPHA: // 缩放系数
				OnAlphaChanged(uNotifyCode);
				break;
			case IDC_EDIT_LOG_BETA: // 偏差量
				OnBetaChanged(uNotifyCode);
				break;
			case IDC_EDIT_LOG_KERNEL: // Laplace核大小
				OnKernelSizeChanged(uNotifyCode);
				break;
			case IDC_COMBO_LOG_BORDER_TYPE: // 边框类型
				OnBorderTypeChanged(uNotifyCode);
				break;
			case IDC_COMBO_THRESHOLD_TYPE_LOG: // 边框类型
				OnThresholdTypeChanged(uNotifyCode);
				break;
			case IDC_EDIT_THRESHOLD_LOG: // 减影阈值
				OnThresholdChanged(uNotifyCode);
				break;
			}
		}

		void OnGXChanged(UINT uNotifyCode)
		{
			IDC_EDIT_G_KERNEL_X;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_G_KERNEL_X);
				if (value % 2 == 0)
				{
					value += 1;
					SetDlgItemInt(IDC_EDIT_G_KERNEL_X, value);
				}
				SetDlgItemInt(IDC_EDIT_G_KERNEL_Y, value);

				if (m_RecvMsgWnd)
				{
					// 注意：默认X,Y值相等，如果不相等，则特殊处理Y值
					auto w = MAKEWPARAM(IDC_EDIT_G_KERNEL_X, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}

		void OnGYChanged(UINT uNotifyCode)
		{
			IDC_EDIT_G_KERNEL_Y;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_G_KERNEL_Y);
				if (value % 2 == 0)
				{
					value += 1;
					SetDlgItemInt(IDC_EDIT_G_KERNEL_Y, value);
				}

				if (m_RecvMsgWnd)
				{
					auto w = MAKEWPARAM(IDC_EDIT_G_KERNEL_Y, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}

		void OnSXChanged(UINT uNotifyCode)
		{
			IDC_EDIT_G_SIGMA_X;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_G_SIGMA_X);
				SetDlgItemInt(IDC_EDIT_G_SIGMA_Y, value);

				if (m_RecvMsgWnd)
				{
					// 注意：默认X,Y值相等，如果不相等，则特殊处理Y值
					auto w = MAKEWPARAM(IDC_EDIT_G_SIGMA_X, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}

		void OnSYChanged(UINT uNotifyCode)
		{
			IDC_EDIT_G_SIGMA_Y;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_G_SIGMA_Y);
				if (m_RecvMsgWnd)
				{
					auto w = MAKEWPARAM(IDC_EDIT_G_SIGMA_Y, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}

		void OnAlphaChanged(UINT uNotifyCode)
		{
			IDC_EDIT_LOG_ALPHA;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_LOG_ALPHA);
				if (m_RecvMsgWnd)
				{
					auto w = MAKEWPARAM(IDC_EDIT_LOG_ALPHA, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}

		void OnBetaChanged(UINT uNotifyCode)
		{
			IDC_EDIT_LOG_BETA;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_LOG_BETA);
				if (m_RecvMsgWnd)
				{
					auto w = MAKEWPARAM(IDC_EDIT_LOG_BETA, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
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
				if (m_RecvMsgWnd && (selected_index != -1))
				{
					auto w = MAKEWPARAM(IDC_COMBO_LOG_BORDER_TYPE, selected_index);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
				break;
			}
		}

		//
		void OnThresholdTypeChanged(UINT uNotifyCode)
		{
			int selected_index = -1;
			switch (uNotifyCode)
			{
			default:
				break;
			case CBN_SELCHANGE:
				selected_index = m_ComboThresholdType.GetCurSel();
				if (m_RecvMsgWnd && (selected_index != -1))
				{
					auto w = MAKEWPARAM(IDC_COMBO_THRESHOLD_TYPE_LOG, selected_index);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
				break;
			}
		}

		void OnKernelSizeChanged(UINT uNotifyCode)
		{
			IDC_EDIT_LOG_KERNEL;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_LOG_KERNEL);
				//if (value % 2 == 0)
				//{
				//	value += 1;
				//	SetDlgItemInt(IDC_EDIT_LOG_KERNEL, value);
				//}

				if (m_RecvMsgWnd)
				{
					// 注意：默认X,Y值相等，如果不相等，则特殊处理Y值
					auto w = MAKEWPARAM(IDC_EDIT_LOG_KERNEL, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}
		
		void OnThresholdChanged(UINT uNotifyCode)
		{
			IDC_EDIT_THRESHOLD_LOG;
			if (uNotifyCode == EN_CHANGE)
			{
				if (m_RecvMsgWnd)
				{
					auto value = GetDlgItemInt(IDC_EDIT_THRESHOLD_LOG);
					auto w = MAKEWPARAM(IDC_EDIT_THRESHOLD_LOG, value);
					::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
				}
			}
		}

		void OnGYCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_CHECK_G_KERNEL_Y;
			auto checked = m_CheckMinusLoG.GetCheck() == BST_CHECKED;
			m_EditGY.EnableWindow(checked);
		}

		void OnSYCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_CHECK_G_SIGMA_Y;
			auto checked = m_CheckMinusLoG.GetCheck() == BST_CHECKED;
			m_EditSigmaY.EnableWindow(checked);
		}

		void OnMinusLoGCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_CHECK_LOG_MINUS;
			auto checked = m_CheckMinusLoG.GetCheck() == BST_CHECKED;
			m_ThresholdLoG.EnableWindow(checked);
			m_TrackThresholdLoG.EnableWindow(checked);
			m_ComboThresholdType.EnableWindow(checked);

			if (m_RecvMsgWnd)
			{
				auto w = MAKEWPARAM(IDC_CHECK_LOG_MINUS, checked ? 1 : 0);
				::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
			}
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			auto w = MAKEWPARAM(wID, SW_HIDE);
			::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
			return 0;
		}

		LRESULT OnLoGReset(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if (!m_RecvMsgWnd)return 0;

			//m_Track1.SetPos(V_THRESHOLD_1);
			//m_Track2.SetPos(V_THRESHOLD_2);
			//SetDlgItemInt(IDC_THRESHOLD_EDIT_1, V_THRESHOLD_1);
			//SetDlgItemInt(IDC_THRESHOLD_EDIT_2, V_THRESHOLD_2);
			//SetDlgItemInt(IDC__APERTURE_EDIT, V_APERTURE);
			//m_L2Gradient.SetCheck(BST_UNCHECKED);

			//auto w = MAKEWPARAM(ID_RESET_BTN, 0);
			//::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);

			return 0;
		}
	};
}
