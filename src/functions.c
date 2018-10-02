#include "functions.h"

#include <math.h>
#include <stddef.h>

double _phi;
double _samplerate;

double _x_n_1, _x_n_2, _y_n_1, _y_n_2;

void init_functions(double samplerate) {
    _phi = 0.0;
    _samplerate = samplerate;
    _x_n_1 = 0.0;
    _x_n_2 = 0.0;
    _y_n_1 = 0.0;
    _y_n_2 = 0.0;
}

/* unit conversion functions */

double dB(double n) {
    return pow(10.0, n / 20.0);
}

double deg(double n) {
    return n * M_PI / 180.0;
}

/* pre-oscillator functions */

double phi(double delta) {
    const double temp0 = _phi;
    _phi = fmod(_phi + delta * M_TAU / _samplerate, M_TAU);
    return temp0;
}

double symm(double t, double n) {
    const double u = fmod(t, M_TAU);
    double temp0 = u - M_PI;
    if (fabs(temp0) < (1.0 - n) * M_PI_2) {
        temp0 /= 1.0 - n;
    } else if (temp0 > 0.0) {
        temp0 = M_PI - (M_PI - temp0) / (n + 1.0);
    } else if (temp0 < 0.0) {
        temp0 = M_PI + (M_PI + temp0) / (n + 1.0);
    }
    temp0 += M_PI;
    return t - u + temp0;
}

double sync(double x1, double x2) {
    return x1 - fmod(x1, M_TAU) + fmod(x2, M_TAU * x2 / x1);
}

/* oscillator functions */

double circ(double x) {
    const double u = fmod(x, M_TAU);
    const double temp0 = M_PI_2 - ((u < M_PI) ? u : (u - M_PI));
    const double temp1 = sqrt(M_PI_2_SQ - temp0 * temp0) / M_PI_2;
    return (u < M_PI) ? temp1 : -temp1;
}

double exptl(double x) {
    const double u = fmod(x, M_TAU);
    static const double roundness = 0.00001;
    return roundness * exp(((u < M_PI) ? u : (M_TAU - u)) * log(1.0 / roundness) / M_PI) * 2.0 - 1.0;
}

double moog(double x) {
    const double u = fmod(x + M_3_PI_2, M_TAU);
    return (u < M_PI) ? -(u / M_PI) : ((u - M_PI) * 2.0 / M_PI - 1.0);
}

double noise(double x) {
    /* this is the largest ten digit prime number
       there might be a more suitable number for this purpose */
    static const double prime = 9999999967.0;
    static const double half = 4999999983.5;
    return fmod((sin(fmod(x, M_TAU) * prime) + 1.0) * half, 2.0) - 1.0;
}

double parab(double x, double n) {
    const double u = fmod(x, M_TAU);
    const double temp0 = 3.0 - 2.0 * (u < M_PI);
    const double temp1 = fabs(pow(fabs(u / M_PI_2 - temp0), n));
    return (u < M_PI) ? 1.0 - temp1 : (temp1 - 1.0);
}

double pulse(double x, double n) {
    return n + 2.0 * (fmod(x, M_TAU) < (1.0 - n) * M_PI) - 1.0;
}

double sawf(double x) {
    return -(fmod(x + M_PI, M_TAU) / M_PI - 1.0);
}

double sawr(double x) {
    return fmod(x + M_PI, M_TAU) / M_PI - 1.0;
}

double squ(double x) {
    return 2.0 * (fmod(x, M_TAU) < M_PI) - 1.0;
}

double tri(double x) {
    return fabs(fmod(x + M_3_PI_2, M_TAU) / M_PI_2 - 2.0) - 1.0;
}

/* envelopes */

double adsr(double t, double t_on, double t_off, double a, double d, double s, double r) {
    if (t < t_on)
        return 0.0;
    if (t < t_on + a)
        return (t - t_on) / a;
    if (t < t_on + a + d)
        return s + (1.0 - s) * (1.0 - (t - t_on - a) / d);
    if (t < t_off)
        return s;
    if (t < t_off + r)
        return s * (1.0 - (t - t_off) / r);
    return 0.0;
}

