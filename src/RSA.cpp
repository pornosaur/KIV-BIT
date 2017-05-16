#include <fstream>
#include <ostream>
#include <ctime>
#include <cmath>
#include <list>

#include "RSA.h"
#include "HASH.h"

mpz_class RSA::generate_prime()
{
	mpz_class min_value;
	mpz_ui_pow_ui(min_value.get_mpz_t(), 10, DIGIT);
	
	/* Vygenerovani nahodneho cisla o poctu cislis = DIGIT. */
	gmp_randclass random(gmp_randinit_mt);
	random.seed(time(NULL));
	mpz_class tmp_value = min_value + random.get_z_range(min_value);

	/* Zvzsuj hodnotu vygenerovaneho cisla dokud neni prvocislem. */
	while(mpz_probab_prime_p(tmp_value.get_mpz_t(), RAB_MILL_CONST) != 1) {
		tmp_value += 1;
	}

	return tmp_value;
}

mpz_class RSA::generate_public_key(mpz_class from, mpz_class to)
{
	/* Vygenerovani nahodneho verejneho klice */
	mpz_class gcd;
	gmp_randclass random(gmp_randinit_mt);
	random.seed(time(NULL));
	e = random.get_z_range(to - from) + from;

	/* Pokud hodnoty e a x jsou soudelne, tak zvysuj hodnotu e tak, aby porad platilo, 
	    ze je mensi nez x do te doby, dokud bude s x nesoudelne. */
	mpz_gcd(gcd.get_mpz_t(), e.get_mpz_t(), x.get_mpz_t());
	while (gcd != 1) {
		e++;
		
		if (e >= x) {
			random.seed(time(NULL));
			e = random.get_z_range(to - from) + from;
		}

		mpz_gcd(gcd.get_mpz_t(), e.get_mpz_t(), x.get_mpz_t());
	}

	/* Pokud by nahodou e bylo vetsi nez x, tak vyhod chybu. */
	if (e >= x) {
		std::cout << "\nWrong generated the public key, try again..." << std::endl;
		std::exit(0);
	}

	std::cout << "\nThe public key was generated properly to " << FILE_PUBLIC_NAME << " file." << std::endl;

	return e;
}

void RSA::generate_private_key(mpz_class& result, mpz_class a, mpz_class b)
{
	/* Generovani soukromeho klice pomoci rozsireneho Eklidova algoritmu. */

	mpz_class z, tmp_a = a, tmp_b = b;
	std::list<std::pair<mpz_class, mpz_class>> mod_numbers;

	/* Jednotlive deleni cisel az do zbytku = 1. Postupne se pamatuji hodnoty zbytku a nasobku.*/
	while (z != 1) {
		z = tmp_a % tmp_b;
		mod_numbers.push_back(std::pair<mpz_class, mpz_class> (z, (tmp_a / tmp_b)));

		tmp_a = tmp_b;
		tmp_b = z;

		if (z == 0) {
			std::cout << "\nWrong generated the private key, try again..." << std::endl;
			std::exit(0);
		}
	} 

	mpz_class r1("1");
	mpz_class r2 (mod_numbers.front().second );

	mpz_class p1("1");
	mpz_class p2("0");

	tmp_a = b; 
	tmp_b = a;

	mod_numbers.pop_front();

	/* Zpetny chod Eukl. alg. k zjisteni vyslednych nasobku cisla a a b, respektive x a e. */
	mpz_class A, B;
	for (auto z : mod_numbers) {
		A = z.second * r2 + p1;
		B = z.second * r1 + p2;
		
		p1 = r1;
		p2 = r2;

		r1 = A;
		r2 = B;

		mpz_class t(tmp_a);
		tmp_a = tmp_b;
		tmp_b = t;
	}

	/* Vyzsledny nasobek cisla b, spektive e, je hodnota k zjisteni verejneho klice jeste deleni
		module hodnota a, respektive x. */
	if (cmp(tmp_a, a)) {
		result = a - B;
	} else {
		result = A % a;
	}

	std::cout << "The private key was generated properly to " << FILE_PRIVATE_NAME << " file." << std::endl;
}

void RSA::generate_keys()
{
	p = generate_prime();
	q = generate_prime();

	/* Pokud q je stejne jako p, pak vygeneruj nove q, dokud nebude rozdilne od p. */
	while(cmp(p, q) == 0) {
		q = generate_prime();
	}

	n = p * q;
	x = (p - 1) * (q - 1) ;

	std::ofstream keys;

	generate_public_key(3, x);
	keys.open(FILE_PUBLIC_NAME);
	keys << e << "\n";
	keys << n;
	keys.close();

	generate_private_key(d, x, e);
	keys.open(FILE_PRIVATE_NAME);
	keys << d << "\n";
	keys << n;
	keys.close();
}

