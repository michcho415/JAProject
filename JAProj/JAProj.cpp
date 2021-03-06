// Projekt z Języków assemblerowych
// Wykrywanie krawędzi, krzyż Robertsa
// Algorytm wykrywa krawędzie w obrazie o formacie .bmp. Korzysta z dwóch masek 2x2. Na ich podstawie obliczana jest wynikowa wartość piksela.
// Na podstawie wag w każdej masce mnożony jest odpowiedni piksel przez odpowiednią wagę i wyniki są sumowane. Po tej operacji wykonywana jest wartość
// bezwzględna dla wyników z każdej maski i są one sumowane. Wynik wpisywany jest w miejsce pierwszego piksela w masce.
// Wykonano 2.11.2020r. semestr 5, rok akademicki 2020/2021
// Michał Cholewa
// Zmiany 5.10 - odczyt i zapis .bmp
// 6.10 - stworzenie dwóch plików .dll
// 12.10 - wątki
// 13.10 - parametry wywołania programu
// 23.10 - mierzenie czasu wykonania procedury

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <stdint.h>
#include <thread>
#include <queue>
#include <chrono>


uint8_t* read_BMP(char* filename, uint8_t header[], int & height, int &width, int & bitmapheadersize)
{
	FILE* file = fopen(filename, "rb");
	if (file)
	{
		int padding = 0;
		fread(header, sizeof(uint8_t), 54, file);
		
		bitmapheadersize = *(int*)&header[14];
		bitmapheadersize += 14;
		fseek(file, 0, SEEK_SET);
		fread(header, sizeof(uint8_t), bitmapheadersize, file);

		width = *(int*)&header[18];
		height = *(int*)&header[22];



		if ((3*width) % 4 != 0)
		{
			padding = 4 - ((3 * width) % 4); 
		}
		//std::cout << padding << std::endl;
		int size = 3*width*height; 


		uint8_t* pixel_table = new uint8_t[size];
		fread(pixel_table, sizeof(uint8_t), 3*width*height, file);
		fclose(file);
		

		return pixel_table;
	}
	else {
		return nullptr;
	}
	
}

void write_BMP(char* outfilename, uint8_t* processed, uint8_t header[], int width, int height, int bitmapheadersize)
{
	FILE* file = fopen(outfilename, "wb");
	int size = 3 * width*height;
	if (file)
	{
		fwrite(header, sizeof(uint8_t), bitmapheadersize, file);
		fwrite(processed, sizeof(uint8_t), 3 * width*height, file);
		fclose(file);
	}

}

typedef void(_fastcall *MyProc1)(uint8_t*, uint8_t*, int, int);

int main(int argc, char* argv[])
{
	int threads = 1;
	char* infilename;
	char* outfilename;
	if (argc != 4)
	{
		std::cout << "Niepoprawna ilosc przelacznikow!\n";
		std::cout << "Poprawny format: (nazwaprogramu) (liczba watkow) (plik do przetworzenia) (nazwa_pliku_wynikowego)\n";
		return 0;
	}
	else {
		std::stringstream ss(argv[1]);
		ss >> threads;
		infilename = argv[2];
		outfilename = argv[3];
	}
	std::string choice;
	std::cout << "Wybierz sposób wykonania: (1) - C, (inna liczba) - ASM ";
	std::cin >> choice;
	HINSTANCE dllHandle = NULL;
	MyProc1 procedura;
	if(choice == "1")
	{
		dllHandle = LoadLibrary(L"DLLCPP.dll");
		procedura = (MyProc1)GetProcAddress(dllHandle, "fun");
	}
	else {
		dllHandle = LoadLibrary(L"JAAsm.dll");
		procedura = (MyProc1)GetProcAddress(dllHandle, "MyProc1");
	}
	




	int width = 0, height = 0, size = 0, bitmapheadersize = 0;
	uint8_t header[300];
	uint8_t* pixel_table = read_BMP(infilename, header, height, width, bitmapheadersize);
	size = 3 * height * width;
	uint8_t* processed = new uint8_t[3 * width * height];
	int total_width = 3 * width;
	//std::cout << size % threads << std::endl;


	int size_for_one_thread = size / threads;
	std::queue<std::thread> Threadqueue;
	auto start_clock = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < threads-1; ++i)
	{
		Threadqueue.push(std::move(std::thread(procedura,
											  (i*size_for_one_thread) + pixel_table,
											  (i*size_for_one_thread) + processed,
												total_width,
											  (size_for_one_thread+total_width)	
										)));
	}

	Threadqueue.push(std::move(std::thread(procedura,
		((threads - 1)*size_for_one_thread) + pixel_table,
		((threads - 1)*size_for_one_thread) + processed,
		total_width,
		size_for_one_thread)));
	
	for (int i = 0; i < threads; ++i)
	{
		Threadqueue.front().join();
		Threadqueue.pop();
	}
	auto finish_clock = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish_clock - start_clock;
	auto time_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
	std::cout << time_in_ms.count();

	write_BMP(outfilename, processed, header, width, height, bitmapheadersize);
	return 0;
}
