#include "decrypt.h"
#include <algorithm>
#include <thread>
#include <iostream>
std::vector<std::string> xor_decrypt::find_keys(const xor_decrypt::split_encrypt* split)
{
	auto possible_keys1 = std::make_unique<std::vector<char>>();
	auto possible_keys2 = std::make_unique<std::vector<char>>();
	auto possible_keys3 = std::make_unique<std::vector<char>>();

	auto t1 = std::thread(find_partial_key, std::ref(split->split1), (possible_keys1.get()));
	auto t2 = std::thread(find_partial_key, std::ref(split->split2), (possible_keys2.get()));
	auto t3 = std::thread(find_partial_key, std::ref(split->split3), (possible_keys3.get()));
	t1.join();
	t2.join();
	t3.join();
	return generate_possible_keys(possible_keys1.get(), possible_keys2.get(), possible_keys3.get());
}


void xor_decrypt::find_partial_key(const std::vector<char>& encrypted_text, std::vector<char>* possible_keys)
{
	for (char key = 'a';key <= 'z';++key) {
		bool found_possible_key{ true };
		for (const auto& elem : encrypted_text) {
			char decoded_char = elem ^ key;
			if ((decoded_char>=32 && decoded_char<=126)) {
			} else {
				found_possible_key = false;
				break;
			}
		}
		if(found_possible_key) {
			possible_keys->push_back(key);
		}
	}
}


std::vector<char> xor_decrypt::everynth(const std::vector<char>& source, std::size_t elem_start, std::size_t skip_len)
{
	std::vector<char> res;
	res.reserve(source.size() / skip_len);
	for (std::size_t i{ elem_start };i < source.size();i+=skip_len) {
		res.push_back(source[i]);
	}
	return res;
}

std::vector<std::string> xor_decrypt::generate_possible_keys(std::vector<char>* k1, std::vector<char>* k2, std::vector<char>* k3)
{
	std::vector<std::string> possible_keys;

	std::string possible_key;
	possible_key.reserve(3);
	for(const auto& elem : (*k1)) {
		for(const auto& elem2: (*k2)) {
			for(const auto& elem3: (*k3)) {
				possible_key.append({ elem, elem2, elem3 });
				possible_keys.push_back(possible_key);
				possible_key.clear();
			}
		}
	}
	return possible_keys;
}

std::vector<xor_decrypt::key_stats> xor_decrypt::perform_statistical_analysis(const xor_decrypt::split_encrypt* split, const std::vector<std::string> keys)
{
	std::vector<xor_decrypt::key_stats> results;
	for(const auto& key : keys) {
		float f1, f2, f3;
		auto t1 = std::thread(analyze_key, std::ref(split->split1), std::ref(f1), key[0]);
		auto t2 = std::thread(analyze_key, std::ref(split->split2), std::ref(f2), key[1]);
		auto t3 = std::thread(analyze_key, std::ref(split->split3), std::ref(f3), key[2]);
		t1.join();
		t2.join();
		t3.join();
		float avg = (f1 + f2 + f3) / 3;
		results.push_back({ key,avg });
	}
	std::sort(results.begin(),results.end(), [](const xor_decrypt::key_stats& elem1, const xor_decrypt::key_stats& elem2)
	{
		return elem1.percent_reg_chars > elem2.percent_reg_chars;
	});
	return results;
}

void xor_decrypt::analyze_key(const std::vector<char> split, float& stats, char key)
{
	float counter{ 0.0 };
	for(const auto& encelem : split) {
		auto elem = encelem ^ key;
		if((elem>='a'&&elem<='z') || elem==',' || elem=='.' || elem==' ') {
			counter++;
		}
	}
	float percentage_regular_char = counter / split.size();
	stats = percentage_regular_char;
}

std::string xor_decrypt::decrypt(const xor_decrypt::split_encrypt* split, std::string key)
{
	std::string part1, part2, part3;
	auto t1 = std::thread(inplace_decrypt,std::ref(split->split1), std::ref(part1), key[0]);
	auto t2 = std::thread(inplace_decrypt, std::ref(split->split2), std::ref(part2), key[1]);
	auto t3 = std::thread(inplace_decrypt, std::ref(split->split3), std::ref(part3), key[2]);
	t1.join();t2.join();t3.join();
	return merge_strings(part1, part2, part3);

}

void xor_decrypt::inplace_decrypt(const std::vector<char>& to_dec, std::string& decrypted, char key_ch)
{
	for(auto& elem : to_dec) {
		decrypted+=(elem^key_ch);
	}
}

std::string xor_decrypt::merge_strings(const std::string& str1, const std::string& str2, const std::string& str3)
{
	std::string result;
	result.reserve(str1.size() + str2.size() + str3.size());
	auto min_length = std::min(str1.size(), str2.size());
	min_length = std::min(min_length, str3.size());

	for (std::size_t i{ 0 };i < min_length*3;i+=3) {
		result.push_back(str1.at(i / 3));
		result.push_back(str2.at(i / 3));
		result.push_back(str3.at(i / 3));
	}
	if(str1.size()>str3.size()) {
		result.push_back(str1.at(str3.size()));
	}
	if(str2.size()>str3.size()) {
		result.push_back(str2.at(str3.size()));
	}
	return result;
	
}