std::string RSA::encrypt_text(const std::string& plain_text, const std::string& out_file, bool sig)
{
	std::string input, prep;

	if (sig) {
		load_private_key();
	} else { 
		load_public_key();
	}

	/* Vypocteni hodnot pro velikost bloku a poctu bloku ze vstupniho plain textu. */
	int len_text = plain_text.size();
	int size_block = floor(n.get_str().size() / 3.0) - 1;
	int count_block = ceil(len_text / (float)size_block);

	/* Zpracovani jednotlivych bloku - prevod na cislo a kazdy blok je 
	   zasifrovan pomoci verejneho klice. */
	for(int j = 0; j < count_block; j++) {
		std::string sub_text = plain_text.substr(j * size_block, size_block);

		/* Prevod bloku s textem na ciselnou hodnotu podle ASCII hodnot. */
		for(unsigned int i = 0; i < sub_text.size(); i++) {
			unsigned long int t = sub_text[i];

			/* Pokud je hodnota znaku < 100, pak je na zacatek pridana 3. 
			   Je to z duvodu, ze trida mpz_class neumi nacist hodnotu s nulami
			   na zacatku (napr. 067). */
			if (t < 100) {
				prep += '3';
				prep += std::to_string(t);
			} else {
				prep += std::to_string(t);
			}
		}

		/* Prevedena cast bloku na cislo je zasifrovana pomoci e(verejneho klice) a hodnoty n. */
		mpz_class crypt;
		if (!sig) {
			mpz_powm_sec(crypt.get_mpz_t(), mpz_class(prep).get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
		} else {
			mpz_powm_sec(crypt.get_mpz_t(), mpz_class(prep).get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
		}
		
		std::string tmp = crypt.get_str();

		/* Doplneni bloku nulami na zacatek na pozadovanou velikost. */
		int max =  n.get_str().size();
		while(tmp.size() < max) {
			tmp.insert(0, "0");
		}	
		input += tmp;

		prep.clear();
	}

	/* Ulozeni zasifrovaneho klice do souboru */
	if (!out_file.empty()) {
		std::ofstream encrypted_output;
		encrypted_output.open(out_file);
		encrypted_output << input;
		encrypted_output.close();

		std::cout << "Encrypted text was saved into " << out_file << std::endl;
	}

	return input;
}

std::string RSA::decrypt_text(std::string& crypt_text, const std::string& out_file, bool sig)
{
	if (sig) {
		load_public_key();
	} else {
		load_private_key();
	}

	std::string text;

	/* Vypocteni hodnot pro velikost bloku a poctu bloku ze vstupniho crypt textu. */
	int len_text = crypt_text.size();
	int size_block = n.get_str().size();
	int count_block = ceil(len_text / (float)size_block);


	/* Zpracovani jednotlivych bloku a jejich nasledne desifrovani. */
	for (int j = 0; j < count_block; j++) {
		std::string tmp = crypt_text.substr(j * size_block, size_block);

		/* Odstraneni 0 ze zacatku bloku. */
		int count_erase = 0;
		for (int i = 0; i < size_block; i++) {
			if (tmp[i] == '0') {
				count_erase++;
			} else {
				break;
			}
		}

		tmp.erase(0, count_erase);

		/* Desifrovani zasifrovaneho textu pomoci d(soukromeho klice) a hodnoty n; */
		mpz_class test;
		if(!sig) { 
			mpz_powm(test.get_mpz_t(), mpz_class(tmp).get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
		} else {
			mpz_powm(test.get_mpz_t(), mpz_class(tmp).get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
		}
		std::string tmp_text = test.get_str();

		/* Prevod jednotlivych cisel z desifrovaneho textu na klasicky text (znaky). */
		for(unsigned int i = 0; i < tmp_text.size(); i += 3) {
			std::string num;

			/* Pokud zacina na 3, tak ignorovat a vzit pouze posledni dve hodnoty. */
			if (tmp_text[i] == '3') {
				num = tmp_text.substr(i + 1, 2);
			} else {
				num = tmp_text.substr(i, 3);
			}
			text += (char) std::stoi(num);
		}
	}
	//text += "\n";

	/* Ulozeni desifrovaneho textu do souboru */
	if (!out_file.empty()) {
		std::ofstream encrypted_output;
		encrypted_output.open(out_file);
		encrypted_output << text;
		encrypted_output.close();

		std::cout << "Decrypted text was saved into " << out_file << std::endl;
	}

	return text;
}

void RSA::load_key(const std::string& name_file, std::string& first, std::string& second)
{	
	std::ifstream key;
	std::string line;

	key.open(name_file);
	if (key.is_open()) {
		getline(key, first);
		getline(key, second);

		key.close();
	} else {
		std::cout << "\nCould not load the file with key." << std::endl;
		std::exit(1);
	}

	std::cout << "\nThe " << name_file <<" was successfully loaded." << std::endl;
}

void RSA::load_public_key(const std::string& name)
{
	std::string e_const, n_const;
	load_key(name, e_const, n_const);

	e = e_const;
	n = n_const;
}

void RSA::load_private_key(const std::string& name)
{
	std::string d_const, n_const;
	load_key(name, d_const, n_const);

	d = d_const;
	n = n_const;
}

void RSA::load_file_to_string(std::string& input, const std::string& file_name)
{
	std::ifstream file;
	std::string line;

	file.open(file_name);
	if (file.is_open()) {
		while (getline(file, line)) {
				input += line + "\n";
		}

		if (line != "") {
			input.pop_back();
		}

		file.close();
	} else {
		std::cout << "\nCould not load " << file_name << " file." << std::endl;
		std::exit(0);
	}
}