// Projekt z Języków assemblerowych
// Wykrywanie krawędzi, krzyż Robertsa
// Algorytm wykrywa krawędzie w obrazie o formacie .bmp. Korzysta z dwóch masek 2x2. Na ich podstawie obliczana jest wynikowa wartość piksela.
// Na podstawie wag w każdej masce mnożony jest odpowiedni piksel przez odpowiednią wagę i wyniki są sumowane. Po tej operacji wykonywana jest wartość
// bezwzględna dla wyników z każdej maski i są one sumowane. Wynik wpisywany jest w miejsce pierwszego piksela w masce.
// Wykonano 2.11.2020r. semestr 5, rok akademicki 2020/2021
// Michał Cholewa
// Zmiany 14.10
// Stworzenie funkcji w cpp
// 

#include "stdafx.h"
#include <stdint.h>
#include <cmath>

void fun(uint8_t* pixel_table, uint8_t* destination, int width, int size)
{
	int sum1 = 0, sum2 = 0;
	int wynik1 = 0;
	int wynik2 = 0;
	int wynik = 0;
	int height = (size / width);
	for (int j = 0; j < size-width; j += 3) //przejscie co 3 na kolejny piksel ; rozdzielic na dwie pętle dla kolumn i wierszy
		{

			for (int i = 0; i < 3; ++i) //przejscie miedzy kolorami
			{

				wynik1 = pixel_table[i + j];
				wynik2 = pixel_table[j + (width) + i + 3];
				sum1 = abs(wynik1 - wynik2);

				wynik1 = pixel_table[(i + j) + 3];
				wynik2 = pixel_table[j + (width) + i];
				sum2 = abs(wynik1 - wynik2);

				wynik = sum1 + sum2;
				destination[j + i] = wynik;

			}
		}
	
}
