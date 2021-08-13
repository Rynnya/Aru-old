#include "hash.hpp"

#include <openssl/md5.h>
#include <openssl/evp.h>

std::string aru::hash::create_sha512(const std::string& input, const std::string& salt)
{
	unsigned char result[64];
	char output[129];

	PKCS5_PBKDF2_HMAC(input.c_str(), input.length(), reinterpret_cast<const unsigned char*>(salt.c_str()), salt.size(), 4096, EVP_sha512(), 64, result);

	for (size_t i = 0; i < 64; i++) {
		std::snprintf(&output[i * 2], sizeof(output), "%02x", (unsigned int)result[i]);
	}

	return std::string(output);
}

std::string aru::hash::create_md5(const std::string& input)
{
	unsigned char result[16];
	char output[33];

	MD5((unsigned char*)input.c_str(), input.length(), result);

	for (size_t i = 0; i < 16; i++) {
		std::snprintf(&output[i * 2], sizeof(output), "%02x", (unsigned int)result[i]);
	}

	return std::string(output);
}