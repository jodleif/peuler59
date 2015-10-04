#include "decrypt.h"
#include <thread>
#include <algorithm>
#include <iostream>

std::vector<std::unique_ptr<std::vector<char>>> xor_decrypt::find_keys(const xor_decrypt::split_encrypt* split)
{
	auto possible_keys1 = std::make_unique<std::vector<char>>();
	auto possible_keys2 = std::make_unique<std::vector<char>>();
	auto possible_keys3 = std::make_unique<std::vector<char>>();

	auto t1 = std::thread(find_partial_key, std::ref(split->split1), (possible_keys1.get()));
	auto t2 = std::thread(find_partial_key, std::ref(split->split2), (possible_keys2.get()));
	auto t3 = std::thread(find_partial_key, std::ref(split->split3), (possible_keys3.get()));
	std::vector<std::unique_ptr<std::vector<char>>> res;
	t1.join();
	t2.join();
	t3.join();

	res.push_back(std::move(possible_keys1));
	res.push_back(std::move(possible_keys2));
	res.push_back(std::move(possible_keys3));
	

	return res;
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

/* 
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
*/

xor_decrypt::key_stats xor_decrypt::perform_statistical_analysis(const xor_decrypt::split_encrypt* split, const std::vector<std::unique_ptr<std::vector<char>>> & keys)
{
	std::vector<xor_decrypt::code_pair> key1;
	std::vector<xor_decrypt::code_pair> key2;
	std::vector<xor_decrypt::code_pair> key3;
	
	auto t1 = std::thread(analyze_keys, std::ref(split->split1), std::ref(key1), (keys[0]).get());
	auto t2 = std::thread(analyze_keys, std::ref(split->split2), std::ref(key2), (keys[1]).get());
	auto t3 = std::thread(analyze_keys, std::ref(split->split3), std::ref(key3), (keys[2]).get());
	t1.join(); t2.join(); t3.join();
	if(key1.size()==0 || key2.size()==0||key3.size()==0) {
		std::cerr << "[xor_decrypt::perform_statistical_analysis]ERROR: Empty key\n";

		return{ "",0.0 };
	}
	std::string top_key;
	top_key.push_back(key1[0].key);
	top_key += key2[0].key;
	top_key += key3[0].key;

	float avg_match = key1[0].percent_reg_chars + key2[0].percent_reg_chars + key3[0].percent_reg_chars;
	avg_match /= 3;


	return {top_key,avg_match};
}

void xor_decrypt::analyze_keys(const std::vector<char>& split, std::vector<xor_decrypt::code_pair>& to_store_stat, std::vector<char>* keys)
{
	for(const auto& key : (*keys)) {
		float to_store_res{ 0 };
		analyze_key(std::ref(split), std::ref(to_store_res), key);
		to_store_stat.push_back(xor_decrypt::code_pair{ key , to_store_res });
	}

	std::sort(to_store_stat.begin(), to_store_stat.end(), [](const xor_decrypt::code_pair& elem1, const xor_decrypt::code_pair& elem2) {
		return elem1.percent_reg_chars > elem2.percent_reg_chars;
	});
}

void xor_decrypt::analyze_key(const std::vector<char>& split, float& stats, char key)
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