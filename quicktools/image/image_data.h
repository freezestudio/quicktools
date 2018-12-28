#pragma once

#define V_THRESHOLD_1 106
#define V_THRESHOLD_2 66
#define V_APERTURE 3
#define V_L2 false

#define V_KERNEL_X 3
#define V_KERNEL_Y 3
#define V_SIGMA_X 0
#define V_SIGMA_X 0
#define V_BORDER_TYPE 0

// 封装图像数据的类

namespace freeze {
	inline namespace {

		struct CannyParam
		{
			double threshold1 = V_THRESHOLD_1;
			double threshold2 = V_THRESHOLD_2;
			int    aperture = V_APERTURE;
			bool   l2 = V_L2;
		};

		struct CannyWithArrayParam : CannyParam
		{
			HANDLE hEvent;
			cv::InputArray inArray;
			cv::InputOutputArray outArray;
			cv::Scalar color;
		};

		struct GaussianParam
		{
			int x;
			int y;
			double sx;
			double sy;
			int type;
		};

		struct GaussianWithArrayParam : public GaussianParam
		{
			HANDLE hEvent;
			cv::InputArray inArray;
			cv::/*Input*/OutputArray outArray;
		};

		struct ThresholdParam
		{
			unsigned char threshold;
			unsigned char max_value;
			int type;//cv::BORDER_DEFAULT
		};

		struct ThresholdWithArrayParam : ThresholdParam
		{
			HANDLE hEvent;
			cv::InputArray inArray;
			cv::/*Input*/OutputArray outArray;
		};

		struct LaplacianOfGaussianParam
		{
			int depth;
			int ksize;
			double scale;// 1
			double delta;// 0
			int type;// cv::BORDER_DEFAULT
		};

		struct LaplacianOfGaussianWithArrayParam : LaplacianOfGaussianParam
		{
			HANDLE hEvent;
			cv::InputArray inArray;
			cv::InputOutputArray outArray;
		};

		inline int convert_type(int n)
		{
			switch (n)
			{
			default:return cv::BORDER_DEFAULT;
			case 0: return cv::BORDER_DEFAULT;
			case 1: return cv::BORDER_CONSTANT;
			case 2: return cv::BORDER_REPLICATE;
			case 3: return cv::BORDER_REFLECT;
			case 4: return cv::BORDER_WRAP;
			case 5: return cv::BORDER_REFLECT_101;
			case 6: return cv::BORDER_TRANSPARENT;
			case 7: return cv::BORDER_ISOLATED;
			}
		}

		inline static DWORD CALLBACK AsyncOpera(LPVOID pParam)
		{
			CannyWithArrayParam* pCannyParam = reinterpret_cast<CannyWithArrayParam*>(pParam);
			auto result = WaitForSingleObject(pCannyParam->hEvent, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				cv::Mat ret_mat;
				try
				{
					cv::Canny(pCannyParam->inArray, ret_mat,
						pCannyParam->threshold1, pCannyParam->threshold2, pCannyParam->aperture, pCannyParam->l2);
				}
				catch (const std::exception& e)
				{
					auto reason = e.what();
					SetEvent(pCannyParam->hEvent);
					return FALSE;
				}

				std::vector<std::vector<cv::Point>> contours;
				std::vector<cv::Vec4i> hierarchy;

				cv::findContours(ret_mat, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
				//cv::Scalar color{ 0.0,0.0,255.0, };

#pragma omp parallel for
				for (int i = 0; i < contours.size(); ++i)
				{
					cv::drawContours(pCannyParam->outArray, contours, i, pCannyParam->color, 2, cv::LINE_4/*, hierarchy, 0*/);
				}

				//PostThreadMessage(pCannyParam->idThread, WM_CANNY_FINISH, 0, 0);
				SetEvent(pCannyParam->hEvent);
				return TRUE;
			}
			return FALSE;
		}

		inline static DWORD CALLBACK AsyncGaussian(LPVOID pParam)
		{
			GaussianWithArrayParam* pGaussianParam = reinterpret_cast<GaussianWithArrayParam*>(pParam);
			auto result = WaitForSingleObject(pGaussianParam->hEvent, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				try
				{
					cv::Size size = { pGaussianParam->x,pGaussianParam->y, };
					cv::GaussianBlur(pGaussianParam->inArray, pGaussianParam->outArray,
						size, pGaussianParam->sx, pGaussianParam->sy, pGaussianParam->type);
				}
				catch (const std::exception& e)
				{
					auto reason = e.what();
					SetEvent(pGaussianParam->hEvent);
					return FALSE;
				}

				SetEvent(pGaussianParam->hEvent);
				return TRUE;
			}
			return FALSE;
		}

		inline static DWORD CALLBACK AsyncThreshold(LPVOID pParam)
		{
			ThresholdWithArrayParam* pThreshold = reinterpret_cast<ThresholdWithArrayParam*>(pParam);
			auto result = WaitForSingleObject(pThreshold->hEvent, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				cv::Mat ret_mat;
				try
				{
					cv::threshold(pThreshold->inArray, pThreshold->outArray,
						pThreshold->threshold,
						pThreshold->max_value,
						pThreshold->type
					);
				}
				catch (const std::exception& e)
				{
					auto reason = e.what();
					MessageBoxA(nullptr, reason, "error", MB_OK);
					SetEvent(pThreshold->hEvent);
					return FALSE;
				}

				SetEvent(pThreshold->hEvent);
				return TRUE;
			}
			return FALSE;
		}

		inline static DWORD CALLBACK AsyncLaplacianOfGaussian(LPVOID pParam)
		{
			LaplacianOfGaussianWithArrayParam* pLoGParam = reinterpret_cast<LaplacianOfGaussianWithArrayParam*>(pParam);
			auto result = WaitForSingleObject(pLoGParam->hEvent, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				try
				{
					cv::Laplacian(pLoGParam->inArray, pLoGParam->outArray, CV_8U, pLoGParam->ksize,
						pLoGParam->scale, pLoGParam->delta, pLoGParam->type);

					//cv::Mat blur_mat;
					//cv::Laplacian(pLoGParam->inArray, blur_mat, CV_8U, pLoGParam->ksize,
					//	pLoGParam->scale, pLoGParam->delta, pLoGParam->type);
					//cv::convertScaleAbs(blur_mat, pLoGParam->outArray);
				}
				catch (const std::exception& e)
				{
					auto reason = e.what();
					SetEvent(pLoGParam->hEvent);
					return FALSE;
				}

				SetEvent(pLoGParam->hEvent);
				return TRUE;
			}
			return FALSE;
		}

	} // end inline namespace

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
			init_event(FALSE);
			if (!image_file.empty())
			{
				raw_file(image_file);
				if (dc && auto_convert)
				{
					(void)create_all_image(dc);
				}
			}
		}

