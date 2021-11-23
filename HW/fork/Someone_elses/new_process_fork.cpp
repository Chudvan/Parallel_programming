#include <iostream>
#include <fstream>
#include <math.h>

double f(double x)
{
    return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

int main(int argc, char* argv[])
{
//    std::cout << "argc = " << argc << std::endl;
    if (argc != 4) {
        return 1;
    }
//    std::cout << "argv[0] = " << argv[0] << std::endl;
//    std::cout << "argv[1] (id) = " << argv[1] << std::endl;
//    std::cout << "argv[2] (begin) = " << argv[2] << std::endl;
//    std::cout << "argv[3] (end) = " << argv[3] << std::endl;

    int id = atoi(argv[1]);
    double begin = atof(argv[2]);
    double end = atof(argv[3]);
    double x = begin;
    double cur_min = f(x);
    x += 0.0001;

    while (x < end)
    {
        double cur_f = f(x);

        if (cur_f < cur_min)
            cur_min = cur_f;

        x += 0.0001;
    }

    std::ofstream out;
    char firstPart[50] = "/home/ivan/CPPProjects/HW/Someone_elses/Files/";
    char lastPart[10] = ".txt";
    char fileName[90];
    sprintf(fileName, "%s%d%s", firstPart, id, lastPart);
    out.open(fileName);
    if (out.is_open())
    {
        out << cur_min << std::endl;
    }
    out.close();

    std::cout << "End of " << id << " process. min(f) = " << cur_min << ". Result is written in " << fileName << std::endl;
}
