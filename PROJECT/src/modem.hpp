/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

// Platform includes
#include <stdint.h>
#include <complex>
#include <vector>
#include <map>
#include <unordered_map>
#include <bitset>

// FFTW
#include <fftw3.h>

// Project includes
#include "ofdm.hpp"    // Orthogonal frequency-division multiplexing
#include "encoder.hpp" // OFDM based audio encoder
#include "decoder.hpp" // OFDM based audio decoder