		~image_any()
		{
			CloseHandle(opear_event);
		}

		explicit operator bool() const
		{
			return !image_data_raw.empty();
		}

		// 操作
	public:
		// 原始图像文件
		void raw_file(std::wstring const& image_file, image_type flag = image_type::IMREAD_COLOR)
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

		// 参考图像文件(如果有)
		void reference_file(std::wstring const& ref_file, image_type flag = image_type::IMREAD_COLOR)
		{
			auto file = convert_from(ref_file);
			from_file_internal_ref(file, flag);
		}

		// 创建所有图像的显示图像
		void create_all_image(HDC dc)
		{
			create_raw_bitmap(dc);

			if (exclude_other)return;

			if (!exclude_defect)
			{
				create_defect_bitmap(dc);
			}

			//if (use_operator)
			//{
			create_opera_bitmap(dc);
			//}

			if (use_ref_image_data)
			{
				create_ref_opera_bitmap(dc);
			}

			if (use_threshold)
			{
				create_threshold_bitmap(dc);
			}
		}

		// 创建原始图像位图
		void create_raw_bitmap(HDC dc)
		{
			reset_raw_image();

			// 注意，这里使用了原始数据的克隆。
			//auto clone_image = mat_raw_clone();

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			raw_image.CreateDIBitmap(
				dc,
				&bitmap_info->bmiHeader,
				CBM_INIT,
				/*clone_image.data*/image_data_raw.data,
				bitmap_info,
				DIB_RGB_COLORS
			);

		}

		// 创建缺陷图像位图
		void create_defect_bitmap(HDC dc)
		{
			reset_defect_image();
			if (image_data_defect.empty())return;

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			defect_image.CreateDIBitmap(dc, &bitmap_info->bmiHeader, CBM_INIT, image_data_defect.data, bitmap_info, DIB_RGB_COLORS);
		}

