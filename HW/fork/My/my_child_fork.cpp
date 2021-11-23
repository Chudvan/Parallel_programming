#include <cmath>
#include <iostream>
#include <limits>
#include <fstream>

double f(double x) {
    return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

int main(int argc, char* argv[])
{

    //std::cout << " argc " << argc << std::endl;

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

	std::cout << "process " << process_num << " "
		<< min_y << " " << min_x << std::endl;

	std::ofstream out;
	std::string path_to_dir = "/home/ivan/CPPProjects/HW/fork/My/Files/";
	std::string ext = ".txt";

	out.open(path_to_dir + std::to_string(process_num) + ext);
	out << min_y << " " << min_x;
	out.close();

    return 0;
}