#include <iostream>
#include <limits>
#include <cmath>
#include <mpi.h>
#include <unistd.h>

using namespace std;


const int ROOT = 0;
const int ABSOLUTE_MAX = 1000;

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

bool is_correct_range(double a, double b){
    return (a >= -ABSOLUTE_MAX && b <= ABSOLUTE_MAX);
}

int main(int argc, char* argv[]){
    int rank, commSize;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    double a, b, a_local, b_local;
    bool correct_range = true;
    double buffer[2];
    Point result;
    MPI_Status status;

    if (rank == ROOT){
        usleep(300);

        cout << "Select desired range: " << endl;
        cout << "a = ";
        cin >> a;
        cout << "b = ";
        cin >> b;
        if(!is_correct_range(a, b)){
            cout << "Range is not correct. Range should satisfy the conditions (a >= " << 
                -ABSOLUTE_MAX << " && b <= " << ABSOLUTE_MAX << ")." << endl;
            
            correct_range = false;

            for (int i = ROOT + 1; i < commSize; i++){
                MPI_Send(&correct_range, 1, MPI_C_BOOL, i, i, MPI_COMM_WORLD);
            }

            MPI_Finalize();
            return 1;
        }

        double range[2] = {a, b};

        double start = MPI_Wtime();

        for (int i = ROOT + 1; i < commSize; i++){
            MPI_Send(&correct_range, 1, MPI_C_BOOL, i, i, MPI_COMM_WORLD);
            MPI_Send(range, 2, MPI_DOUBLE, i, i + 1000, MPI_COMM_WORLD);
        }

        a_local = a + (b - a) * rank / commSize;
        b_local = a + (b - a) * (rank + 1) / commSize;

        result = find_min_in_range(f, a_local, b_local);

        cout << "process " << rank << " [" << a_local << ";" << b_local << "]: (" << result.x << ";" << result.y << ")" << endl;

        for (int i = ROOT + 1; i < commSize; i++){
            MPI_Recv(buffer, 2, MPI_DOUBLE, i, i + 2000, MPI_COMM_WORLD, &status);
            if (buffer[1] < result.y){
                result.x = buffer[0];
                result.y = buffer[1];
            }
        }

        double end = MPI_Wtime();
        double time_elapsed = end - start;

        usleep(300);

        cout << "MPI result: " << result.x << " " << result.y << endl;
        cout << "time elapsed: " << time_elapsed << " seconds" << endl;
    }
    else{

        MPI_Recv(&correct_range, 1, MPI_C_BOOL, ROOT, rank, MPI_COMM_WORLD, &status);

        if (!correct_range){
            MPI_Finalize();
            return 1;
        }

        MPI_Recv(buffer, 2, MPI_DOUBLE, ROOT, rank + 1000, MPI_COMM_WORLD, &status);
        a = buffer[0];
        b = buffer[1];
        a_local = a + (b - a) * rank / commSize;
        b_local = a + (b - a) * (rank + 1) / commSize;

        result = find_min_in_range(f, a_local, b_local);
        buffer[0] = result.x;
        buffer[1] = result.y;

        cout << "process " << rank << " [" << a_local << ";" << b_local << "]: (" << result.x << ";" << result.y << ")" << endl;

        MPI_Send(buffer, 2, MPI_DOUBLE, ROOT, rank + 2000, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}