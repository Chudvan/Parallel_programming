#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <iostream>
#include <limits>
#include <cmath>
#include <windows.h>

double f(double x) {
	return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

struct thread_params{
	int thread_num;
	double a;
	double b;
};

CRITICAL_SECTION sc;
double min_x_, min_y_;

DWORD __stdcall foo(void* arg) {
	thread_params* task = (thread_params*) arg;

	double a = task->a;
	double b = task->b;
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

	EnterCriticalSection(&sc);
	std::cout << "thread " << task->thread_num << " "
		<< min_y << " " << min_x << " prev min_y: " 
		<< min_y_ << std::endl;

	if (min_y < min_y_) {
		min_y_ = min_y;
		min_x_ = min_x;
	}
	LeaveCriticalSection(&sc);

	return 0;
}

int main() {

	InitializeCriticalSection(&sc);

	const int N = 5;
	HANDLE handles[N];
	thread_params params[N];

	double b = 100;
	min_y_ = std::numeric_limits<double>::max();

	for (int i = 0; i < N; i++) {
		params[i].thread_num = i;
		params[i].a = b * i / N;
		params[i].b = b * (i + 1) / N;

		handles[i] = CreateThread(NULL, 0, foo, &params[i], 0, NULL);
		if (!handles[i]) {
			std::cout << "Error " << GetLastError();
		}
	}

	WaitForMultipleObjects(N, handles, true, INFINITE);

	std::cout << std::endl << min_y_ << " " << min_x_ << std::endl;
}