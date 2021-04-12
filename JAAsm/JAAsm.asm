; Projekt z J�zyk�w Assemblerowych
; Wykrywanie kraw�dzi, krzy� Robertsa
; Algorytm wykrywa kraw�dzie w obrazie o formacie .bmp. Korzysta z dw�ch masek 2x2. Na ich podstawie obliczana jest wynikowa warto�� piksela.
; Na podstawie wag w ka�dej masce mno�ony jest odpowiedni piksel przez odpowiedni� wag� i wyniki s� sumowane. Po tej operacji wykonywana jest warto��
; bezwzgl�dna dla wynik�w z ka�dej maski i s� one sumowane. Wynik wpisywany jest w miejsce pierwszego piksela w masce.
; Wykonano 5.11.2020r. semestr 5, rok akademicki 2020/2021
; Micha� Cholewa
; Zmiany
; 26.10 stworzenie p�tli i przetwarzania danych w ASM
; 27.10 poprawki optymalizacyjne
; 5.11 poprawki optymalizacyjne
.code
MyProc1 proc
push rbx ; rbx na stos
xor rbx, rbx ; zerowanie rbx
mov RAX, R9; zapisz rozmiar tablicy do przetworzenia w RAX
sub RAX, r8 ; odejmij dlugosc ostatniego wiersza, aby go nie przetwarzac
xor r9, r9 ;nowa linijka zerowanie r9
mov r9, rcx ; przenies adres
add r9, r8 ; dodaj do adresu tablicy szerokosc wiersza
	petlaprog:	
		movups xmm0, [rcx+4*rbx] ; wczytanie z tablicy elementy(i, j) - lewy, g�rny pixel z maski
		movups xmm2, [rcx+4*rbx + 3] ; elementy (i+1, j) - prawy, g�rny pixel z maski

		movups xmm1, [r9+4*rbx + 3] ; wczytanie wiersza ponizej (i+1, j+1) - dolny prawy pixel z maski
		movups xmm3, [r9+4*rbx] ; elementy (i, j+1) - dolny, lewy pixel z maski
		
		psubb xmm0, xmm1 ;odejmowanie wartosci wg pierwszej maski
		pabsb xmm0, xmm0 ;wartosc bezwzgl
		psubb xmm2, xmm3 ;odejmowanie wartosci wg drugiej maski
		pabsb xmm2, xmm2 ;wartosc bezwzgl

		paddb xmm0, xmm2 ;dodanie obu warto�ci wyniku

		movups [rdx + 4*rbx], xmm0 ;zapisanie przetworzonych bajt�w
		add rbx, 4 ; przesuniecie
		sub rax, 16; odejmij 16 przetworzonych bajtow, dzia�a jako licznik
		cmp rax, 0 ;sprawdzenie czy wszystkie dane przetworzone
		jnz petlaprog
		pop rbx ; pobranie rbx ze stosu
		
ret
MyProc1 endp
end