		// 创建参考图像算子位图
		void create_ref_opera_bitmap(HDC dc)
		{
			reset_ref_opera_image();

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			ref_opera_image.CreateDIBitmap(
				dc,
				&bitmap_info->bmiHeader,
				CBM_INIT,
				image_data_ref_opera.data,
				bitmap_info,
				DIB_RGB_COLORS
			);
		}

		// 创建算子位图 直接替换或赋值底层数据
		void create_opera_bitmap(HDC dc)
		{
			//if (opera_image.IsNull())
			//{
			//	opera_image.CreateCompatibleBitmap(dc, width(), height());
			//}

			//unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			//auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			//freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			//auto ret = opera_image.SetDIBits(
			//	dc,
			//	0,
			//	height(),
			//	image_data_opera.data,
			//	bitmap_info,
			//	DIB_RGB_COLORS
			//);
			//if (ret == 0)
			//{
			//	auto err = ::GetLastError();
			//	ERROR_INVALID_PARAMETER;
			//}

			reset_opera_image();
			if (image_data_opera.empty())return;

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			opera_image.CreateDIBitmap(
				dc,
				&bitmap_info->bmiHeader,
				CBM_INIT,
				image_data_opera.data,
				bitmap_info,
				DIB_RGB_COLORS
			);
		}

		// 创建二值化位图 直接替换或赋值底层数据
		void create_threshold_bitmap(HDC dc)
		{
			if (threshold_image.IsNull())
			{
				threshold_image.CreateCompatibleBitmap(dc, width(), height());
			}

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			threshold_image.SetDIBits(
				dc,
				0,
				height(),
				image_data_threshold.data,
				bitmap_info,
				DIB_RGB_COLORS
			);
		}

		// 运算
	public:
		void canny(double threshold1, double threshold2, int aperture, bool l2 = false)
		{
			set_event();
			if (use_ref_image_data && !image_data_ref.empty())
			{
				// 每次运行，算子数据都需要重置为原始数据的副本
				reset_data_ref_opera();
				cv::Mat out_ref_mat = cv::Mat::zeros(
					image_data_ref_opera.size(),
					CV_8UC3
				);

				// 重置事件
				//SetEvent(opear_event);

				CannyWithArrayParam param = {
					threshold1,
					threshold2,
					aperture,
					l2,
					opear_event,
					image_data_ref_opera,
					out_ref_mat,
					cv::Scalar{0.0,255.0,0.0},
				};

				auto hRefThread = CreateThread(nullptr, 0, AsyncOpera, &param, 0, nullptr);
				auto result = WaitForSingleObject(hRefThread, INFINITE);
				if (WAIT_OBJECT_0 == result)
				{
					set_data_ref_opera(out_ref_mat);
				}
				CloseHandle(hRefThread);
			}

			// 每次运行，算子数据都需要重置为原始数据的副本
			reset_data_opera();

			// 重置事件
			//SetEvent(opear_event);

			cv::Mat out_mat = cv::Mat::zeros(
				image_data_opera.size(),
				CV_8UC3
			);
			CannyWithArrayParam param = {
				threshold1,
				threshold2,
				aperture,
				l2,
				opear_event,
				image_data_opera,
				out_mat,
				cv::Scalar{ 0.0,0.0,255.0 },
			};

			auto hThread = CreateThread(nullptr, 0, AsyncOpera, &param, 0, nullptr);
			auto result = WaitForSingleObject(hThread, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				set_data_opera(out_mat);
			}
			CloseHandle(hThread);
		}

		void laplacian()
		{

		}

		void sobel()
		{

		}

