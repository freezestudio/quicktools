// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0600
#define _WIN32_WINNT	0x0600
#define _WIN32_IE	0x0600
#define _RICHEDIT_VER	0x0300

#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _WTL_NO_AUTOMATIC_NAMESPACE

#include <atlbase.h>
#include <atlapp.h>
#include <atlimage.h>

extern WTL::CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlsplit.h>
#include <atlscrl.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <atlcrack.h>

#include <opencv2/opencv.hpp>

#include <omp.h>
#include <filesystem>

// 自定义消息

// see directorybar.h

// 更改图像目录
#define WM_DIRECTORY_CHANGED WM_USER + 100
#define WM_DETECTDIRECTORY_CHANGED + 101
// 重置列表视图
#define WM_RESET_LISTVIEW    WM_USER + 102

// see custom_checked_listview.h

// 打开一个图像文件
#define WM_OPEN_IMAGE WM_USER + 103

// see canny_dlg

// 改变Canny算子参数
#define WM_CANNY WM_USER + 104

// see image_data.h

// 完成一次Canny算子的运算时
#define WM_CANNY_FINISH WM_USER + 105

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#ifdef NDEBUG
#    pragma comment(lib,"D:\\opencv\\build\\x64\\vc15\\lib\\opencv_world400.lib")
#else
#    pragma comment(lib,"D:\\opencv\\build\\x64\\vc15\\lib\\opencv_world400d.lib")
#endif
