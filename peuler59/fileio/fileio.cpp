#include "fileio.h"
#include <fstream>
#include <iostream>


std::vector<char> xor_decrypt::fileio::read_file(const std::string filename)
{
	std::ifstream file_in(filename.c_str());
	std::string string_buffer;
	std::vector<char> result;

	if(file_in.is_open()) {
		while(std::getline(file_in, string_buffer)) {
			result = string_into_vector(result, string_buffer);
			string_buffer.clear(); // empty string after read
		}
	} else {
		std::cerr << "[xor_decrypt::fileio::read_file] Could not open file! " << filename << std::endl;
	}
	return result;
}

std::vector<char> xor_decrypt::fileio::string_into_vector(std::vector<char> vec, const std::string& string)
{
	for(const auto& elem : string) {
		vec.push_back(elem);
	}
	return vec;
}