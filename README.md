# KIV-BIT
Implementace RSA sifry a MD5 hash funkce.

Testováno pouze OS Linux.

Pro každou architekturu je  nutné přeložit přiloženou knihovnu GMP.

K přeložení slouží shell skript build.sh

install.sh vytvori adresar lib, kde je přeložena knihovna.

K přeložení samotné aplikace slouží CMakeLists, kde je zajištěno linkování knihoven a vytvoření spustitelného souboru main.

Možné parametry je možné vypsat pomocí parametru -help

