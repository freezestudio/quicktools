#pragma once

namespace freeze {
	std::string convert_from(std::wstring const& wstr)
	{
		auto len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		auto pstr = new char[len] {};
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, pstr, len, nullptr, nullptr);
		return std::string{ pstr,static_cast<size_t>(len) };
	}
}
