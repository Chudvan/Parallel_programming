#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <string>

using namespace std;

LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM); // функция обработки сообщений окна

const int N = 8; //кол-во процессов
const char* LPSZCLASSNAME = "MyClass";
const char* LPWINDOWNAME = "Глобальный минимум";
const char* F_STRING = "f = sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x";

// Функция создания N процессов с глобальными границами интервала [a;b]
void createProcesses(double a, double b) {
    STARTUPINFO si[N];
    PROCESS_INFORMATION pi[N];

    for (int i = 0; i < N; i++)
    {
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]);
        ZeroMemory(&pi[i], sizeof(pi[i]));
    }

    std::string cmdline;
    LPSTR cmdline_;
    std::string path_to_exe = "F:\\Code\\C++\\ParallelProgramming\\CreateProcess\\SendMessage_PeekMessage\\ChildProcess\\Debug\\\
ChildProcess.exe";
    std::string sep = " ";
    double _a, _b;
    int process_num;

    for (int i = 0; i < N; i++) {
        process_num = i + 1;
        _a = a + (b - a) * i / N;
        _b = a + (b - a) * (i + 1) / N;
        cmdline = path_to_exe + sep + std::to_string(process_num) + sep + \
            std::to_string(_a) + sep + std::to_string(_b);
        cmdline_ = &(cmdline[0]);

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
}

// Функция преобразования строки в число
int StrToInt(char* s)
{
    int temp = 0; // число
    int i = 0;
    int sign = 0; // знак числа 0- положительное, 1 - отрицательное
    if (s[i] == '-')
    {
        sign = 1;
        i++;
    }
    while (s[i] >= 0x30 && s[i] <= 0x39)
    {
        temp = temp + (s[i] & 0x0F);
        temp = temp * 10;
        i++;
    }
    temp = temp / 10;
    if (sign == 1)
        temp = -temp;
    return(temp);
}

//Функция инициализации переменных для вычисления минимума/максимума
void init_n_and_min_max(int* _n, double* min_) {
    *_n = 0;
    *min_ = std::numeric_limits<double>::max();
}

//Функция разбиения char* с разделителем sep в 2 double
void two_doubles_from_string(double* first_d, double* second_d, 
    char* str, char sep) {

    char* c = strchr(str, sep);
    char s1[100], s2[100];

    if (c != 0)
    {
        strcpy_s(s1, str);
        s1[c - str] = 0;
        strcpy_s(s2, c + 1);
    }

    *first_d = atof(s1);
    *second_d = atof(s2);
}

//Функция инициализации главного окна
void InitWindow(MSG* msg, HINSTANCE* hInstance, int nCmdShow) {
    HWND hwnd; // дескриптор окна
    WNDCLASS w; // структура класса окна
    memset(&w, 0, sizeof(WNDCLASS)); // очистка памяти для структуры
    w.style = CS_HREDRAW | CS_VREDRAW;
    w.lpfnWndProc = WndProc;
    w.hInstance = *hInstance;
    w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
    w.lpszClassName = LPSZCLASSNAME;
    RegisterClass(&w); // регистрация класса окна
    // Создание окна
    hwnd = CreateWindow(LPSZCLASSNAME, LPWINDOWNAME,
        WS_OVERLAPPEDWINDOW,
        500, 300, 400, 300,
        NULL, NULL, *hInstance, NULL);
    ShowWindow(hwnd, nCmdShow); // отображение окна
    UpdateWindow(hwnd);         // перерисовка окна
}

