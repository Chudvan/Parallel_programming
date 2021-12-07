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

void min_point_option(void* invec, void* outvec, int* len, MPI_Datatype data_type){
    Point* inpoint = (Point*) invec;
    Point* outpoint = (Point*) outvec;

    if (inpoint->y < outpoint->y) {
        outpoint->y = inpoint->y;
        outpoint->x = inpoint->x;
    }
}

int main(int argc, char* argv[]){
    int rank, commSize;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commSize);

    double a_local, b_local;
    bool correct_range = true;
    double range[2], start, end;
    Point local_result, global_result;

    if (rank == ROOT){
        cout << "Select desired range: " << endl;
        cout << "a = ";
        cin >> range[0];
        cout << "b = ";
        cin >> range[1];
        if(!is_correct_range(range[0], range[1])){
            cout << "Range is not correct. Range should satisfy the conditions (a >= " << 
                -ABSOLUTE_MAX << " && b <= " << ABSOLUTE_MAX << ")." << endl;
            
            correct_range = false;
        }
        else{
            start = MPI_Wtime();
        }
    }

    MPI_Bcast(&correct_range, 1, MPI_C_BOOL, ROOT, MPI_COMM_WORLD);
    if (!correct_range){
            MPI_Finalize();
            return 1;
    }

    MPI_Bcast(&range, 2, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    //cout  << "process " << rank << " [a;b]: [" << range[0] << ";" << range[1] << "]" << endl;
    
    a_local = range[0] + (range[1] - range[0]) * rank / commSize;
    b_local = range[0] + (range[1] - range[0]) * (rank + 1) / commSize;

    local_result = find_min_in_range(f, a_local, b_local);
    cout << "process " << rank << " [" << a_local << ";" << b_local << "]: (" << 
        local_result.x << ";" << local_result.y << ")" << endl;

    //Create a user-defined MPI_Datatype and MPI_Op
    MPI_Datatype point_data_type;
    MPI_Type_contiguous(2, MPI_DOUBLE, &point_data_type);
    MPI_Type_commit(&point_data_type);
    MPI_Op operation;
    MPI_Op_create((MPI_User_function*)min_point_option, 1, &operation);

    MPI_Reduce(&local_result, &global_result, 1, point_data_type, operation, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT){
        cout << "MPI_all result: " << global_result.x << " " << global_result.y << endl;
        end = MPI_Wtime();
        double time_elapsed = end - start;
        cout << "time elapsed: " << time_elapsed << " seconds" << endl;
    }

    MPI_Finalize();

    return 0;
}