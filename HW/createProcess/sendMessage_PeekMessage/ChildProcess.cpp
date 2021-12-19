#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <limits>
#include <fstream>

const char* LPSZCLASSNAME = "MyClass";
const char* LPWINDOWNAME = "√лобальный минимум";

double f(double x) {
	return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

int main(int argc, char* argv[])
{

	if (argc != 4)
	{
		printf("Usage: %s %s %s %s\n", argv[0], "process_num", "a", "b");
		return 1;
	}

	int process_num = std::stoi(argv[1]);
	double a = std::stod(argv[2]);
	double b = std::stod(argv[3]);
	double h = 1e-4;
	double x = a;
	double min_x, min_y;
	double cur_f;

	min_y = std::numeric_limits<double>::max();

	while (x < b) {
		cur_f = f(x);
		if (cur_f < min_y) {
			min_y = cur_f;
			min_x = x;
		}
		x += h;
	}

	HWND hwndFound;
	hwndFound = FindWindow(LPSZCLASSNAME, LPWINDOWNAME);
	//std::cout << hwndFound << std::endl;
	
	std::string str_ = std::to_string(min_y) + " " + std::to_string(min_x);
	char res[100];
	strcpy_s(res, sizeof(res), str_.c_str());
	std::cout << res << std::endl;
	
	/*
	LPARAM lparam = (LPARAM)(str_.c_str());
	std::cout << lparam << std::endl;
	std::cout << ((char*)lparam) << std::endl;
	*/

	COPYDATASTRUCT cd;
	cd.lpData = res;
	cd.cbData = strlen(res) + 1;

	SendMessage(hwndFound, WM_COPYDATA, 0, (LPARAM)&cd);

    //Sleep(3000);

	return 0;
}
