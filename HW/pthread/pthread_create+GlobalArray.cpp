#include <iostream>
#include <math.h>
#include <pthread.h>

using namespace std;

double f(double x)
{
    return sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x;
}

struct pth_params
{
    int id;
    double begin;
    double end;
};

const int N = 4;
pthread_t pthreads[N];
pth_params params[N];
double results[N];
double results_x[N];

void* pthread_f(void *args)
{
    pth_params *prms = (pth_params*)args;
    double x = prms->begin;
    double cur_min = f(x);
    double min_x = x;
    x += 0.0001;

    while(x < prms->end)
    {
        double cur_f = f(x);

        if (cur_f < cur_min){
            cur_min = cur_f;
            min_x = x;
        }

        x += 0.0001;
    }

    results[prms->id] = cur_min;
    results_x[prms->id] = min_x;
    return 0;
}

int main()
{
    double begin = 0;
    double end = 100;

    for  (int i = 0; i < N; i++)
    {
        params[i].begin = begin;
        params[i].end = begin + end/N;
        params[i].id = i;

        int ret = pthread_create(&pthreads[i], NULL, pthread_f, &params[i]);
        if (ret == 0 )
            cout << i << " поток запущен успешно" << endl;
        else
            cout << "Запуск " << i << " потока не удался" << endl;

        begin += end/N;
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(pthreads[i], NULL);
    }

    double cur_min = results[0];
    double min_x;
    for  (int i = 1; i < N; i++)
        if (results[i] < cur_min){
            cur_min = results[i];
            min_x = results_x[i];
        }

    cout << "pthread_create + global array" << endl;
    cout << "f = sin(5 * pow(x, 3)) + cos(7 * pow(x, 4)) * x" << endl;
    cout << "min(f) = " << cur_min << " min_x " << min_x << endl;
    return 0;
}

