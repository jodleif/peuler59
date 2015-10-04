#include <iostream>
#include "fileio/fileio.h"

int main()
{
	std::cout << "XOR - decrypt\n";
	auto encrypted_file = xor_decrypt::fileio::read_file("p059_cipher.txt");
	return 0;
}