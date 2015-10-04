#pragma once
#include <vector>
#include <string>
namespace xor_decrypt {
	namespace fileio {
		std::vector<char> read_csv(const std::string& filename);
		std::vector<char> strip_separator(const std::vector<char>& csv_input); // AND convert to char..
	}
}