double sfs(double t, double t_on, double t_off, double a, double d, double s, double r) {
    const double t_a = 10.0 * pow(a / 80.0, 8.0);
    if (t < t_on)
        return 0.0;
    if (t < t_on + t_a)
        return (t - t_on) / t_a;
    if (t < t_off)
        return exp(-pow(10.0, 3.0 - d / 20.0) * (t - t_on - t_a)) * (1.0 - s) + s;
    return exp(-pow(10.0, 3.0 - r / 20.0) * (t - t_off)) * (exp(-pow(10.0, 3.0 - d / 20.0) * t_off) * (1.0 - s) + s);
}

double dahdsr(double t, double t_on, double t_off, double dl, double a, double h, double dc, double s, double r) {
    if (t < t_on + dl)
        return 0.0;
    if (t < t_on + dl + a)
        return (t - t_on - dl) / a;
    if (t < t_on + dl + a + h)
        return 1.0;
    if (t < t_on + dl + a + h + dc)
        return s + (1.0 - s) * (1.0 - (t - t_on - dl - a - h) / dc);
    if (t < t_off)
        return s;
    if (t < t_off + r)
        return s * (1.0 - (t - t_off) / r);
    return 0.0;
}

double fade(double t, size_t n_count, double t_arr[], double y_arr[]) {
    if (t < t_arr[0])
        return y_arr[0];
    for (size_t i = 1; i < n_count; i++) {
        if (t < t_arr[i])
            return y_arr[i - 1] + (y_arr[i] - y_arr[i - 1]) * (t - t_arr[i - 1]) / (t_arr[i] - t_arr[i - 1]);
    }
    return y_arr[n_count - 1];
}

/* filter */

/* see: https://en.wikipedia.org/wiki/Digital_biquad_filter#Direct_form_1 */
double filter2(double x_n, double b0, double b1, double b2, double a1, double a2) {
    const double y_n = b0 * x_n + b1 * _x_n_1 + b2 * _x_n_2 - a1 * _y_n_1 - a2 * _y_n_2;
    _x_n_2 = _x_n_1;
    _x_n_1 = x_n;
    _y_n_2 = _y_n_1;
    _y_n_1 = y_n;
    return y_n;
}

double filterlp2(double x_n, double fc, double bw) {
    const double omega = M_TAU * fc / _samplerate;
    const double so = sin(omega);
    const double co = cos(omega);
    const double alpha = so * sinh(M_LN2 * bw * omega / so / 2.0);
    const double b0 = (1.0 - co) / 2.0;
    const double b1 = 1.0 - co;
    const double b2 = (1.0 - co) / 2.0;
    const double a0 = 1.0 + alpha;
    const double a1 = -2.0 * co;
    const double a2 = 1.0 - alpha;
    return filter2(x_n, b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0);
}

/* arpeggiators */

double arp(double t, double t2, size_t n_count, double n_arr[]) {
    const double u = fmod(t, n_count * t2);
    for (size_t i = 0; i < n_count - 1; i++) {
        if (u < (i + 1) * t2)
            return n_arr[i];
    }
    return n_arr[n_count - 1];
}

/* math functions */

double max(size_t n_count, double n_arr[]) {
    double result = n_arr[0];
    for (size_t i = 1; i < n_count; i++) {
        if (n_arr[i] > result)
            result = n_arr[i];
    }
    return result;
}

double min(size_t n_count, double n_arr[]) {
    double result = n_arr[0];
    for (size_t i = 1; i < n_count; i++) {
        if (n_arr[i] < result)
            result = n_arr[i];
    }
    return result;
}

/* see: https://stackoverflow.com/a/1903975 */
double sgn(double n) {
    return (n > 0.0) - (n < 0.0);
}
