/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

// Constants
#define PI 3.141592653589793238462643383

// Standard C includes
#include <stdint.h>
#include <math.h>

// Standard C++11 includes
#include <tuple>
#include <vector>
#include <map>
#include <unordered_map>
#include <deque>
#include <complex>
#include <bitset>
#include <string>
#include <cassert>
#include <cstring>
#include <iostream>

// Third party includes
#include <fftw3.h>
#include <sndfile.h>

// Project includes
#include "hash.hpp"      // Hashing helper routines
#include "signal.hpp"    // Signal processing routines
#include "soundfile.hpp" // Sound file routines
#include "ofdm.hpp"      // Orthogonal frequency-division multiplexing
