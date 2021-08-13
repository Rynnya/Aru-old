#include "utils.hpp"

#include <random>

std::pair<std::string_view, std::string_view> aru::utils::split_pair(const char* ptr, size_t size, char delim)
{
	int64_t pos = (std::find(ptr, ptr + size, delim) - ptr);
	return { std::string_view(ptr, pos), std::string_view(ptr + pos + 1, size - pos - 1) };
}

const std::unordered_map<std::string_view, std::string_view> aru::utils::parse_cookie_string(const oatpp::String& _cookie)
{
	std::unordered_map<std::string_view, std::string_view> map = {};
	if (!_cookie || _cookie->getSize() == 0)
	{
		return map;
	}

	char* cookie = const_cast<char*>(_cookie->c_str());
	size_t size = _cookie->getSize() - std::strlen(std::remove(cookie, cookie + _cookie->getSize(), ' '));

	for (auto ptr = cookie;; ++ptr)
	{
		auto prev = ptr;
		ptr = std::find(ptr, cookie + size, ';');
		map.emplace(utils::split_pair(&*prev, (ptr - prev), '='));
		if (ptr == cookie + size)
		{
			break;
		}
	}

	return map;
}

const std::string aru::utils::get_ip_address(const std::shared_ptr<oatpp::web::protocol::http::incoming::Request>& request)
{
	oatpp::String address = request->getHeader("CF-Connecting-IP");
	if (address)
	{
		return address->c_str();
	}

	address = request->getHeader("X-Real-IP");
	if (address)
	{
		return address->c_str();
	}

	return "0.0.0.0";
}

std::string aru::utils::rtrim(std::string& s, const char* t)
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

std::string aru::utils::ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

std::string aru::utils::trim(std::string& s, const char* t)
{
	return ltrim(rtrim(s, t), t);
}

uint32_t aru::utils::gen_random_number()
{
	static std::random_device rd;
	static std::mt19937 engine(rd());

	return engine();
}

std::string aru::utils::gen_random_string(const int32_t& len)
{

	std::string tmp_s;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	tmp_s.reserve(len);
	for (int32_t i = 0; i < len; ++i)
		tmp_s.push_back(alphanum[gen_random_number() % (sizeof(alphanum) - 1)]);

	return tmp_s;
}

void aru::utils::str_tolower(std::string& str)
{
	for (char& c : str)
		c = std::tolower(c);
}

std::string aru::utils::str_tolower(const std::string& str)
{
	std::string res = str;
	str_tolower(res);
	return res;
}

bool aru::utils::int_to_bool(const int32_t& i, bool more)
{
	if (i == 1)
	{
		return true;
	}

	if (more && i >= 1)
	{
		return true;
	}

	return false;
}