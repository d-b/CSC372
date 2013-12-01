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
#include <exception>
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
#include <sstream>
#include <algorithm>

// Third party includes
#include <fftw3.h>
#include <sndfile.h>
#include <alsa/asoundlib.h>

// Project includes
#include "exception.hpp"       // Global exception type
#include "hash.hpp"            // Hashing helper routines
#include "signal.hpp"          // Signal processing routines
#include "soundfile.hpp"       // Sound file routines
#include "ofdm.hpp"            // Orthogonal frequency-division multiplexing

// OFDM modules
#include "medium_alsa.hpp"     // ALSA communications medium
#include "modulator_naive.hpp" // Naive subcarrier modulation
#include "modulator_qam.hpp"   // Quadrature amplitude modulation based subcarrier modulation
#include "stream_text.hpp"     // Text streaming
