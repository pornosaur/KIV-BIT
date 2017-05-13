#include <iostream>
#include <fstream>
#include <ostream>

#include "RSA.h"
#include "HASH.h"

int main(int argc, char** args) 
{
	std::cout << "RSA and MD5 - Patrik Patera - BIT" << std::endl;
	
	if (argc == 2 && (std::string(args[1]).compare("-help") == 0)) {
		std::cout << "Useable parameters:\n -g\t\t\tgenerate public and private keys.\n"
			<< " -e <in> <out> [-s]\tencryption input text to the out file (with -s make signature to out.sig).\n"
			<< " -d <in> <out>\t\tdecryptoin encrypted text to the out file.\n" 
			<< " -c <sig> <decrypted>\tcompare signature with hash of decrypted text.\n" 
			<< " -s <plain> <sig>\tmake signature from plain to the sig file.\n"
			<< " -h <in> <out>\t\thash input(in) file to output(out) file." << std::endl;
		return 0;
	}

	RSA rsa;

	if(argc == 2) {
		if ((std::string(args[1]).compare("-g") == 0)) {
			rsa.generate_keys();
		} else {
			std::cout << "Bad parameters, use -help." << std::endl;
		}

		return 0;
	}

	if (argc == 4) {
		std::string a (args[1]), in_file(args[2]), out_file(args[3]);

		std::string input_text, hash_text;
		rsa.load_file_to_string(input_text, in_file);

		if (a.compare("-s") == 0) {
			HASH hash;

			hash.hash_text(input_text, hash_text);
			rsa.encrypt_text(hash_text, out_file + ".sig", true);
		} else if (a.compare("-e") == 0) {
			rsa.encrypt_text(input_text, out_file);
		} else if (a.compare("-d") == 0) {
			rsa.decrypt_text(input_text, out_file);
		} else if (a.compare("-h") == 0) {
			HASH hash;
			hash.hash_text(input_text, hash_text);

			std::ofstream keys;
			keys.open(out_file);
			keys << hash_text << "\n";
			keys.close();
			std::cout << "Hash saved into " << out_file << std::endl;
		} else if (a.compare("-c") == 0) {
			std::string in_for_hash;
			rsa.load_file_to_string(in_for_hash, out_file);

			HASH hash;

			std::string decr_hash = rsa.decrypt_text(input_text, "", true);

			if (hash.compare_hash_signed(decr_hash, in_for_hash)) {
				std::cout << "Data is correct !" << std::endl;
			} else {
				std::cout << "Data is incorrect !" << std::endl;
			}
		} else {
			std::cout << "Bad parameters, use -help." << std::endl;
		}

		return 0;
	}

	if (argc == 5) {
		std::string a(args[1]), in_file(args[2]), out_file(args[3]), b(args[4]);

		std::string input_text, hash_text;
		rsa.load_file_to_string(input_text, in_file);

		if (a.compare("-e") == 0 && b.compare("-s") == 0) {
			HASH hash;

			rsa.encrypt_text(input_text, out_file);
			hash.hash_text(input_text, hash_text);
			rsa.encrypt_text(hash_text, out_file+".sig", true);
		} else {
			std::cout << "Bad parameters, use -help." << std::endl;
		}

		return 0;
	}

	
	std::cout << "Bad parameters, use -help." << std::endl;

	return 0;
}