		void gaussian_blur(int x, int y, double sigma_x, double sigma_y = 0, int border = cv::BORDER_DEFAULT)
		{
			set_event();
			if (use_ref_image_data && !image_data_ref.empty())
			{
				// 每次运行，算子数据都需要重置为原始数据的副本
				reset_data_ref_opera();
				cv::Mat out_ref_mat = cv::Mat::zeros(
					image_data_ref_opera.size(),
					CV_8UC3/*CV_8UC1*/
				);

				// 重置事件
				//SetEvent(opear_event);

				GaussianWithArrayParam param = {
					x,
					y,
					sigma_x,
					sigma_y,
					border,
					opear_event,
					image_data_ref_opera,
					out_ref_mat,
				};

				auto hRefThread = CreateThread(nullptr, 0, AsyncGaussian, &param, 0, nullptr);
				auto result = WaitForSingleObject(hRefThread, INFINITE);
				if (WAIT_OBJECT_0 == result)
				{
					set_data_ref_opera(out_ref_mat);
				}
				CloseHandle(hRefThread);
			}

			// 每次运行，算子数据都需要重置为原始数据的副本
			reset_data_opera();

			// 重置事件
			//SetEvent(opear_event);
			//cv::Mat out_mat = cv::Mat::zeros(
			//	image_data_opera.size(),
			//	CV_8UC3/*CV_8UC1*/
			//);
			cv::Mat out_mat;
			GaussianWithArrayParam param = {
				x,
				y,
				sigma_x,
				sigma_y,
				border,
				opear_event,
				image_data_opera,
				out_mat
			};

			auto hThread = CreateThread(nullptr, 0, AsyncGaussian, &param, 0, nullptr);
			auto result = WaitForSingleObject(hThread, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				set_data_opera(out_mat);
			}
			CloseHandle(hThread);
		}

		// 减影 -- 原图像 - 高斯模糊
		void minus_image()
		{
			auto minus_image = static_cast<cv::Mat>((image_data_raw - image_data_opera) + cv::Scalar{ 128 });
			set_data_opera(minus_image);
		}

		// 二值化
		void threshold(unsigned char threshold, unsigned char max_value = 255, int type = cv::THRESH_BINARY)
		{
			set_event();

			reset_data_threshold();

			cv::Mat out_mat = cv::Mat::zeros(
				image_data_threshold.size(),
				CV_8UC3/*CV_8UC1*/
			);
			ThresholdWithArrayParam param = {
					threshold,
					max_value,
					type,
					opear_event,
					image_data_threshold,
					out_mat,
			};

			auto hRefThread = CreateThread(nullptr, 0, AsyncThreshold, &param, 0, nullptr);
			auto result = WaitForSingleObject(hRefThread, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				//并行
#pragma omp parallel for
				for (int i = 0; i < out_mat.rows; ++i)
				{
					for (int j = 0; j < out_mat.cols; ++j)
					{
						auto& color = out_mat.at<cv::Vec3b>(i, j);
						if (color == cv::Vec3b{ 255,255,255 })
						{
							color = cv::Vec3b{ 0,0,255 };
						}
					}
				}
				set_data_threshold(out_mat);
			}
			CloseHandle(hRefThread);
		}

		// LoG算子也就是 Laplacian of Gaussian function（高斯拉普拉斯函数）。
		// 常用于数字图像的边缘提取和二值化。
		void laplacian_of_gaussian(
			int ksize,
			int x, int y, double sigma_x, double sigma_y = 0, int border = cv::BORDER_DEFAULT,
			int depth = CV_8U,
			double scale = 1.0,
			double delta = 0.0)
		{
			gaussian_blur(x, y, sigma_x, sigma_y, border);

			cv::Mat out_mat;
			LaplacianOfGaussianWithArrayParam param = {
					depth,
					static_cast<unsigned char>(ksize),
					scale,
					delta,
					/*type*/border,
					opear_event,
					image_data_opera,
					out_mat,
			};

			auto hRefThread = CreateThread(nullptr, 0, AsyncLaplacianOfGaussian, &param, 0, nullptr);
			auto result = WaitForSingleObject(hRefThread, INFINITE);
			if (WAIT_OBJECT_0 == result)
			{
				//				//并行
				//#pragma omp parallel for
				//				for (int i = 0; i < out_mat.rows; ++i)
				//				{
				//					for (int j = 0; j < out_mat.cols; ++j)
				//					{
				//						auto& color = out_mat.at<cv::Vec3b>(i, j);
				//						if (color == cv::Vec3b{ 255,255,255 })
				//						{
				//							color = cv::Vec3b{ 0,0,255 };
				//						}
				//					}
				//				}
				set_data_opera(out_mat);
			}
			CloseHandle(hRefThread);
		}

