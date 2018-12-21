#pragma once

#define V_THRESHOLD_1 106
#define V_THRESHOLD_2 66
#define V_APERTURE 3
#define V_L2 false

// 封装图像数据的类

namespace freeze {
	inline namespace {

		inline HANDLE g_hEvent=nullptr;

		void init_event()
		{
			if (g_hEvent)
			{
				CloseHandle(g_hEvent);
			}
			g_hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
			//CloseHandle(g_hEvent);
		}

		struct CannyParam
		{
			double threshold1 = V_THRESHOLD_1;
			double threshold2 = V_THRESHOLD_2;
			int    aperture = V_APERTURE;
			bool   l2 = V_L2;
		};

		struct CannyWithArrayParam : CannyParam
		{
			DWORD idThread;
			cv::InputArray inArray;
			cv::InputOutputArray outArray;
		};

		inline static DWORD CALLBACK AsyncOpera(LPVOID pParam)
		{
			CannyWithArrayParam* pCannyParam = reinterpret_cast<CannyWithArrayParam*>(pParam);
			auto result = WaitForSingleObject(g_hEvent, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				cv::Mat ret_mat;
				cv::Canny(pCannyParam->inArray, ret_mat,
					pCannyParam->threshold1, pCannyParam->threshold2, pCannyParam->aperture, pCannyParam->l2);

				
				std::vector<std::vector<cv::Point>> contours;
				std::vector<cv::Vec4i> hierarchy;

				cv::findContours(ret_mat, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
				cv::Scalar color{ 0.0,0.0,255.0, };

#pragma omp parallel for
				for (int i = 0; i < contours.size(); ++i)
				{
					cv::drawContours(pCannyParam->outArray, contours, i, color/*, 2, cv::LINE_8, hierarchy, 0*/);
				}

				//PostThreadMessage(pCannyParam->idThread, WM_CANNY_FINISH, 0, 0);
				ResetEvent(g_hEvent);
				return TRUE;
			}
			return FALSE;
		}
	}
	template<typename ImageData = WTL::CBitmap>
	class image_any
	{
	public:
		enum class image_type {
			IMREAD_UNCHANGED = -1, //!< If set, return the loaded image as is (with alpha channel, otherwise it gets cropped).
			IMREAD_GRAYSCALE = 0,  //!< If set, always convert image to the single channel grayscale image (codec internal conversion).
			IMREAD_COLOR = 1,  //!< If set, always convert image to the 3 channel BGR color image.
			IMREAD_ANYDEPTH = 2,  //!< If set, return 16-bit/32-bit image when the input has the corresponding depth, otherwise convert it to 8-bit.
			IMREAD_ANYCOLOR = 4,  //!< If set, the image is read in any possible color format.
			IMREAD_LOAD_GDAL = 8,  //!< If set, use the gdal driver for loading the image.
			IMREAD_REDUCED_GRAYSCALE_2 = 16, //!< If set, always convert image to the single channel grayscale image and the image size reduced 1/2.
			IMREAD_REDUCED_COLOR_2 = 17, //!< If set, always convert image to the 3 channel BGR color image and the image size reduced 1/2.
			IMREAD_REDUCED_GRAYSCALE_4 = 32, //!< If set, always convert image to the single channel grayscale image and the image size reduced 1/4.
			IMREAD_REDUCED_COLOR_4 = 33, //!< If set, always convert image to the 3 channel BGR color image and the image size reduced 1/4.
			IMREAD_REDUCED_GRAYSCALE_8 = 64, //!< If set, always convert image to the single channel grayscale image and the image size reduced 1/8.
			IMREAD_REDUCED_COLOR_8 = 65, //!< If set, always convert image to the 3 channel BGR color image and the image size reduced 1/8.
			IMREAD_IGNORE_ORIENTATION = 128 //!< If set, do not rotate the image according to EXIF's orientation flag.
		};

		explicit image_any(std::wstring const& image_file = L"", HDC dc = nullptr, bool auto_convert = true)
		{
			init_event();
			if (!image_file.empty())
			{
				from_file(image_file);
				if (dc && auto_convert)
				{
					(void)convert(dc);
				}
			}
		}

		explicit operator bool() const
		{
			return !image_data_raw.empty();
		}

		// 操作
	public:
		// 原始图像文件
		void from_file(std::wstring const& image_file, image_type flag = image_type::IMREAD_COLOR)
		{
			auto file = convert_from(image_file);
			from_file_internal(file, flag);
		}

		// 缺陷图像文件
		void defect_file(std::wstring const& defectfile, image_type flag = image_type::IMREAD_COLOR)
		{
			auto file = convert_from(defectfile);
			from_file_internal_defect(file, flag);
		}

		void convert(HDC dc, int action = 0)
		{
			create_raw_bitmap(dc);

			switch (action)
			{
			default:
				break;
			case 1:
				create_opera_bitmap(dc);
				break;
			case 2:
				create_section(dc);
				break;
			case 3:
				create_dib(dc);
				break;
			case 4:
				create_dib2(dc);
				break;
			}
		}

		// 创建缺陷图像位图
		void create_defect_bitmap(HDC dc)
		{
			reset_defect_image();

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			defect_image.CreateDIBitmap(dc, &bitmap_info->bmiHeader, CBM_INIT, image_data_defect.data, bitmap_info, DIB_RGB_COLORS);
		}

		// 创建原始图像位图
		void create_raw_bitmap(HDC dc)
		{
			if (!raw_image)
			{
				// 注意，这里使用了原始数据的克隆。
				auto clone_image = mat_raw_clone();

				unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
				auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
				freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

				raw_image.CreateDIBitmap(dc, &bitmap_info->bmiHeader, CBM_INIT, clone_image.data, bitmap_info, DIB_RGB_COLORS);
			}
		}

		// 创建算子位图
		void create_opera_bitmap(HDC dc)
		{
			reset_opera_image();

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			opera_image.CreateDIBitmap(dc, &bitmap_info->bmiHeader, CBM_INIT, image_data_opera.data, bitmap_info, DIB_RGB_COLORS);
		}

		// 创建算子位图 调用得不到正确的结果
		void create_section(HDC dc)
		{
			reset_opera_image();
			// 注意，这里使用了原始数据的克隆。
			auto clone_image = mat_raw_clone();

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			opera_image.CreateDIBSection(dc, bitmap_info, DIB_PAL_COLORS, (void**)(&image_data_opera.data), nullptr, 0);
		}

		// 创建算子位图 重新生成位图并赋值底层数据
		void create_dib(HDC dc)
		{
			reset_opera_image();

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			opera_image.CreateCompatibleBitmap(dc, width(), height());
			opera_image.SetDIBits(dc, 0, height(), image_data_opera.data, bitmap_info, DIB_RGB_COLORS);
		}

		// 创建算子位图 直接替换或赋值底层数据
		void create_dib2(HDC dc)
		{
			if (opera_image.IsNull())
			{
				opera_image.CreateCompatibleBitmap(dc, width(), height());
			}

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			opera_image.SetDIBits(dc, 0, height(), image_data_opera.data, bitmap_info, DIB_RGB_COLORS);
		}

		void canny(double threshold1, double threshold2, int aperture, bool l2 = false,DWORD thread_id=0)
		{
//			// 每次运行，算子数据都需要重置为原始数据的副本
//			reset_data_opera();
//
//			cv::Mat ret_mat;
//			cv::Canny(image_data_opera, ret_mat, threshold1, threshold2, aperture, l2);
//			std::vector<std::vector<cv::Point>> contours;
//			std::vector<cv::Vec4i> hierarchy;
//			cv::findContours(ret_mat, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
//			cv::Scalar color{ 0.0,0.0,255.0,0.0 };
//			//cv::Mat draw_mat = cv::Mat::zeros(ret_mat.size(), CV_8UC3);
//			//cv::UMat draw_image;
//			//draw_mat.copyTo(draw_image);
//			cv::Mat draw_image = cv::Mat::zeros(ret_mat.size(), CV_8UC3);
//			auto ts = cv::getTickCount();
//#pragma omp parallel for
//			for (int i = 0; i < contours.size(); ++i)
//			{
//				cv::drawContours(draw_image, contours, i, color/*, 2, cv::LINE_8, hierarchy, 0*/);
//			}
//			auto es = cv::getTickCount();
//			auto times = (es - ts) / cv::getTickFrequency();
//			//set_data_opera(draw_image.getMat(cv::ACCESS_READ));
//			set_data_opera(draw_image);

			// 每次运行，算子数据都需要重置为原始数据的副本
			reset_data_opera();

			cv::Mat out_mat = cv::Mat::zeros(image_data_opera.size(), CV_8UC3);
			CannyWithArrayParam param = {
				threshold1,threshold2,aperture,l2,
				thread_id,image_data_opera,out_mat,
			};
			SetEvent(g_hEvent);
			auto hThread = CreateThread(nullptr, 0, AsyncOpera, &param, 0, nullptr);
			auto result = WaitForSingleObject(hThread, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				set_data_opera(out_mat);
			}
		}

		void laplacian()
		{

		}

		void sobel()
		{

		}

		// 属性
	public:
		int width() const
		{
			return (image_data_raw.rows);
		}

		int height() const
		{
			return (image_data_raw.cols);
		}

		CSize size() const
		{
			auto s = image_data_raw.size();
			return { s.width,s.height };
		}

		int channels() const
		{
			return (image_data_raw.channels());
		}

		int bpp() const
		{
			return channels() * image_any<ImageData>::depth;
		}

		cv::Mat mat_raw() const
		{
			return image_data_raw;
		}

		cv::Mat mat_raw_clone() const
		{
			return image_data_raw.clone();
		}

		void set_data_opera(cv::Mat const& mat)
		{
			image_data_opera = mat/*.clone()*/;
		}

		// 重置数据--从原始数据中
		void reset_data_opera()
		{
			image_data_opera = image_data_raw.clone();
		}

		unsigned char* raw_data() const
		{
			return image_data_raw.data();
		}

		unsigned char* raw_data()
		{
			return image_data_raw.data();
		}

		// 是否排除其它图像的显示
		void exclude_other_image(bool exclude = true)
		{
			exclude_other = exclude;
		}

		// 是否排除缺陷图像的显示
		void exclude_defect_image(bool exclude = true)
		{
			exclude_defect = exclude;
		}
	public:
		bool draw(HDC dc, int offset_x = 0, int offset_y = 0)
		{
			if (raw_image.IsNull())
			{
				return false;
			}

			// 双缓冲
			WTL::CDCHandle temp_dc;
			temp_dc.CreateCompatibleDC(dc);
			temp_dc.SaveDC();

			WTL::CMemoryDC mem_dc{ dc,CRect{offset_x,offset_y,width() + offset_x,height() + offset_y} };

			// 绘制原始图像
			if (!raw_image.IsNull())
			{
				temp_dc.SelectBitmap(raw_image);
				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);
			}

			// 排除其它图像时，直接返回
			if (exclude_other)
			{
				temp_dc.RestoreDC(-1);
				return true;
			}

			// 绘制缺陷图像
			if (!exclude_defect && !defect_image.IsNull())
			{
				temp_dc.SelectBitmap(defect_image);
				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCPAINT);
			}

			// 绘制算子图像
			if (!opera_image.IsNull())
			{
				temp_dc.SelectBitmap(opera_image);
				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCPAINT);
			}

