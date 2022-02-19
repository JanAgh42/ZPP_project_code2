
Kompilácia programu bola otestovaná v Debian Linux aj Windows 10 pomocou gcc spôsobom, aký nám posielal pán Zelenka. Okrem toho bolo otestované aj správne alokovanie a dealokovanie dynamických polí s nástrojom Valgrind (iba v Debian). Žiaden nástroj nenašiel chybu v kóde. 

Pri spustení programu v Debian sa občas stalo, že niektorý reťazec z dynamicky alokovaných polí nebol vypísaný do konzoly (podľa mňa to bolo spôsobené tým, že v /home adresári nemám skoro žiadne voľné miesto). Pri spustení programu vo Windows 10 som nemal žiadne podobné problémy s výpisom.

Ján Ágh