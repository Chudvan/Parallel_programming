#include <iostream>
#include <cmath>
#include <mutex>
#include <thread>
#include <vector>
#include <functional>

double f(double x){
    return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

std::mutex mutex;

struct thread_params{
	int thread_num;
	double a;
	double b;
};

double min_x_, min_y_;

void foo(thread_params params){
    double a = params.a;
    double b = params.b;
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

    mutex.lock();
    std::cout << "thread " << params.thread_num << " "
		<< min_y << " " << min_x << " prev min_y: " 
		<< min_y_ << std::endl;

	if (min_y < min_y_) {
		min_y_ = min_y;
		min_x_ = min_x;
	}
    mutex.unlock();
}

int main(){
    const int N = 5;
	thread_params params[N];
    std::vector <std::thread> th_vec;

	double b = 100;
	min_y_ = std::numeric_limits<double>::max();

	for (int i = 0; i < N; i++) {
		params[i].thread_num = i;
		params[i].a = b * i / N;
		params[i].b = b * (i + 1) / N;

		std::thread t(foo, params[i]);
        th_vec.push_back(std::move(t));
	}

	for (int i = 0; i < N; i++) {
		th_vec[i].join();
	}

	std::cout << std::endl << min_y_ << " " << min_x_ << std::endl;
}