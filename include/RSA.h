#include <gmpxx.h>
#include <gmp.h>
#include <iostream>
#include <string>

#define DEFAULT_DIGIT	255		/* Konstanta pro urceni radu prvocisla.	  */
#define RAB_MILL_CONST	 30		/* Konstanta pro presnost testu prvocisla. */

#define FILE_PUBLIC_NAME	"public.key"	/* Vychozi nazev souboru pro verejny klic. */
#define FILE_PRIVATE_NAME	"private.key"	/* Vychozi nazev souboru pro soukromy klic. */

class RSA {

private:
	/* Maximalni pocet cislic pro vygenerovani p a q. */
	const unsigned int DIGIT;

	/* Hodnoty dulezite pro vypocet klicu.

		p, q - nahodne vygenerovana prvocisla.
		n - p*q.
		x - (p-1)*(q-1).
		e - verejny klic.
		d - soukromy klic.
	*/
	mpz_class p, q, n, x, e, d;

	/*
		Vygenerovani nahodneho prvocisla.
	*/
	mpz_class generate_prime();

	/* Slouzi k vygenerovani soukromeho klice, ktery je zavisly na verejnym klici.

		result - vysledny privatni klic.
		a - hodnota x=(p-1)*(q-1).
		b - hodnota e(verejny klic).
	*/
	void generate_private_key(mpz_class& result, mpz_class a, mpz_class b);

	/* Slouzi k vygenerovani dvojice klicu - verejny a soukromy. Verejny je 
		ulozen do souboru public.key a soukromy klic do souboru private.key

		from - minimalni pocet mist cisla.
		to - maximalni pocet mist cisla.
	*/
	mpz_class generate_public_key(mpz_class from, mpz_class to);



	/* Slouzi k nacteni klice ze souboru.

		name_file - nazev souboru s klicem.
		first - nacteni prvni casti klice (e - public; d - private).
		second - nacteni druhe casti klice (n).

	*/
	void load_key(const std::string& name_file, std::string& first, std::string& second);

	/* Slouzi k nacteni verejneho klice ze souboru. 
		
		name - nazev souboru s verejnym klice (vyzchozi - public.key).
	*/
	void load_public_key(const std::string& name = FILE_PUBLIC_NAME /* public.key */);

	/* Slouzi k nacteni soukromeho klice ze souboru. 

		name - nazev souboru se skoukromym klice (vyzchozi - private.key).
	*/
	void load_private_key(const std::string& name = FILE_PRIVATE_NAME /* private.key */);

public:
	/* Vytvoreni tridy RSA s vyzchozim max. poctem cislic pro generovani p a q. */
	RSA() : DIGIT(DEFAULT_DIGIT) {};

	/* Vytovreni tridy RSA ze zadanim max. poctu cislic pro generovani p a q.
		
	   digit - max. pocet cislic pro generovani cislic p a q.
	*/
	RSA(unsigned int digit) : DIGIT(digit) {};

	/* Vygenerovani dvojicek klic - verejny a skoukromy. */
	void generate_keys();

	/* Zasifrovani plain textu. 

	   file - nazev souboru s plain textem.
	*/
	std::string encrypt_text(const std::string& plain_text, const std::string& out_file, bool sig = false);

	/* Rozsifrvani zasifrovaneho textu 

	   file - nazev souboru s cypher textem.
    */
	std::string decrypt_text(std::string& crypt_text, const std::string& out_file, bool sig = false);

	/* Slouzi k nacteni obsahu souboru od retezce.

		input - nacteny retezec.
		file_name - nazev souboru.
	*/
	void load_file_to_string(std::string& input, const std::string& file_name);

};