#pragma once
#include <string>
#include <vector>
#include <memory>

namespace xor_decrypt
{
	struct key_stats
	{
		std::string key;
		float percent_reg_chars;
	};

	struct code_pair 
	{
		char key;
		float percent_reg_chars;
	};

	struct split_encrypt
	{
		std::vector<char> split1, split2, split3;
	};

	std::vector<std::unique_ptr<std::vector<char>>> find_keys(const split_encrypt* split);
	void find_partial_key(const std::vector<char>& encrypted_text, std::vector<char>* possible_keys);

	std::vector<char> everynth(const std::vector<char>& source, std::size_t elem_start, std::size_t skip_len);

	//std::vector<std::string> generate_possible_keys(std::vector<char>* k1, std::vector<char>* k2, std::vector<char>* k3);

	key_stats perform_statistical_analysis(const xor_decrypt::split_encrypt* split, const std::vector<std::unique_ptr<std::vector<char>>> & keys);
	void analyze_keys(const std::vector<char>& split, std::vector<code_pair>& to_store_stat, std::vector<char>* keys);
	void analyze_key(const std::vector<char>& split, float& stats, char key);
	std::vector<char> decrypt(const split_encrypt* split, std::string key);
	void inplace_decrypt(const std::vector<char>& to_dec, std::vector<char>& decrypted, char key_ch);
	std::vector<char> merge_vectors(const std::vector<char>& str1, const std::vector<char>& str2, const std::vector<char>& str3);
}