		// LoG算子也就是 Laplacian of Gaussian function（高斯拉普拉斯函数）。
		// 常用于数字图像的边缘提取和二值化。
		void laplacian_of_gaussian_ex()
		{
			auto log_mat = image_data_raw.clone();
			cv::Mat blur;
			cv::GaussianBlur(log_mat, blur, cv::Size{ 3,3 }, 0.0);
			cv::Mat lap_mat;
			cv::Laplacian(blur, lap_mat, CV_8U, 3);
			image_data_opera = lap_mat.clone();
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

		// 获取原始数据(选择的某幅待分析图像)
		cv::Mat mat_raw() const
		{
			return image_data_raw;
		}

		// 获取原始数据(选择的某幅待分析图像)的副本
		cv::Mat mat_raw_clone() const
		{
			return image_data_raw.clone();
		}

		void set_data_opera(cv::Mat const& mat)
		{
			image_data_opera = mat.clone();
		}

		void set_data_ref_opera(cv::Mat const& mat)
		{
			image_data_ref_opera = mat/*.clone()*/;
		}

		void set_data_threshold(cv::Mat const& mat)
		{
			image_data_threshold = mat/*.clone()*/;
		}

		//void set_data_log(cv::Mat const& mat)
		//{
		//	image_data_log = mat/*.clone()*/;
		//}

		// 重置数据(二值化)--从image_data_opera数据中
		void reset_data_threshold()
		{
			image_data_threshold = image_data_opera.clone();
		}

		//// 重置数据LoG算子--从原始数据中
		//void reset_data_log()
		//{
		//	image_data_log = image_data_raw.clone();
		//}

		// 重置数据--从原始数据中
		void reset_data_opera()
		{
			image_data_opera = image_data_raw.clone();
		}

		// 重置数据--从参考数据中
		void reset_data_ref_opera()
		{
			image_data_ref_opera = image_data_ref.clone();
		}

		// 原始图像的底层数据(只读)
		unsigned char* raw_data() const
		{
			return image_data_raw.data();
		}

		// 原始图像的底层数据
		unsigned char* raw_data()
		{
			return image_data_raw.data();
		}

		// 是否排除其它图像的显示
		void exclude_other_image(bool exclude = true)
		{
			exclude_other = exclude;
		}

		// 是否排除缺陷图像(训练图像)的显示
		void exclude_defect_image(bool exclude = true)
		{
			exclude_defect = exclude;
		}

		// 使用(标准)参考数据
		void set_use_ref_data(bool use_ref = true)
		{
			use_ref_image_data = use_ref;
		}

		// 使用二值化图像
		void set_use_threshold(bool use = true)
		{
			use_threshold = use;
		}

		bool is_use_threshold() const
		{
			return use_threshold;
		}

		// 当图像切换时,自动应用算子
		void set_auto_use_operator(bool use = true)
		{
			use_operator = use;
		}

		// 是否自动应用算子
		bool is_auto_use_operator() const
		{
			return use_operator;
		}

		// 当图像切换时,自动应用高斯模糊
		void set_auto_use_gaussian(bool use = true)
		{
			use_gaussian = use;
		}

		// 是否自动应用高斯模糊
		bool is_auto_use_gaussian() const
		{
			return use_gaussian;
		}

		// 当图像切换时,自动应用LOG
		void set_auto_use_laplacian_of_gaussian(bool use = true)
		{
			use_laplacian_of_gaussian = use;
		}

		// 是否自动应用LoG算子
		bool is_auto_use_laplacian_of_gaussian() const
		{
			return use_laplacian_of_gaussian;
		}

		// 仅显示原始图像
		bool show_raw_only() const
		{
			return exclude_other;
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

			CRect image_rect = { 0,0,width(),height() };
			CRect out = image_rect;
			out.OffsetRect(offset_x, offset_y);
			WTL::CMemoryDC mem_dc{ dc,out };

			WTL::CBitmap temp_bitmap;
			// 注意创建与原始dc兼容的位图
			temp_bitmap.CreateCompatibleBitmap(dc, width(), height());
			temp_dc.SelectBitmap(temp_bitmap);

			if (!exclude_other)
			{
				// 如果算子运算结果不为空，则直接绘制运算结果，跳过原始图像。
				if (!opera_image.IsNull())
				{
					WTL::CDC opera_dc;
					opera_dc.CreateCompatibleDC(dc);
					opera_dc.SaveDC();
					opera_dc.SelectBitmap(opera_image);

					mem_dc.BitBlt(offset_x, offset_y, width(), height(), opera_dc, 0, 0, SRCCOPY);
					opera_dc.RestoreDC(-1);
					temp_dc.RestoreDC(-1);

					// 直接绘制并退出
					return true;
				}
			}

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

			// 绘制缺陷图像(训练图像)
			if (!exclude_defect && !defect_image.IsNull())
			{
				WTL::CDC defect_dc;
				defect_dc.CreateCompatibleDC(temp_dc);
				defect_dc.SaveDC();
				defect_dc.SelectBitmap(defect_image);

				//temp_dc.BitBlt(0, 0, width(), height(), defect_dc, 0, 0, SRCPAINT);
				temp_dc.TransparentBlt(0, 0, width(), height(), defect_dc, 0, 0, width(), height(), RGB(0, 0, 0));

				defect_dc.RestoreDC(-1);
			}

			// 绘制算子图像
			if (!opera_image.IsNull())
			{
				WTL::CDC opera_dc;
				opera_dc.CreateCompatibleDC(temp_dc);
				opera_dc.SaveDC();
				opera_dc.SelectBitmap(opera_image);

				//temp_dc.BitBlt(0, 0, width(), height(), opera_dc, 0, 0, SRCPAINT);
				temp_dc.TransparentBlt(0, 0, width(), height(), opera_dc, 0, 0, width(), height(), RGB(0, 0, 0));

				opera_dc.RestoreDC(-1);
			}


			// 绘制参考图像的算子图像
			if (use_ref_image_data && !ref_opera_image.IsNull())
			{
				WTL::CDC ref_opera_dc;
				ref_opera_dc.CreateCompatibleDC(temp_dc);
				ref_opera_dc.SaveDC();
				ref_opera_dc.SelectBitmap(ref_opera_image);

				//temp_dc.BitBlt(0, 0, width(), height(), ref_opera_dc, 0, 0, SRCPAINT);
				temp_dc.TransparentBlt(0, 0, width(), height(), ref_opera_dc, 0, 0, width(), height(), RGB(0, 0, 0));

				ref_opera_dc.RestoreDC(-1);
			}

			// 绘制二值化图像
			if (use_threshold && !threshold_image.IsNull())
			{
				WTL::CDC threshold_dc;
				threshold_dc.CreateCompatibleDC(temp_dc);
				threshold_dc.SaveDC();
				threshold_dc.SelectBitmap(threshold_image);
				//temp_dc.BitBlt(0, 0, width(), height(), threshold_dc, 0, 0, SRCPAINT);
				temp_dc.TransparentBlt(0, 0, width(), height(), threshold_dc, 0, 0, width(), height(), RGB(0, 0, 0));
				threshold_dc.RestoreDC(-1);
			}

			mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);
			temp_dc.RestoreDC(-1);

			return true;
		}

