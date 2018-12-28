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

// ��װͼ�����ݵ���

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

		// ����
	public:
		// ԭʼͼ���ļ�
		void raw_file(std::wstring const& image_file, image_type flag = image_type::IMREAD_COLOR)
		{
			auto file = convert_from(image_file);
			from_file_internal(file, flag);
		}

		// ȱ��ͼ���ļ�
		void defect_file(std::wstring const& defectfile, image_type flag = image_type::IMREAD_COLOR)
		{
			auto file = convert_from(defectfile);
			from_file_internal_defect(file, flag);
		}

		// �ο�ͼ���ļ�(�����)
		void reference_file(std::wstring const& ref_file, image_type flag = image_type::IMREAD_COLOR)
		{
			auto file = convert_from(ref_file);
			from_file_internal_ref(file, flag);
		}

		// ��������ͼ�����ʾͼ��
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

		// ����ԭʼͼ��λͼ
		void create_raw_bitmap(HDC dc)
		{
			reset_raw_image();

			// ע�⣬����ʹ����ԭʼ���ݵĿ�¡��
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

		// ����ȱ��ͼ��λͼ
		void create_defect_bitmap(HDC dc)
		{
			reset_defect_image();
			if (image_data_defect.empty())return;

			unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
			auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
			freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

			defect_image.CreateDIBitmap(dc, &bitmap_info->bmiHeader, CBM_INIT, image_data_defect.data, bitmap_info, DIB_RGB_COLORS);
		}

		// �����ο�ͼ������λͼ
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

		// ��������λͼ ֱ���滻��ֵ�ײ�����
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

		// ������ֵ��λͼ ֱ���滻��ֵ�ײ�����
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

		// ����
	public:
		void canny(double threshold1, double threshold2, int aperture, bool l2 = false)
		{
			set_event();
			if (use_ref_image_data && !image_data_ref.empty())
			{
				// ÿ�����У��������ݶ���Ҫ����Ϊԭʼ���ݵĸ���
				reset_data_ref_opera();
				cv::Mat out_ref_mat = cv::Mat::zeros(
					image_data_ref_opera.size(),
					CV_8UC3
				);

				// �����¼�
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

			// ÿ�����У��������ݶ���Ҫ����Ϊԭʼ���ݵĸ���
			reset_data_opera();

			// �����¼�
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
				// ÿ�����У��������ݶ���Ҫ����Ϊԭʼ���ݵĸ���
				reset_data_ref_opera();
				cv::Mat out_ref_mat = cv::Mat::zeros(
					image_data_ref_opera.size(),
					CV_8UC3/*CV_8UC1*/
				);

				// �����¼�
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

			// ÿ�����У��������ݶ���Ҫ����Ϊԭʼ���ݵĸ���
			reset_data_opera();

			// �����¼�
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

		// ��Ӱ -- ԭͼ�� - ��˹ģ��
		void minus_image()
		{
			auto minus_image = static_cast<cv::Mat>((image_data_raw - image_data_opera) + cv::Scalar{ 128 });
			set_data_opera(minus_image);
		}

		// ��ֵ��
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
				//����
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

		// LoG����Ҳ���� Laplacian of Gaussian function����˹������˹��������
		// ����������ͼ��ı�Ե��ȡ�Ͷ�ֵ����
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
				//				//����
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

		// LoG����Ҳ���� Laplacian of Gaussian function����˹������˹��������
		// ����������ͼ��ı�Ե��ȡ�Ͷ�ֵ����
		void laplacian_of_gaussian_ex()
		{
			auto log_mat = image_data_raw.clone();
			cv::Mat blur;
			cv::GaussianBlur(log_mat, blur, cv::Size{ 3,3 }, 0.0);
			cv::Mat lap_mat;
			cv::Laplacian(blur, lap_mat, CV_8U, 3);
			image_data_opera = lap_mat.clone();
		}

		// ����
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

		// ��ȡԭʼ����(ѡ���ĳ��������ͼ��)
		cv::Mat mat_raw() const
		{
			return image_data_raw;
		}

		// ��ȡԭʼ����(ѡ���ĳ��������ͼ��)�ĸ���
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

		// ��������(��ֵ��)--��image_data_opera������
		void reset_data_threshold()
		{
			image_data_threshold = image_data_opera.clone();
		}

		//// ��������LoG����--��ԭʼ������
		//void reset_data_log()
		//{
		//	image_data_log = image_data_raw.clone();
		//}

		// ��������--��ԭʼ������
		void reset_data_opera()
		{
			image_data_opera = image_data_raw.clone();
		}

		// ��������--�Ӳο�������
		void reset_data_ref_opera()
		{
			image_data_ref_opera = image_data_ref.clone();
		}

		// ԭʼͼ��ĵײ�����(ֻ��)
		unsigned char* raw_data() const
		{
			return image_data_raw.data();
		}

		// ԭʼͼ��ĵײ�����
		unsigned char* raw_data()
		{
			return image_data_raw.data();
		}

		// �Ƿ��ų�����ͼ�����ʾ
		void exclude_other_image(bool exclude = true)
		{
			exclude_other = exclude;
		}

		// �Ƿ��ų�ȱ��ͼ��(ѵ��ͼ��)����ʾ
		void exclude_defect_image(bool exclude = true)
		{
			exclude_defect = exclude;
		}

		// ʹ��(��׼)�ο�����
		void set_use_ref_data(bool use_ref = true)
		{
			use_ref_image_data = use_ref;
		}

		// ʹ�ö�ֵ��ͼ��
		void set_use_threshold(bool use = true)
		{
			use_threshold = use;
		}

		bool is_use_threshold() const
		{
			return use_threshold;
		}

		// ��ͼ���л�ʱ,�Զ�Ӧ������
		void set_auto_use_operator(bool use = true)
		{
			use_operator = use;
		}

		// �Ƿ��Զ�Ӧ������
		bool is_auto_use_operator() const
		{
			return use_operator;
		}

		// ��ͼ���л�ʱ,�Զ�Ӧ�ø�˹ģ��
		void set_auto_use_gaussian(bool use = true)
		{
			use_gaussian = use;
		}

		// �Ƿ��Զ�Ӧ�ø�˹ģ��
		bool is_auto_use_gaussian() const
		{
			return use_gaussian;
		}

		// ��ͼ���л�ʱ,�Զ�Ӧ��LOG
		void set_auto_use_laplacian_of_gaussian(bool use = true)
		{
			use_laplacian_of_gaussian = use;
		}

		// �Ƿ��Զ�Ӧ��LoG����
		bool is_auto_use_laplacian_of_gaussian() const
		{
			return use_laplacian_of_gaussian;
		}

		// ����ʾԭʼͼ��
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

			// ˫����
			WTL::CDCHandle temp_dc;
			temp_dc.CreateCompatibleDC(dc);
			temp_dc.SaveDC();

			CRect image_rect = { 0,0,width(),height() };
			CRect out = image_rect;
			out.OffsetRect(offset_x, offset_y);
			WTL::CMemoryDC mem_dc{ dc,out };

			WTL::CBitmap temp_bitmap;
			// ע�ⴴ����ԭʼdc���ݵ�λͼ
			temp_bitmap.CreateCompatibleBitmap(dc, width(), height());
			temp_dc.SelectBitmap(temp_bitmap);

			if (!exclude_other)
			{
				// ���������������Ϊ�գ���ֱ�ӻ���������������ԭʼͼ��
				if (!opera_image.IsNull())
				{
					WTL::CDC opera_dc;
					opera_dc.CreateCompatibleDC(dc);
					opera_dc.SaveDC();
					opera_dc.SelectBitmap(opera_image);

					mem_dc.BitBlt(offset_x, offset_y, width(), height(), opera_dc, 0, 0, SRCCOPY);
					opera_dc.RestoreDC(-1);
					temp_dc.RestoreDC(-1);

					// ֱ�ӻ��Ʋ��˳�
					return true;
				}
			}

			// ����ԭʼͼ��
			if (!raw_image.IsNull())
			{
				WTL::CDC raw_dc;
				raw_dc.CreateCompatibleDC(temp_dc);
				raw_dc.SaveDC();
				raw_dc.SelectBitmap(raw_image);
				temp_dc.BitBlt(0, 0, width(), height(), raw_dc, 0, 0, SRCCOPY);
				raw_dc.RestoreDC(-1);
			}

			// �ų�����ͼ��ʱ��ֱ�ӷ���
			if (exclude_other)
			{
				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);
				temp_dc.RestoreDC(-1);
				return true;
			}

			// ����ȱ��ͼ��(ѵ��ͼ��)
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

			// ��������ͼ��
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


			// ���Ʋο�ͼ�������ͼ��
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

			// ���ƶ�ֵ��ͼ��
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
		// ��ѡ�����
		// 1. ��������ԭʼͼ��ʱ
		// 2. �������Ӳ���ʱ
		// 3. ���и����������ϵĶ��β���ʱ
		//
		bool draw_ex(HDC dc, int offset_x = 0, int offset_y = 0)
		{
			if (raw_image.IsNull())
			{
				return false;
			}

			// ˫����
			WTL::CDCHandle temp_dc;
			temp_dc.CreateCompatibleDC(dc);
			temp_dc.SaveDC();

			CRect image_rect = { 0,0,width(),height() };
			CRect out = image_rect;
			out.OffsetRect(offset_x, offset_y);
			WTL::CMemoryDC mem_dc{ dc,out };

			WTL::CBitmap temp_bitmap;
			// ע�ⴴ����ԭʼdc���ݵ�λͼ
			temp_bitmap.CreateCompatibleBitmap(dc, width(), height());
			temp_dc.SelectBitmap(temp_bitmap);

			if (!exclude_other)
			{
				bool has_other = false;
				// ����ȱ��ͼ��(ѵ��ͼ��)
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

				// ���Ʋο�ͼ�������ͼ��
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
					// ��������ͼ��
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

						// ֱ�ӻ���ԭʼͼ���˳�
						return true;
					}
				}
				// ���������������Ϊ�գ���ֱ�ӻ�����������
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

						// ֱ�ӻ��Ʋ��˳�
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

						// ֱ�ӻ���ԭʼͼ���˳�
						return true;
					}
				}
			}
			else // ��ʽ�ų�������ͼ��ʱ��������ԭʼͼ��
			{
				WTL::CDC raw_dc;
				raw_dc.CreateCompatibleDC(temp_dc);
				raw_dc.SaveDC();
				raw_dc.SelectBitmap(raw_image);
				temp_dc.BitBlt(0, 0, width(), height(), raw_dc, 0, 0, SRCCOPY);
				raw_dc.RestoreDC(-1);

				mem_dc.BitBlt(offset_x, offset_y, width(), height(), temp_dc, 0, 0, SRCCOPY);

				temp_dc.RestoreDC(-1);
				// ֱ�ӻ���ԭʼͼ���˳�
				return true;
			}

			// ���ƶ�ֵ��ͼ��
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

			// ��������ͼ��
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
			// ԭʼ����
			image_data_raw = load_image(image_file, static_cast<int>(flag));
			// �������ӵĸ���
			//image_data_opera = image_data_raw.clone();
		}

		void from_file_internal_defect(std::string const& image_file, image_type flag)
		{
			// ȱ������(ѵ��)
			image_data_defect = load_image(image_file, static_cast<int>(flag));
		}

		void from_file_internal_ref(std::string const& image_file, image_type flag)
		{
			// �ο�����
			image_data_ref = load_image(image_file, static_cast<int>(flag));
		}

		// ��������ͼ�����ʾͼ��
		void reset_opera_image()
		{
			if (!opera_image.IsNull())
			{
				opera_image.DeleteObject();
			}
		}

		// ���ٲο�ͼ�������ͼ�����ʾͼ��
		void reset_ref_opera_image()
		{
			if (!ref_opera_image.IsNull())
			{
				ref_opera_image.DeleteObject();
			}
		}

		// ���ټ��ͼ�����ʾͼ��
		void reset_defect_image()
		{
			if (!defect_image.IsNull())
			{
				defect_image.DeleteObject();
			}
		}

		// ����ԭʼͼ�����ʾͼ��
		void reset_raw_image()
		{
			if (!raw_image.IsNull())
			{
				raw_image.DeleteObject();
			}
		}

	private:
		// ָʾ�������������е��¼�
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
		// BMP λͼ��ƽ��������Ϊ1
		constexpr static auto planes = 1;

		// ��������

		cv::Mat image_data_raw; // ĳһ�������ͼ������(����)
		cv::Mat image_data_defect; // �������㣬��������ʾ�ͶԱ�
		cv::Mat image_data_ref; // �̶��Ĳο�ͼ�������(��׼ͼ������)(����)

		// ����

		cv::Mat image_data_opera; // ��������(ԭʼͼ�����ݵĸ���,image_data_raw.clone())�������������ڴ�����
		cv::Mat image_data_ref_opera; // �ο�ͼ�����������(�ο�ͼ��ĸ�����image_data_ref.clone())
		//cv::Mat image_data_gaussian; // ��������(ԭʼͼ�����ݵĸ���,image_data_raw.clone())������˹�����ڴ�����
		//cv::Mat image_data_minus; //��Ӱͼ������ (raw-gaussian)
		cv::Mat image_data_threshold; // ��ֵ��ͼ������
		//cv::Mat image_data_log; // LoG����

		ImageData raw_image; // ԭʼͼ��
		ImageData defect_image; //ȱ��ͼ��

		ImageData opera_image; // ����ͼ��
		ImageData ref_opera_image; // �ο�ͼ�������ͼ��
		ImageData threshold_image; // ��ֵ��ͼ��
		ImageData l_o_g_image;// LoG����ͼ��

		bool exclude_other = false; // �ų�����ͼ�����ʾ������ʾԭʼͼ��
		bool exclude_defect = true; // �����ų�ȱ��ͼ��(ѵ��ͼ��)����ʾ

		bool use_operator = false; // �Զ��������ӣ�����ͼ�������л�ʱ�Ƿ��Զ�������Ӧ�����ӡ�
		bool use_gaussian = false; // �Զ����ظ�˹ģ��������ͼ�������л�ʱ�Ƿ��Զ�������Ӧ�ĸ�˹ģ����
		bool use_laplacian_of_gaussian = false; // �Զ�����LOG������ͼ�������л�ʱ�Ƿ��Զ�������Ӧ��LOG��
		bool use_ref_image_data = false; //ʹ�òο�ͼ������(ʹ������ʱ)
		bool use_threshold = false; // ʹ�ö�ֵ��ͼ��
	};

	using bmp_image = image_any<>;
}


//////////////////////////////////////////////////////////////////////////////////////


		//// ��������λͼ ���õò�����ȷ�Ľ��
		//void create_section(HDC dc)
		//{
		//	reset_opera_image();
		//	// ע�⣬����ʹ����ԭʼ���ݵĿ�¡��
		//	auto clone_image = mat_raw_clone();

		//	unsigned char buffer[sizeof(BITMAPINFO) + 0xFF * sizeof(RGBQUAD)];
		//	auto bitmap_info = reinterpret_cast<BITMAPINFO*>(buffer);
		//	freeze::fill_bitmap_info(bitmap_info, width(), height(), bpp(), image_any<ImageData>::planes);

		//	opera_image.CreateDIBSection(dc, bitmap_info, DIB_PAL_COLORS, (void**)(&image_data_opera.data), nullptr, 0);
		//}

		//// ��������λͼ ��������λͼ����ֵ�ײ�����
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