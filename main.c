#include <stdio.h>
#include <sys/time.h>
#define down_lim_int -5.0
#define up_lim_int  5.0
#define sig_max 5.0
#define sig_min -5.0

struct timeval st, et;                                              //definicja struktury czasowej z bilbioteki sys/time.h
double current_signal = 0;
double error = 0.0;

double signal(double Kp, double Ki, double Kd, double setting, double time_delta) //funkcja wyliczająca wartość sygnału sterującego
{
    static double integral = 0;                                     //deklaracja całki
    static double prevError = 0;                                    //potrzebny do sprawdzenia jak szybko zmienia się błąd
    error = (setting - current_signal);                             //wyliczenie aktualnego błędu
    integral = integral + Ki * time_delta * (error + prevError);    //wyliczenie wartości członu całkującego
    if (integral > up_lim_int)                                      //limity chroniące przed przeregulowaniem
    {
        integral = up_lim_int;
    }
    else if (integral < down_lim_int)
    {
        integral = down_lim_int;
    }
    double derivative = (error - prevError) / time_delta;           //wyliczenie członu różnicującego
    double output = (Kp * error) + (Ki * integral) + (Kd * derivative); //wyliczenie sygnału sterującego
    prevError = error;                                              //uaktualnienie błędu po wykonanych obliczeniach
    if (output > sig_max)                                           //kontrola, czy sygnał znajduje się w podanym zakresie
    {
        output = sig_max;
    } else if (output < sig_min)
    {
        output = sig_min;
    }
    return output;                                                  //wyjście
}

void simulate(double Kp, double Ki, double Kd, double setting, double time_delta)
{
    double reg_time = 1000;                                     //symulacyjny czas regulacji
    for (int i = 0; i <= reg_time; i++)                         //pętla w której symulujemy działanie programu
    {
        if (current_signal == setting) break;                   //sprawdzenie czy uzyskana została zadana wartość przed czasem
        double output = signal(Kp, Ki, Kd, setting, time_delta);//wyliczenie sygnału sterującego
        double input = output / 10.0;                           //skalowanie wyjścia aby jego zakres lepiej pasował do regulowanego systemu
        current_signal += input * time_delta;                   //wyliczenie wartości sygnału
        //printf("time: %.2f, current signal: %.2f\n", i * time_delta,current_signal);
    }
}

int main() {
    double Kp = 95.238;
    double Ki = 1 / 0.125723;;
    double Kd = 0.0493875;
    double setting = 10.0;
    double time_delta = 0.1;

    gettimeofday(&st, NULL);
    signal(Kp, Ki, Kd, setting, time_delta);
    simulate(Kp, Ki, Kd, setting, time_delta);
    gettimeofday(&et, NULL);

    int time_passed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
    printf("Regulation time: %d microseconds\n", time_passed);
    printf("Current signal: %.2f, error: %.2f", current_signal, error);
    return 0;
}
