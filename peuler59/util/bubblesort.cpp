#include "bubblesort.h"
#include "../decrypt/decrypt.h"

void xor_decrypt::algo::bubblesort(std::vector<code_pair>& vec)
{	
	auto n = vec.size();
	std::size_t newn{ 0 };
	while(n!=0) {
		newn = 0;
		for (std::size_t i{ 1 };i < n;++i) {
			if(vec[i-1].percent_reg_chars < vec[i].percent_reg_chars) {
				std::swap(vec[i - 1], vec[i]);
				newn = i;
			}
		}
		n = newn;
	}
}