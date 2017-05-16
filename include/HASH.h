#include <gmpxx.h>
#include <gmp.h>
#include <iostream>
#include <string>

#define BLOCK_SIZE		512		/* Velikost jednoho bloku */
#define MESS_BLOCK		448 	/* Cast bloku vyhrazena pro zpravu */
#define INFO_BLOCK		64		/* Cast bloku vyhrazena pro uchovani velikosti vstupniho retezce */

class HASH {

private:

	/* Inicializacni konstanty. */
	const uint32_t A_init = 0x67452301, B_init = 0xefcdab89, C_init = 0x98badcfe, D_init = 0x10325476;

	/* Jednotlive hodnoty urcene pro levou rotaci v jednotlive iteraci (urceny podle RFC) */
	const uint32_t s[64] = {
	 	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
     	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
     	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
 	};

	/* Nelinearni funkce 
		
	   B - hodnota B bloku.
	   C - hodnota C bloku
	   D - hodnota D bloku.
	   id_func - id pouzite funkce.
	*/
	uint32_t hash_functions(uint32_t B, uint32_t C, uint32_t D, uint32_t id_func);
	

	/* Leva rotace o i bitu.

	   a - vstupni hodnota pro rotaci.
	   i - pocet posunu do leva cyklicky. 
	*/
	uint32_t rotl(const uint32_t a, uint32_t i);


public:

	/* Provede zaheshovani vstupniho retezce.

		input - vstupni retezec pro hashovani.
		hash - vysledny hash.

		return - true = hashovani probleho uspesne.
	*/
	bool hash_text(std::string& input, std::string& hash);

	/* Slouzi k porovnani desifrovanych dat s digitalnim podpisem.

	   sighned_hash - desifrovany digitalni podpis.
	   input_text -retezec desifrovanych dat.

	   return - true = autentizace je spravna.
	*/
	bool compare_hash_signed(const std::string& signed_hash, std::string& input_text);

};