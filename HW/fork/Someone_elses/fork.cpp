#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <cfloat>
#include <fstream>

using namespace std;


int main() {
    int N = 4;
    pid_t* pid = new pid_t[N];
    double begin = 0;
    double end;
    double end_g = 100;
    
    for(int i = 0; i < N; i++)
    {
        end = begin + end_g / N;

        pid[i]= fork();

        if (pid[i] == 0)                // child
        {
            char id[5], b[50], e[50];
            sprintf(id, "%d", i+1);
            sprintf(b, "%f", begin);
            sprintf(e, "%f", end);
            execl("/home/ivan/CPPProjects/HW/Someone_elses/new_process_fork", "new_process_fork", id, b, e, (char *) 0);
            // cout << id << " has finished" << endl;
            // exit(0);
        }
        else if (pid[i] < 0)    // failed to fork
        {
            cerr << "Failed to fork" << endl;
            exit(1);
        }
        
        begin += end_g / N;
    }

    //cout << "WUNTRACED: " << WUNTRACED << endl;
    for (int i = 0; i < N; ++i) {
        int status;
        //while (-1 == waitpid(pid[i], &status, 0));
        waitpid(pid[i], &status, 0);
        cout << "waitpid " << i << " has been ended." << endl;
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            cerr << "Process " << i << " (pid " << pid[i] << ") failed" << endl;
            exit(1);
        }
    }
    
    std::string line;
    double min = DBL_MAX;
    for (int i = 0; i < N; i++)
    {
        char firstPart[50] = "/home/ivan/CPPProjects/HW/Someone_elses/Files/";
        char lastPart[10] = ".txt";
        char fileName[90];
        sprintf(fileName, "%s%d%s", firstPart, i+1, lastPart);
        double cur_min;
        std::ifstream in(fileName); // окрываем файл для чтения
        if (in.is_open())
        {
            getline(in, line);
//            cout << line << endl;
            cur_min = atof(line.c_str());
            if (min > cur_min)
                min = cur_min;
        }
        in.close();     // закрываем файл
    }

    cout << "fork + execl + files" << endl;
    cout << "f = sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x" << endl;
    cout << "min(f) = " << min << endl;

    return EXIT_SUCCESS;
}
