#pragma once

namespace freeze
{
	inline namespace detail {
		void fill_bitmap_info_bpp_8_internal(BITMAPINFO*& bmi, BITMAPINFOHEADER*& bmih_ptr)
		{
			auto palette = bmi->bmiColors;
			for (auto i = 0; i < std::numeric_limits<unsigned char>::max() + 1; ++i)
			{
				palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = static_cast<BYTE>(i);
				palette[i].rgbReserved = 0;
			}
		}
	}

	cv::Mat load_image(std::string const& file, int flags = cv::IMREAD_COLOR)
	{
		auto mat = cv::imread(file, flags);
		return mat;
	}

	cv::Mat flip(cv::Mat const& mat)
	{
		cv::Mat fliped;
		cv::flip(mat, fliped, 0);
		return fliped;
	}

	// use: 
	//    int   bpp=channels*8,origin=1
	//    uchar buffer[sizeof(BITMAPINFO) + 0xff * sizeof(RGBQUAD)];
	//    auto  bmi = static_cast<BITMAPINFO*>(buffer);
	void fill_bitmap_info(BITMAPINFO* bmi, int width, int height, int bpp, int origin = 1)
	{
		CV_Assert(bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

		auto bmih_ptr = &(bmi->bmiHeader);
		memset(bmih_ptr, 0, sizeof(*bmih_ptr));

		bmih_ptr->biSize = sizeof BITMAPINFOHEADER;
		bmih_ptr->biWidth = width;
		// 正值表示自下而上的DIB,负值表示自上而下的DIB
		bmih_ptr->biHeight = -height;
		bmih_ptr->biPlanes = origin;
		// must 0,1,4,8,16,24,32
		bmih_ptr->biBitCount = static_cast<unsigned short>(bpp);
		bmih_ptr->biCompression = BI_RGB;

		if (8 == bpp)
		{
			fill_bitmap_info_bpp_8_internal(bmi, bmih_ptr);
		}
	}

	bool get_bitmap_data(HDC hdc, SIZE* size, int* channels, void** data)
	{
		BITMAP bmp;
		auto hbmp = GetCurrentObject(hdc, OBJ_BITMAP);
		if (size)
		{
			size->cx = size->cy = 0;
		}
		if (data)
		{
			*data = nullptr;
		}

		if (hbmp == nullptr)
		{
			return false;
		}

		if (0 == GetObject(hbmp, sizeof(bmp), &bmp))
		{
			return false;
		}

		if (size)
		{
			size->cx = std::abs(bmp.bmWidth);
			size->cy = std::abs(bmp.bmHeight);
		}

		if (channels)
		{
			*channels = static_cast<int>(bmp.bmBitsPixel / 8);
		}

		if (data)
		{
			*data = bmp.bmBits;
		}

		return true;
	}
}
