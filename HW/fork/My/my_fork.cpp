#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <limits>
#include <fstream>

int main(){
    const int N = 5;
    pid_t pid[N];

    std::string path_to_bin_dir = "/home/ivan/CPPProjects/HW/fork/My/";
    std::string bin_name = "my_child_fork";
    double end = 100;
    double a, b;
    int process_num;
    for (int i = 0; i < N; i++) {
        pid[i] = fork();
        if (pid[i] == 0){ //child
            process_num = i + 1;
            a = end * i / N;
            b = end * (i + 1) / N;
            execl((path_to_bin_dir + bin_name).c_str(),
                bin_name.c_str(), 
                std::to_string(process_num).c_str(), 
                std::to_string(a).c_str(),
                std::to_string(b).c_str(),
                NULL);
            std::cout << "process " << process_num << " has finished" << std::endl;
            exit(0);
        }
        else if (pid[i] < 0){ //error
            perror((std::string("fork ") + std::to_string(i + 1) + std::string(" failed")).c_str());
            exit(1);
        }
    }

    for (int i = 0; i < N; i++) {
        int status;
        waitpid(pid[i], &status, 0);
        std::cout << "waitpid " << i + 1 << " has been ended" << std::endl;
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0){ //error
            perror((std::string("fork ") + std::to_string(i + 1) + std::string(" failed")).c_str()); 
            exit(1);
        }
    }

    double min_y = std::numeric_limits<double>::max();
    double min_x;
    double cur_min_y, cur_min_x;
    std::string cur_min_y_s, cur_min_x_s;
    std::string path_to_dir = "/home/ivan/CPPProjects/HW/fork/My/Files/";
    std::string ext = ".txt";
    std::ifstream in;
    for (int i = 0; i < N; i++) {
        //std::cout << "here " << std::endl;
        in.open(path_to_dir + std::to_string(i + 1) + ext);
        getline(in, cur_min_y_s, ' ');
        getline(in, cur_min_x_s, '\n');
        cur_min_y = std::stod(cur_min_y_s);
        if (cur_min_y < min_y) {
            min_y = cur_min_y;
            min_x = std::stod(cur_min_x_s);
        }
        in.close();
    }
    std::cout << std::endl << min_y << " " << min_x << std::endl;
}