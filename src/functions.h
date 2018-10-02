#ifndef FUNCTIONS_HEADER_FILE
#define FUNCTIONS_HEADER_FILE

#include <stddef.h>

#define MAX_FUNCTION_ARGS 20

#define M_E       2.71828182845904523536
#define M_H       1.05946309435929526456
#define M_LN2     0.69314718055994530942
#define M_PI_2    1.57079632679489661923
#define M_PI_2_SQ 2.46740110027233965471
#define M_PI      3.14159265358979323846
#define M_3_PI_2  4.71238898038468985769
#define M_TAU     6.28318530717958647693

void init_functions(double samplerate);

/* math functions */

double max(size_t n_count, double n_arr[]);

double min(size_t n_count, double n_arr[]);

double sgn(double n);

/* unit conversion functions */

double dB(double n);

double deg(double n);

/* pre-oscillator functions */

double phi(double delta);

double symm(double t, double n);

double sync(double x1, double x2);

/* oscillator functions */

double circ(double x);

double exptl(double x);

double moog(double x);

double noise(double x);

double parab(double x, double n);

double pulse(double x, double n);

double sawf(double x);

double sawr(double x);

double squ(double x);

double tri(double x);

/* envelopes */

double adsr(double t, double t_on, double t_off, double a, double d, double s, double r);

double sfs(double t, double t_on, double t_off, double a, double d, double s, double r);

double dahdsr(double t, double t_on, double t_off, double dl, double a, double h, double dc, double s, double r);

double fade(double t, size_t n_count, double t_arr[], double y_arr[]);

/* filter */

double filter2(double x_n, double b0, double b1, double b2, double a1, double a2);

double filterlp2(double x_n, double fc, double bw);

/* arpeggiators */

double arp(double t, double t2, size_t n_count, double n_arr[]);

#endif
