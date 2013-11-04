/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#ifndef _FFT_H_
#define _FFT_H_

#include <math.h>

// Complex number
typedef struct {
    double real;
    double imag;
} complex_t;

//
// Complex number functions
//
inline complex_t complex_conj(complex_t z) {
    z.imag = -z.imag;
    return z;
}
inline complex_t complex_add(complex_t x, complex_t y) {
    complex_t result = {
        x.real + y.real,
        x.imag + y.imag
    }; return result;
}
inline complex_t complex_sub(complex_t x, complex_t y) {
    complex_t result = {
        x.real - y.real,
        x.imag - y.imag
    }; return result;
}
inline complex_t complex_mul(complex_t x, complex_t y) {
    complex_t result = {
        y.real*x.real - y.imag*x.imag,
        y.real*x.imag + y.imag*x.real
    }; return result;
}
inline complex_t complex_exp(complex_t z) {
    complex_t result;
    double r = exp(z.real);
    result.real = r*cos(z.imag);
    result.imag = r*sin(z.imag);
    return result;
}
inline double complex_arg(complex_t z) {
    return atan2(z.imag, z.real);
}
inline double complex_abs(complex_t z) {
    return sqrt(z.real*z.real + z.imag*z.imag);
}
inline complex_t complex_new(double real, double imag) {
    complex_t result;
    result.real = real;
    result.imag = imag;
    return result;
}

int audio_fft(const complex_t* signal, complex_t* spectrum, size_t samples);
int audio_ifft(const complex_t* spectrum, complex_t* signal, size_t samples);

#endif
