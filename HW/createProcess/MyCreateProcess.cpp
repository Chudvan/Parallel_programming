#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <fstream>
#include <limits>
#include <string>
#include <iostream>
#include <codecvt>

void _tmain(int argc, TCHAR* argv[])
{
    const int N = 5;
    STARTUPINFO si[N];
    PROCESS_INFORMATION pi[N];

    for (int i = 0; i < N; i++) {
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]);
        ZeroMemory(&pi[i], sizeof(pi[i]));
    }

    std::wstring cmdline;
    LPWSTR cmdline_;
    std::wstring path_to_exe = L"F:\\Code\\C++\\ParallelProgramming\\CreateProcess\\\MyChildProcess\\Debug\\\
MyChildProcess.exe";
    std::wstring sep = L" ";
    double end = 100;
    double a, b;
    int process_num;
    for (int i = 0; i < N; i++) {
        process_num = i + 1;
        a = end * i / N;
        b = end * (i + 1) / N;
        cmdline = path_to_exe + sep + std::to_wstring(process_num) + sep + \
            std::to_wstring(a) + sep + std::to_wstring(b);
        cmdline_ = &(cmdline[0]);

        //std::wcout << cmdline_ << std::endl;

        if (!CreateProcess(NULL,
            cmdline_,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si[i],
            &pi[i])
            )
        {
            printf("CreateProcess failed (%d).\n", GetLastError());
            return;
        }
    }

    for (int i = 0; i < N; i++) {
        WaitForSingleObject(pi[i].hProcess, INFINITE);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    double min_y = std::numeric_limits<double>::max();
    double min_x;
    double cur_min_y, cur_min_x;
    std::string cur_min_y_s, cur_min_x_s;
    std::string path_to_dir = "F:\\Code\\C++\\ParallelProgramming\\Files\\";
    std::string ext = ".txt";
    std::ifstream in;
    for (int i = 0; i < N; i++) {
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
