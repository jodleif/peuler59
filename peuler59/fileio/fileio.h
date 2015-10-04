#pragma once
#include <vector>
#include <string>

namespace xor_decrypt
{
	namespace fileio
	{
		std::vector<char> read_file(const std::string filename);
		std::vector<char> string_into_vector(std::vector<char> vec, const std::string& string);
	}
}