#pragma once

namespace freeze {

	class CLaplacianOfGaussianDlg : public ATL::CDialogImpl<CLaplacianOfGaussianDlg>
	{
	public:
		enum { IDD = IDD_DIALOG_LOG };

		// Kernel
		WTL::CTrackBarCtrl m_TrackKernelSize; // IDC_SLIDER_LOG_KERNEL
		WTL::CEdit m_KernelSize;

		// BorderType
		//WTL::CComboBox m_ComboBorderType;

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

			m_TrackKernelSize.Attach(GetDlgItem(IDC_SLIDER_LOG_KERNEL));
			m_TrackKernelSize.SetRange(1, 31);
			m_TrackKernelSize.SetPos(V_KERNEL_X);

			m_KernelSize.Attach(GetDlgItem(IDC_EDIT_LOG_KERNEL));

			//m_ComboBorderType.Attach(GetDlgItem(IDC_COMBO_BORDER_TYPE));

			SetDlgItemInt(IDC_EDIT_LOG_KERNEL, V_KERNEL_X);


			////默认;特定值;复制;镜像;回卷;镜像缩进;透明;隔离
			//wchar_t combo_item[][5] = {
			//	L"默认",
			//	L"特定值",
			//	L"复制",
			//	L"镜像",
			//	L"回卷",
			//	L"镜像缩进",
			//	L"透明",
			//	L"隔离",
			//};
			//std::for_each(std::begin(combo_item), std::end(combo_item), [this](auto item) {
			//	this->m_ComboBorderType.AddString(item);
			//});
			//m_ComboBorderType.SetCurSel(0);

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
			switch (nID)
			{
			default:
				break;
			case IDC_EDIT_LOG_KERNEL: // 核大小
				OnKernelSizeChanged(uNotifyCode);
				break;
			//case IDC_COMBO_BORDER_TYPE: // 边框类型
			//	OnBorderTypeChanged(uNotifyCode);
			//	break;
			case IDC_EDIT_THRESHOLD_LOG: // 减影阈值
				OnThresholdChanged(uNotifyCode);
				break;
			}
		}

		//void OnBorderTypeChanged(UINT uNotifyCode)
		//{
		//	int selected_index = -1;
		//	switch (uNotifyCode)
		//	{
		//	default:
		//		break;
		//	case CBN_SELCHANGE:
		//		selected_index = m_ComboBorderType.GetCurSel();
		//		if (m_RecvMsgWnd && (selected_index != -1))
		//		{
		//			auto w = MAKEWPARAM(IDC_COMBO_BORDER_TYPE, selected_index);
		//			::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
		//		}
		//		break;
		//	}
		//}

		void OnKernelSizeChanged(UINT uNotifyCode)
		{
			IDC_EDIT_LOG_KERNEL;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_LOG_KERNEL);
				if (value % 2 == 0)
				{
					value += 1;
					SetDlgItemInt(IDC_EDIT_LOG_KERNEL, value);
				}

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

		void OnMinusLoGCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_CHECK_LOG_MINUS;
			auto checked = m_CheckMinusLoG.GetCheck() == BST_CHECKED;
			m_ThresholdLoG.EnableWindow(checked);
			m_TrackThresholdLoG.EnableWindow(checked);

			if (m_RecvMsgWnd)
			{
				auto w = MAKEWPARAM(IDC_CHECK_LOG_MINUS, checked ? 1 : 0);
				::PostMessage(m_RecvMsgWnd, WM_LAPLACIAN_OF_GAUSSIAN, w, 0);
			}
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
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

