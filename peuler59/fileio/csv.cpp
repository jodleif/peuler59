#include "csv.h"
#include "fileio.h"
#include <iostream>

std::vector<char> xor_decrypt::fileio::read_csv(const std::string& filename)
{
	auto raw_text = read_file(filename);
	if(raw_text.size() == 0) {
		std::cerr << "[fileio::read_csv]ERROR: File empty\n";
		std::terminate();
	}
	return strip_separator(raw_text);
}
// Strip separator and convert to char
std::vector<char> xor_decrypt::fileio::strip_separator(const std::vector<char>& csv_input)
{
	std::vector<char> result;
	result.reserve(csv_input.size() / 3); // Approximate space needed
	int buff{ 0 };
	for(const auto& elem : csv_input) {
		if(elem != ',') {
			buff *= 10; // If a previous number- move it forward next spot i.e. 3 -> 30
			buff += (elem - '0');
		} else {
			if (buff < 256) {
				result.push_back(static_cast<char>(buff));
				buff = 0;
			} else {
				std::cerr << "[fileio::strip_separator]ERROR: conversion failed! char to big\n";
			}
		}
	}
	if(buff!=0) {
		result.push_back(static_cast<char>(buff));
	}
	result.shrink_to_fit();
	return result;
}

std::vector<char> xor_decrypt::fileio::strip_separator(const std::string& csv_input)
{
	std::vector<char> result;
	result.reserve(csv_input.size() / 3); // Approximate space needed
	int buff{ 0 };
	for (const auto& elem : csv_input) {
		if (elem != ',' && elem != ' ') {
			buff *= 10; // If a previous number- move it forward next spot i.e. 3 -> 30
			buff += (elem - '0');
		} else if(elem==',') {
			if (buff < 256) {
				result.push_back(static_cast<char>(buff));
				buff = 0;
			} else {
				std::cerr << "[fileio::strip_separator]ERROR: conversion failed! char to big\n";
			}
		}
	}
	if (buff != 0) {
		result.push_back(static_cast<char>(buff));
	}
	result.shrink_to_fit();
	return result;
}