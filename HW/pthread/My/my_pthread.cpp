#include <iostream>
#include <cmath>
#include <limits>
#include <pthread.h>

double f(double x){
    return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

struct thread_params{
	int thread_num;
	double a;
	double b;
};

const int N = 5;
double min_x_[N], min_y_[N];

void* foo(void* task){
    thread_params* params = (thread_params*) task;
    double a = params->a;
    double b = params->b;
    int id = params->thread_num;
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

    std::cout << "thread " << params->thread_num << " "
		<< min_y << " " << min_x << std::endl;

	min_y_[id] = min_y;
	min_x_[id] = min_x;
}

int main(){
	thread_params params[N];
    pthread_t pthreads[N];

	double b = 100;

	for (int i = 0; i < N; i++) {
		params[i].thread_num = i;
		params[i].a = b * i / N;
		params[i].b = b * (i + 1) / N;

		int return_code = pthread_create(&pthreads[i], NULL, foo, &params[i]);

        if (return_code == 0)
            std::cout << "thread " << i << " was succesfully started" << std::endl;
        else
            std::cout << "thread " << i << " was not succesfully started" << std::endl;
	}

	for (int i = 0; i < N; i++) {
		pthread_join(pthreads[i], NULL);
	}

    double min_x, min_y;
    min_y = std::numeric_limits<double>::max();
    for (int i = 0; i < N; i++) {
        if (min_y_[i] < min_y){
            min_y = min_y_[i];
            min_x = min_x_[i];
        }
    }

	std::cout << std::endl << min_y << " " << min_x << std::endl;
}