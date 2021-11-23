/*
#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

DWORD WINAPI thread2(LPVOID);

int main()
{
	cout << "First thread\n";
	HANDLE thread = CreateThread(NULL, 0, thread2, NULL, 0, NULL);
	cout << "More data from first thread\n";
	for (int i = 0; i < 1000000; i++)
	{
	}
	cout << "Even more data from first thread\n";
	_getch();
	return 0;
}

DWORD WINAPI thread2(LPVOID t)
{
	cout << "Second thread\n";
	return 0;
}
*/