		//
		// 分选项绘制
		// 1. 当仅绘制原始图像时
		// 2. 当有算子操作时
		// 3. 当有附加于算子上的二次操作时
		//
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

			if (!exclude_other)
			{
				bool has_other = false;
				// 绘制缺陷图像(训练图像)
				if (!exclude_defect && !defect_image.IsNull())
				{
					WTL::CDC defect_dc;
					defect_dc.CreateCompatibleDC(temp_dc);
					defect_dc.SaveDC();
					defect_dc.SelectBitmap(defect_image);

					//temp_dc.BitBlt(0, 0, width(), height(), defect_dc, 0, 0, SRCPAINT);
					temp_dc.TransparentBlt(0, 0, width(), height(), defect_dc, 0, 0, width(), height(), RGB(0, 0, 0));

					defect_dc.RestoreDC(-1);
					has_other = true;
				}

				// 绘制参考图像的算子图像
				if (use_ref_image_data && !ref_opera_image.IsNull())
				{
					WTL::CDC ref_opera_dc;
					ref_opera_dc.CreateCompatibleDC(temp_dc);
					ref_opera_dc.SaveDC();
					ref_opera_dc.SelectBitmap(ref_opera_image);

					//temp_dc.BitBlt(0, 0, width(), height(), ref_opera_dc, 0, 0, SRCPAINT);
					temp_dc.TransparentBlt(0, 0, width(), height(), ref_opera_dc, 0, 0, width(), height(), RGB(0, 0, 0));

					ref_opera_dc.RestoreDC(-1);
					has_other = true;
				}

				if (!has_other)has_other = use_threshold;

				if (has_other)
				{
					// 绘制算子图像
					if (!opera_image.IsNull())
					{
						WTL::CDC opera_dc;
						opera_dc.CreateCompatibleDC(temp_dc);
						opera_dc.SaveDC();
						opera_dc.SelectBitmap(opera_image);

						//temp_dc.BitBlt(0, 0, width(), height(), opera_dc, 0, 0, SRCPAINT);
						temp_dc.TransparentBlt(0, 0, width(), height(), opera_dc, 0, 0, width(), height(), RGB(0, 0, 0));

						opera_dc.RestoreDC(-1);
					}
					else
					{
						WTL::CDC raw_dc;
						raw_dc.CreateCompatibleDC(temp_dc);
						raw_dc.SaveDC();
						raw_dc.SelectBitmap(raw_image);
						temp_dc.BitBlt(0, 0, width(), height(), raw_dc, 0, 0, SRCCOPY);
						raw_dc.RestoreDC(-1);

						mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);

						temp_dc.RestoreDC(-1);

						// 直接绘制原始图像并退出
						return true;
					}
				}
				// 如果算子运算结果不为空，则直接绘制运算结果。
				else
				{
					if (!opera_image.IsNull())
					{
						WTL::CDC opera_dc;
						opera_dc.CreateCompatibleDC(dc);
						opera_dc.SaveDC();
						opera_dc.SelectBitmap(opera_image);

						mem_dc.BitBlt(offset_x, offset_y, width(), height(), opera_dc, 0, 0, SRCCOPY);
						opera_dc.RestoreDC(-1);
						temp_dc.RestoreDC(-1);

						// 直接绘制并退出
						return true;
					}
					else
					{
						WTL::CDC raw_dc;
						raw_dc.CreateCompatibleDC(temp_dc);
						raw_dc.SaveDC();
						raw_dc.SelectBitmap(raw_image);
						temp_dc.BitBlt(0, 0, width(), height(), raw_dc, 0, 0, SRCCOPY);
						raw_dc.RestoreDC(-1);

						mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);

						temp_dc.RestoreDC(-1);

						// 直接绘制原始图像并退出
						return true;
					}
				}
			}
			else // 显式排除了其它图像时，仅绘制原始图像
			{
				WTL::CDC raw_dc;
				raw_dc.CreateCompatibleDC(temp_dc);
				raw_dc.SaveDC();
				raw_dc.SelectBitmap(raw_image);
				temp_dc.BitBlt(0, 0, width(), height(), raw_dc, 0, 0, SRCCOPY);
				raw_dc.RestoreDC(-1);

				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);

				temp_dc.RestoreDC(-1);
				// 直接绘制原始图像并退出
				return true;
			}

			// 绘制二值化图像
			if (use_threshold && !threshold_image.IsNull())
			{
				WTL::CDC threshold_dc;
				threshold_dc.CreateCompatibleDC(temp_dc);
				threshold_dc.SaveDC();
				threshold_dc.SelectBitmap(threshold_image);
				//temp_dc.BitBlt(0, 0, width(), height(), threshold_dc, 0, 0, SRCPAINT);
				temp_dc.TransparentBlt(0, 0, width(), height(), threshold_dc, 0, 0, width(), height(), RGB(0, 0, 0));
				threshold_dc.RestoreDC(-1);
			}

			mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);
			temp_dc.RestoreDC(-1);

			return true;
		}

		bool draw_opera_only(HDC dc, int offset_x = 0, int offset_y = 0)
		{
			CRect image_rect = { 0,0,width(),height() };
			CRect out = image_rect;
			out.OffsetRect(offset_x, offset_y);
			WTL::CMemoryDC mem_dc{ dc,out };

			WTL::CDC temp_dc;
			temp_dc.CreateCompatibleDC(dc);
			temp_dc.SaveDC();

			// 绘制算子图像
			if (!opera_image.IsNull())
			{
				temp_dc.SelectBitmap(opera_image);
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
			// 缺陷数据(训练)
			image_data_defect = load_image(image_file, static_cast<int>(flag));
		}

		void from_file_internal_ref(std::string const& image_file, image_type flag)
		{
			// 参考数据
			image_data_ref = load_image(image_file, static_cast<int>(flag));
		}

		// 销毁运算图像的显示图像
		void reset_opera_image()
		{
			if (!opera_image.IsNull())
			{
				opera_image.DeleteObject();
			}
		}

		// 销毁参考图像的运算图像的显示图像
		void reset_ref_opera_image()
		{
			if (!ref_opera_image.IsNull())
			{
				ref_opera_image.DeleteObject();
			}
		}

		// 销毁检测图像的显示图像
		void reset_defect_image()
		{
			if (!defect_image.IsNull())
			{
				defect_image.DeleteObject();
			}
		}

		// 销毁原始图像的显示图像
		void reset_raw_image()
		{
			if (!raw_image.IsNull())
			{
				raw_image.DeleteObject();
			}
		}

	private:
		// 指示算子正在运算中的事件
		HANDLE opear_event = nullptr;

		void init_event(BOOL manual = TRUE)
		{
			if (opear_event)
			{
				CloseHandle(opear_event);
			}
			opear_event = CreateEvent(nullptr, manual, FALSE, nullptr);
			//CloseHandle(opear_event);
		}

		bool set_event()
		{
			if (!opear_event)return false;
			return SetEvent(opear_event) == TRUE;
		}


	private:
		constexpr static auto depth = 8;
		// BMP 位图的平面数必须为1
		constexpr static auto planes = 1;

		// 不做运算

		cv::Mat image_data_raw; // 某一幅待检测图像数据(缓存)
		cv::Mat image_data_defect; // 不做运算，仅用于显示和对比
		cv::Mat image_data_ref; // 固定的参考图像的数据(标准图像数据)(缓存)

		// 运算

		cv::Mat image_data_opera; // 运算数据(原始图像数据的副本,image_data_raw.clone())，将算子作用于此数据
		cv::Mat image_data_ref_opera; // 参考图像的运算数据(参考图像的副本，image_data_ref.clone())
		//cv::Mat image_data_gaussian; // 运算数据(原始图像数据的副本,image_data_raw.clone())，将高斯作用于此数据
		//cv::Mat image_data_minus; //减影图像数据 (raw-gaussian)
		cv::Mat image_data_threshold; // 二值化图像数据
		//cv::Mat image_data_log; // LoG算子

		ImageData raw_image; // 原始图像
		ImageData defect_image; //缺陷图像

		ImageData opera_image; // 算子图像
		ImageData ref_opera_image; // 参考图像的算子图像
		ImageData threshold_image; // 二值化图像
		ImageData l_o_g_image;// LoG算子图像

		bool exclude_other = false; // 排除其它图像的显示，仅显示原始图像
		bool exclude_defect = true; // 单独排除缺陷图像(训练图像)的显示

		bool use_operator = false; // 自动加载算子，控制图像数据切换时是否自动运算相应的算子。
		bool use_gaussian = false; // 自动加载高斯模糊，控制图像数据切换时是否自动运算相应的高斯模糊。
		bool use_laplacian_of_gaussian = false; // 自动加载LOG，控制图像数据切换时是否自动运算相应的LOG。
		bool use_ref_image_data = false; //使用参考图像数据(使用算子时)
		bool use_threshold = false; // 使用二值化图像
	};

	using bmp_image = image_any<>;
}


//////////////////////////////////////////////////////////////////////////////////////


		//// 创建算子位图 调用得不到正确的结果
		//void create_section(HDC dc)
		//{
		//	reset_opera_image();
		//	// 注意，这里使用了原始数据的克隆。
		//	auto clone_image = mat_raw_clone();

		//	unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
		//	auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
		//	freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

		//	opera_image.CreateDIBSection(dc, bitmap_info, DIB_PAL_COLORS, (void**)(&image_data_opera.data), nullptr, 0);
		//}

		//// 创建算子位图 重新生成位图并赋值底层数据
		//void create_dib(HDC dc)
		//{
		//	reset_opera_image();

		//	unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
		//	auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
		//	freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

		//	opera_image.CreateCompatibleBitmap(dc, width(), height());
		//	opera_image.SetDIBits(
		//		dc,
		//		0,
		//		height(),
		//		image_data_opera.data,
		//		bitmap_info,
		//		DIB_RGB_COLORS
		//	);
		//}