#pragma once
#include <string>
#include <stdint.h>

namespace utility {
	std::string sha256(const void* data, uint32_t length);
	std::string sha256(const std::string& data);
	std::string sha256file(std::FILE* file);
	bool sha256file(const char* filename, std::string& sha256);
}
