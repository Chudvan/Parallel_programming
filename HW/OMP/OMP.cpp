#include <iostream>
#include <limits>
#include <cmath>
#include <omp.h>
#include <chrono>

using namespace std;

double f(double x){
	return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

struct Point
{
    double x;
    double y;
};

Point find_min_in_range(double foo(double), double a, double b){
	double h = 1e-4;
	double x = a;
	double min_x, min_y;
	double cur_f;
    min_x = x;
	min_y = std::numeric_limits<double>::max();
	
	while(x < b){
		cur_f = f(x);
		if(cur_f < min_y){
			min_y = cur_f;
			min_x = x;
		}
		x += h;
	}

    return Point{min_x, min_y};
}

bool is_correct_N(int N){
    return (N > 0 && N <= 16);
}

int main(){
    int N;
    cout << "Select a number of threads (N): ";
    cin >> N;

    if (!is_correct_N(N)){
        cout << "N is not correct. N should be in range (N > 0 && N <= 16)." << endl;
        return 1;
    }

    omp_set_dynamic(false);
    omp_set_num_threads(N);

    double a = 0;
	double b = 100;

    Point global_result{a, std::numeric_limits<double>::max()};
    double i;

    auto start = chrono::steady_clock::now();

    #pragma omp parallel
    {
        double i = omp_get_thread_num();
        Point local_result = find_min_in_range(f, b * i / N, b * (i + 1) / N);
        #pragma omp critical (cout_and_Point)
        {
            cout << "thread " << i << ": " << local_result.x << " " << local_result.y << endl;
            if(local_result.y < global_result.y){
                global_result = local_result;
		    }
        }
    }

    auto end = chrono::steady_clock::now();
    double time_elapsed = double((end - start).count()) *
        chrono::steady_clock::period::num /
        chrono::steady_clock::period::den;

    cout << "global OMP result: " << global_result.x << " " << global_result.y << endl;
    cout << "time elapsed: " << time_elapsed << " seconds" << endl;

    return 0;
}