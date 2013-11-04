/*
 * CSC372 - RTOS
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "user.h"

/* The Cooley-Tukey multiple column algorithm, see page 124 of Charles
   Van Loan x[] is input data, overwritten by output, viewed as n/n2 by n2
   array. flag = 1 for forward and -1 for backward transform.
*/
void cooley_tukey(complex_t* x, int n, int flag, int n2)
{
   complex_t c, w, t;
   int i, j, k, m, p, n1;
   int Ls, ks, ms, jm, dk;

   n1 = n/n2;                               /* do bit reversal permutation */
   for(k = 0; k < n1; ++k) {        /* This is algorithms 1.5.1 and 1.5.2. */
      j = 0; 
      m = k;
      p = 1;                               /* p = 2^q,  q used in the book */
      while(p < n1) {
         j = 2*j + (m&1);
         m >>= 1;
         p <<= 1;
      }
      ASSERT(p == n1);                   /* make sure n1 is a power of two */
      if(j > k) {
         for(i = 0; i < n2; ++i) {                     /* swap k <-> j row */
            c = x[k*n2+i];                              /* for all columns */
            x[k*n2+i] = x[j*n2+i];
            x[j*n2+i] = c;
         }
      }
   }
                                              /* This is (3.1.7), page 124 */
   p = 1;
   while(p < n1) {
      Ls = p;
      p <<= 1;
      jm = 0;                                                /* jm is j*n2 */
      dk = p*n2;
      for(j = 0; j < Ls; ++j) {
         w = complex_exp(complex_new(0, PI*j/Ls));
         w.imag *= -flag;
         for(k = jm; k < n; k += dk) {                      /* "butterfly" */
            ks = k + Ls*n2;
            for(i = 0; i < n2; ++i) {                      /* for each row */
               m = k + i;
               ms = ks + i;
               t = complex_mul(w, x[ms]);
               x[ms] = complex_sub(x[m], t);
               x[m] = complex_add(x[m], t);
            }
         }
         jm += n2;
      } 
   }
}

/*
 * audio_fft
 *
 * Compute the spectrum of the input signal via fast Fourier transform
 */
int audio_fft(const complex_t* signal, complex_t* spectrum, size_t samples) {
    if(signal != spectrum)
        memcpy(spectrum, signal, sizeof(complex_t) * samples);
    cooley_tukey(spectrum, samples, 1, 1);
    return 0;
}

/*
 * audio_ifft
 *
 * Produce samples of the signal represented in the input spectrum data via an inverse fast Fourier transform
 */
int audio_ifft(const complex_t* spectrum, complex_t* signal, size_t samples) {
    if(spectrum != signal)
        memcpy(signal, spectrum, sizeof(complex_t) * samples);
    cooley_tukey(signal, samples, -1, 1);
    int i; for(i = 0; i < samples; i++) {
        signal[i].real /= samples;
        signal[i].imag /= samples;
    } return 0;
}
