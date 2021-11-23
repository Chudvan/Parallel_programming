#include <iostream>
#include <limits>
#include <cmath>

double f(double x){
	return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

int main(){
	double a = 0;
	double b = 100;
	double h = 1e-4;
	double x = a;
	double min_x, min_y;
	double cur_f;
	min_y = std::numeric_limits<double>::max();
	
	while(x < b){
		cur_f = f(x);
		if(cur_f < min_y){
			min_y = cur_f;
			min_x = x;
		}
		x += h;
	}
	
	std::cout << min_y << " " << min_x;
}
