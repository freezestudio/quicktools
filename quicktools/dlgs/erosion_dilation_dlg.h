#pragma once

namespace freeze {

	class CErosionDilationDlg : public ATL::CDialogImpl<CErosionDilationDlg>
	{
	public:
		enum { IDD = IDD_DIALOG_ED };

		WTL::CButton m_RadioErosion;//IDC_RADIO_EROSION
		WTL::CButton m_RadioDilation;//IDC_RADIO_DILATION

		WTL::CTrackBarCtrl m_TrackKernelSize; //IDC_SLIDER_ED_KERNEL_SIZE
		WTL::CEdit m_EditKernelSize;//IDC_EDIT_ED_KERNEL_SIZE

		WTL::CComboBox m_ComboShape; //IDC_COMBO_ED_SHAPE
		// BorderType
		WTL::CComboBox m_ComboBorderType; //IDC_COMBO_ED_BORDER_TYPE

		WTL::CEdit m_EditIterCount; //IDC_EDIT_ITER_COUNT

		// 接收此对话框发出的消息的窗口
		HWND m_RecvMsgWnd = nullptr;

		BEGIN_MSG_MAP_EX(CErosionDilationDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
			COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
			COMMAND_ID_HANDLER(ID_RESET_ED, OnEDReset)
			COMMAND_ID_HANDLER_EX(IDC_RADIO_EROSION, OnErosionCheck)
			COMMAND_ID_HANDLER_EX(IDC_RADIO_DILATION, OnDilationCheck)
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

		bool CanUseErosion() const
		{
			return m_RadioErosion.GetCheck() == BST_CHECKED;
		}

		bool CanUseDilation() const
		{
			return m_RadioDilation.GetCheck() == BST_CHECKED;
		}

		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			CenterWindow(GetParent());

			m_RadioErosion.Attach(GetDlgItem(IDC_RADIO_EROSION));//IDC_RADIO_EROSION
			m_RadioErosion.SetCheck(BST_CHECKED);
			m_RadioDilation.Attach(GetDlgItem(IDC_RADIO_DILATION));//IDC_RADIO_DILATION

			m_TrackKernelSize.Attach(GetDlgItem(IDC_SLIDER_ED_KERNEL_SIZE)); //IDC_SLIDER_ED_KERNEL_SIZE
			m_TrackKernelSize.SetRange(1, 31);
			m_TrackKernelSize.SetPos(1);
			m_EditKernelSize.Attach(GetDlgItem(IDC_EDIT_ED_KERNEL_SIZE));//IDC_EDIT_ED_KERNEL_SIZE
			SetDlgItemInt(IDC_EDIT_ED_KERNEL_SIZE, 1);

			m_ComboShape.Attach(GetDlgItem(IDC_COMBO_ED_SHAPE)); //IDC_COMBO_ED_SHAPE
			m_ComboBorderType.Attach(GetDlgItem(IDC_COMBO_ED_BORDER_TYPE));//IDC_COMBO_ED_BORDER_TYPE

			m_EditIterCount.Attach(GetDlgItem(IDC_EDIT_ITER_COUNT)); //IDC_EDIT_ITER_COUNT
			SetDlgItemInt(IDC_EDIT_ITER_COUNT, 1);

			//cv::MORPH_RECT;
			//cv::MORPH_CROSS;
			//cv::MORPH_ELLIPSE;
			wchar_t shape_item[][3] = {
				L"矩形",
				L"十字",
				L"椭圆"
			};
			std::for_each(std::begin(shape_item), std::end(shape_item), [this](auto item) {
				this->m_ComboShape.AddString(item);
			});
			m_ComboShape.SetCurSel(0);

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
			m_ComboBorderType.SetCurSel(1);

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
				case IDC_SLIDER_ED_KERNEL_SIZE:
					SetDlgItemInt(IDC_EDIT_ED_KERNEL_SIZE, pos % 2 == 0 ? pos + 1 : pos);
					break;
					break;
				}
			}
			break;
			}
		}

		void OnCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			//WTL::CEdit m_EditKernelSize;//IDC_EDIT_ED_KERNEL_SIZE
			//WTL::CComboBox m_ComboShape; //IDC_COMBO_ED_SHAPE
			//WTL::CComboBox m_ComboBorderType; //IDC_COMBO_ED_BORDER_TYPE
			//WTL::CEdit m_EditIterCount; //IDC_EDIT_ITER_COUNT

			switch (nID)
			{
			default:
				break;
			case IDC_EDIT_ED_KERNEL_SIZE: // 核大小
				OnKernelSizeChanged(uNotifyCode);
				break;
			case IDC_COMBO_ED_SHAPE: // 形状
				OnShapeChanged(uNotifyCode);
				break;
			case IDC_COMBO_LOG_BORDER_TYPE: // 边框类型
				OnBorderTypeChanged(uNotifyCode);
				break;
			case IDC_EDIT_ITER_COUNT: // 迭代次数
				OnIterCountChanged(uNotifyCode);
				break;
			}
		}

		void OnKernelSizeChanged(UINT uNotifyCode)
		{
			IDC_EDIT_ED_KERNEL_SIZE;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_ED_KERNEL_SIZE);
				if (value % 2 == 0)
				{
					value += 1;
					SetDlgItemInt(IDC_EDIT_ED_KERNEL_SIZE, value);
				}

				if (m_RecvMsgWnd)
				{
					// 注意：默认X,Y值相等，如果不相等，则特殊处理Y值
					auto w = MAKEWPARAM(IDC_EDIT_ED_KERNEL_SIZE, value);
					::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
				}
			}
		}

		void OnShapeChanged(UINT uNotifyCode)
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
					auto w = MAKEWPARAM(IDC_COMBO_ED_SHAPE, selected_index);
					::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
				}
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
				if (m_RecvMsgWnd && (selected_index != -1))
				{
					auto w = MAKEWPARAM(IDC_COMBO_LOG_BORDER_TYPE, selected_index);
					::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
				}
				break;
			}
		}

		void OnIterCountChanged(UINT uNotifyCode)
		{
			IDC_EDIT_ITER_COUNT;
			if (uNotifyCode == EN_CHANGE)
			{
				auto value = GetDlgItemInt(IDC_EDIT_ITER_COUNT);
				if (value % 2 == 0)
				{
					value += 1;
					SetDlgItemInt(IDC_EDIT_ITER_COUNT, value);
				}

				if (m_RecvMsgWnd)
				{
					auto w = MAKEWPARAM(IDC_EDIT_ITER_COUNT, value);
					::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
				}
			}
		}

		void OnErosionCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_RADIO_EROSION;
			auto checked = m_RadioErosion.GetCheck();
			if (m_RecvMsgWnd)
			{
				auto w = MAKEWPARAM(IDC_RADIO_EROSION, checked);
				::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
			}
		}

		void OnDilationCheck(UINT uNotifyCode, int nID, CWindow wndCtl)
		{
			IDC_RADIO_DILATION;
			auto checked = m_RadioDilation.GetCheck();
			if (m_RecvMsgWnd)
			{
				auto w = MAKEWPARAM(IDC_RADIO_DILATION, checked);
				::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
			}
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			auto w = MAKEWPARAM(wID, SW_HIDE);
			::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);
			return 0;
		}

		LRESULT OnEDReset(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if (!m_RecvMsgWnd)return 0;

			//m_Track1.SetPos(V_THRESHOLD_1);
			//m_Track2.SetPos(V_THRESHOLD_2);
			//SetDlgItemInt(IDC_THRESHOLD_EDIT_1, V_THRESHOLD_1);
			//SetDlgItemInt(IDC_THRESHOLD_EDIT_2, V_THRESHOLD_2);
			//SetDlgItemInt(IDC__APERTURE_EDIT, V_APERTURE);
			//m_L2Gradient.SetCheck(BST_UNCHECKED);

			//auto w = MAKEWPARAM(ID_RESET_BTN, 0);
			//::PostMessage(m_RecvMsgWnd, WM_EROSION_DILATION, w, 0);

			return 0;
		}
	};
}