// Функция обработки сообщений
LONG WINAPI WndProc(HWND hwnd, UINT Message,
    WPARAM wparam, LPARAM lparam) {
    HDC hdc;
    HINSTANCE hInst;
    PAINTSTRUCT ps;
    static HWND hBtn; // дескриптор кнопки
    static HWND hEdt1, hEdt2; // дескрипторы полей редактирования
    static HWND hStat; // дескриптор статического текста
    TCHAR StrA[20];
    double a, b, Len;
    static int _n;
    char lparam_[100];
    double cur_min, cur_x_min;
    static double min_, x_min;
    const WPARAM CREATE_PROCESSES_WPARAM = 1001;

    switch (Message) {
    case WM_CREATE: // сообщение создания окна
        init_n_and_min_max(&_n, &min_);

        hInst = ((LPCREATESTRUCT)lparam)->hInstance; // дескриптор приложения
        // Создаем и показываем первое поле редактирования
        hEdt1 = CreateWindow("edit", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 50, 50, 60, 20,
            hwnd, 0, hInst, NULL);
        ShowWindow(hEdt1, SW_SHOWNORMAL);
        // Создаем и показываем второе поле редактирования
        hEdt2 = CreateWindow("edit", "0",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 150, 50, 60,
            20, hwnd, 0, hInst, NULL);
        ShowWindow(hEdt2, SW_SHOWNORMAL);
        // Создаем и показываем кнопку
        hBtn = CreateWindow("button", "Рассчитать",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            50, 130, 120, 30, hwnd, 0, hInst, NULL);
        ShowWindow(hBtn, SW_SHOWNORMAL);
        // Создаем и показываем поле текста для результата
        hStat = CreateWindow("static", "0", WS_CHILD | WS_VISIBLE,
            150, 200, 180, 40, hwnd, 0, hInst, NULL);
        ShowWindow(hStat, SW_SHOWNORMAL);
        break;
    case WM_COMMAND:  // сообщение о команде
        if (lparam == (LPARAM)hBtn)    // если нажали на кнопку
        {
            Len = GetWindowText(hEdt1, StrA, 20);
            a = StrToInt(StrA); // считываем число из первого поля
            Len = GetWindowText(hEdt2, StrA, 20);
            b = StrToInt(StrA); // считываем число из второго поля
            SendMessage(hwnd, WM_COMMAND, (WPARAM)CREATE_PROCESSES_WPARAM,
                (LPARAM)((to_string(a) + " " + to_string(b)).c_str()));
        }
        else if (wparam == CREATE_PROCESSES_WPARAM) {
            two_doubles_from_string(&a, &b, (char*)lparam, ' ');
            createProcesses(a, b);
        }
        break;
    case WM_COPYDATA:
        two_doubles_from_string(&cur_min, &cur_x_min, (char*)(((PCOPYDATASTRUCT)lparam)->lpData), ' ');

        if (min_ > cur_min) {
            min_ = cur_min;
            x_min = cur_x_min;
        }
        _n += 1;
        if (_n == N) {
            SendMessage(hwnd, WM_PRINT, 0, 0);
        }
        break;
    case WM_PRINT:
        SetWindowText(hStat, (std::to_string(min_) + "\n" + std::to_string(x_min)).c_str());
        init_n_and_min_max(&_n, &min_);
        break;
    case WM_PAINT: // перерисовка окна
        hdc = BeginPaint(hwnd, &ps); // начало перерисовки
        TextOut(hdc, 50, 20, "Введите границы интервала [a;b]", 32); // вывод текстовых сообщений
        TextOut(hdc, 50, 80, "Инструменты: SendMessage/PeekMessage", 37);
        TextOut(hdc, 50, 100, F_STRING, 48);
        TextOut(hdc, 50, 180, "Результат:", 11);
        TextOut(hdc, 50, 200, "Y", 2);
        TextOut(hdc, 50, 220, "X", 2);
        EndPaint(hwnd, &ps); // конец перерисовки
        break;
    case WM_DESTROY: // закрытие окна
        PostQuitMessage(0);
        break;
    default: // обработка сообщения по умолчанию
        return DefWindowProc(hwnd, Message, wparam, lparam);
    }
    return 0;
}

// Стартовая функция
int  WINAPI  WinMain(HINSTANCE  hInstance,
    HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;   // структура сообщения
    InitWindow(&msg, &hInstance, nCmdShow);

    int n = 0;
    while (true) {
        while (!PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
        {
            continue;
        }
        n += 1;
        if (n == N) break; // Ожидаем пока все процессы завершат работу
    }

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}