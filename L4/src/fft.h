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
complex_t complex_conj(complex_t z);
complex_t complex_add(complex_t x, complex_t y);
complex_t complex_sub(complex_t x, complex_t y);
complex_t complex_mul(complex_t x, complex_t y);
complex_t complex_exp(complex_t z);
double complex_arg(complex_t z);
double complex_abs(complex_t z);
complex_t complex_new(double real, double imag);

int audio_fft(const complex_t* signal, complex_t* spectrum, size_t samples);
int audio_ifft(const complex_t* spectrum, complex_t* signal, size_t samples);

#endif