			temp_dc.RestoreDC(-1);
			return true;
		}

		bool draw_ex(HDC dc, int offset_x = 0, int offset_y = 0)
		{
			if (raw_image.IsNull())
			{
				return false;
			}

			// 双缓冲
			WTL::CDCHandle temp_dc;
			temp_dc.CreateCompatibleDC(dc);
			temp_dc.SaveDC();

			CRect image_rect = { 0,0,width(),height() };
			CRect out = image_rect;
			out.OffsetRect(offset_x, offset_y);
			WTL::CMemoryDC mem_dc{ dc,out };

			WTL::CBitmap temp_bitmap;
			// 注意创建与原始dc兼容的位图
			temp_bitmap.CreateCompatibleBitmap(dc, width(), height());
			temp_dc.SelectBitmap(temp_bitmap);

			// 绘制原始图像
			if (!raw_image.IsNull())
			{
				WTL::CDC raw_dc;
				raw_dc.CreateCompatibleDC(temp_dc);
				raw_dc.SaveDC();
				raw_dc.SelectBitmap(raw_image);
				temp_dc.BitBlt(0, 0, width(), height(), raw_dc, 0, 0, SRCCOPY);
				raw_dc.RestoreDC(-1);
			}

			// 排除其它图像时，直接返回
			if (exclude_other)
			{
				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);
				temp_dc.RestoreDC(-1);
				return true;
			}

			// 绘制缺陷图像
			if (!exclude_defect && !defect_image.IsNull())
			{
				WTL::CDC defect_dc;
				defect_dc.CreateCompatibleDC(temp_dc);
				defect_dc.SaveDC();
				defect_dc.SelectBitmap(defect_image);

				temp_dc.BitBlt(0, 0, width(), height(), defect_dc, 0, 0, SRCPAINT);
				defect_dc.RestoreDC(-1);
			}

			// 绘制算子图像
			if (!opera_image.IsNull())
			{
				WTL::CDC opera_dc;
				opera_dc.CreateCompatibleDC(temp_dc);
				opera_dc.SaveDC();
				opera_dc.SelectBitmap(opera_image);
				temp_dc.BitBlt(0, 0, width(), height(), opera_dc, 0, 0, SRCPAINT);
				opera_dc.RestoreDC(-1);
			}

			mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);
			temp_dc.RestoreDC(-1);

			return true;
		}

	private:
		void from_file_internal(std::string const& image_file, image_type flag)
		{
			// 原始数据
			image_data_raw = load_image(image_file, static_cast<int>(flag));
			// 用于算子的副本
			//image_data_opera = image_data_raw.clone();
		}

		void from_file_internal_defect(std::string const& image_file, image_type flag)
		{
			// 缺陷数据
			image_data_defect = load_image(image_file, static_cast<int>(flag));
		}

		void reset_opera_image()
		{
			if (!opera_image.IsNull())
			{
				opera_image.DeleteObject();
			}
		}

		void reset_defect_image()
		{
			if (!defect_image.IsNull())
			{
				defect_image.DeleteObject();
			}
		}

	private:
		constexpr static auto depth = 8;
		// BMP 位图的平面数必须为1
		constexpr static auto planes = 1;

		cv::Mat image_data_raw;
		cv::Mat image_data_opera;
		cv::Mat image_data_defect;

		ImageData opera_image; // 算子图像
		ImageData raw_image; // 原始图像
		ImageData defect_image; //缺陷图像

		bool exclude_other = false; // 排除其它图像的显示，仅显示原始图像
		bool exclude_defect = false; // 单独排除缺陷图像的显示

		//bool use_operator = false;
	};

	using bmp_image = image_any<>;
}