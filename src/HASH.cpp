#include <fstream>
#include <ostream>
#include <cmath>
#include <cstdint>
#include <algorithm>

#include "HASH.h"

bool HASH::hash_text(const std::string& input, std::string& hash)
{
	/* Delka vstupniho textu */
	int length_text = input.size();

	if (length_text > UINTMAX_MAX) {
		std::cout << "Input string higher than 2^63-1..." << std::endl;
		return false;
	}

	/* Vypocet poctu bloku */
	int count_block = ceil((length_text * 8 / MESS_BLOCK + 1) % BLOCK_SIZE);

	uint64_t mod32 = pow(2, 32);

	/* Nastaveni ABCD na init. hodnoty */
	uint32_t A0 = A_init, B0 = B_init, C0 = C_init, D0 = D_init;
	uint32_t A = A0, B = B0, C = C0, D = D0;

	/* Retezec pro uchovani jednotlivych znaku v binarni podobe */
	std::string final;

	/* Prevod vstupnich znaku do binarni podoby po 8-bitech */
	for (int i = 0; i < length_text; i++) {
		int32_t t = input[i];

		std::string number = mpz_class(t).get_str(2);
		uint32_t t_num = 8 - number.size();
		while (t_num > 0) {
			number.insert(0, "0");
			t_num--;
		}
		final += number;	
	}

	/* Pridani bitu '1' za zpravu (podle RFC) */
	final += "1";

	/* Pridani 0 do bloku po nasobcich 512 */
	int add_zero = ((count_block) * MESS_BLOCK + ((count_block - 1) * INFO_BLOCK)) - final.size();
	while (add_zero > 0) {
		final += "0";
		add_zero--;
	}

	/* Delka zpravy preveda do binarniho retezce */
	std::string inupt_bin = mpz_class(length_text * 8).get_str(2);
	int32_t count_zero = floor(inupt_bin.size() / (float)8);
		
	/* Zprava dopnena do binarniho retezce po 8-bitech */
	int32_t sum = inupt_bin.size();
	while (count_zero >= 0) {
		if(sum >= 8) {
			final += inupt_bin.substr(sum - count_zero * 8, 8);
			sum -= 8;
		} else {
			int add_zero = 8 - sum;
			while (add_zero > 0) { final += "0"; add_zero--; }
			final += inupt_bin.substr(0, sum);
		}
			count_zero--;
	}

	/* Doplneni zpravy nulami do nasobku 512 */
	int size_bits = (count_block * BLOCK_SIZE) - final.size();
	while (size_bits > 0) {
		final += '0';
		size_bits--;
	}

 	/* Algoritmus vypoctu jednotlivych koef. ABCD podle schematu MD5 */
	for (int i = 0; i < count_block; i++) {
		A0 = A; B0 = B; C0 = C; D0 = D;

		/* Zpracovani po blocich BLOCK_SIZE, respektive po 512-bitech */
		std::string prep = final.substr( i * BLOCK_SIZE, BLOCK_SIZE);
		int pos = 0;	
		for (int j = 1; j <= 4; j++) {
			for (int k = 0; k < 16; k++) {		
				uint mes_num;

				if(j == 1) {
					mes_num = pos;
				} else if (j == 2) {
					mes_num = (5 * pos + 1) % 16;
				} else if (j == 3) {
					mes_num = (3 * pos + 5) % 16;
				} else {
					mes_num = (7 * pos) % 16;
				}
			
				/* Zpracovani vstupniho retezce po boku 32-bitu podle little endianu */
				std::string sub, f = prep.substr(mes_num * 32, 32);
				int p = 3;			
 				while (p >= 0) {
					sub += f.substr(p * 8, 8);
					p--;
				}

				uint32_t m = (uint32_t)std::stol(sub, nullptr, 2);
				uint32_t func = hash_functions(B, C, D, j);
				double ebs = sin(pos + 1) < 0.0 ? sin(pos + 1)* (-1) : sin(pos + 1);
				uint32_t con = floor(mod32 * ebs);

				A = B + (rotl(A + func + m + con, s[pos]));
				uint32_t tmp_D = D;
				D = C;
				C = B;
				B = A;
				A = tmp_D;

				pos++;		
			}
		}

		A = (A0 + A);
		B = (B0 + B);
		C = (C0 + C);
		D = (D0 + D);
	}

	/* Vysledny koef. (ABCD) ulozeny do retezce v hex. soustave */
	uint32_t abcd[4] = {A, B, C, D};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			uint32_t post = (abcd[i] >> j * 8) & 0xFF;
			if (post <= 9) {
				hash += "0";
			}
			hash += mpz_class(post).get_str(16);
		}
	}

	//std::cout << "\tHASH: " << hash << std::endl;

	return true;
}

uint32_t HASH::rotl(const uint32_t a, uint32_t i)
{
	return ((a << i) | (a >> (32 - i)));
}

uint32_t HASH::hash_functions(uint32_t B, uint32_t C, uint32_t D, uint32_t id_func)
{
	uint32_t tmp;
	/* V kazde iteraci (po 16 krocich) pouzita jina funkce dle RFC */ 
	switch(id_func) {
		case 1: 
			tmp = (B & C) | ((~B) & D);
			break;
		case 2: 
			tmp = (B & D) | (C & (~D));
			break;
		case 3: tmp = B ^ C ^ D;
			break;
		case 4: tmp = C ^ (B | (~D));
			break;
	}

	return tmp;
}

bool HASH::compare_hash_signed(const std::string& signed_hash, const std::string& input_text)
{
	std::string hash;
	hash_text(input_text, hash);

	std::cout << "\tHASH SIG:\t" << signed_hash << std::endl;
	std::cout << "\tHASH PLAIN:\t" << hash << std::endl;

	return (hash.compare(signed_hash) == 0);
}
