#ifndef utils_curl_hpp_included
#define utils_curl_hpp_included

#include "Globals.hpp"
#include <curl/curl.h>
#include <tuple>

namespace himitsu
{
	class curl
	{
	private:
		static size_t internal_callback(void* raw_data, size_t size, size_t memory, std::string* ptr)
		{
			size_t new_length = size * memory;
			size_t old_length = ptr->size();

			try
			{
				ptr->resize(old_length + new_length);
			}
			catch (const std::bad_alloc& ex)
			{
				std::cout << "Unable to allocate new memory for http response: " << ex.what() << std::endl;
				return 0;
			}

			std::copy((char*)raw_data, (char*)raw_data + new_length, ptr->begin() + old_length);
			return size * memory;
		}
	public:
		static std::tuple<bool, std::string> get(const std::string& url) {
			CURL* curl = curl_easy_init();
			CURLcode status_code;

			if (curl == nullptr)
				return { false, "Unable to acquire curl handle." };

			std::string output;

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, internal_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "osu!Himitsu");
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

			status_code = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			if (status_code == CURLE_OK)
				return { true, output };

			output = curl_easy_strerror(status_code);
			return { false, output };
		}
	};
}

#endif