#include "curl.hpp"

size_t aru::curl::internal_callback(void* raw_data, size_t size, size_t memory, std::string* ptr)
{
	size_t new_length = size * memory;
	size_t old_length = ptr->size();

	ptr->resize(old_length + new_length);
	std::copy((char*)raw_data, (char*)raw_data + new_length, ptr->begin() + old_length);

	return size * memory;
}

std::tuple<bool, std::string> aru::curl::get(const std::string& url)
{
	CURL* curl = curl_easy_init();
	CURLcode status_code;

	if (curl == nullptr)
	{
		return { false, "Unable to acquire curl handle." };
	}

	std::string output;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, internal_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "osu! Aru");
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	status_code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (status_code == CURLE_OK)
	{
		return { true, output };
	}

	output = curl_easy_strerror(status_code);
	return { false, output };
}