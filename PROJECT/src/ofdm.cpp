/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

//
// Bitmap implementation
//

void Bitmap::resize(uint32_t bits) {
    bit_t bit = bits - 1;
    uint32_t i = field(bit);
    if(i >= fields.size())
        fields.resize(i + 1);
}

void Bitmap::set(Bitmap::bit_t index) {
    resize(index + 1);
    uint32_t i = field(index);
    uint32_t j = index % bits();
    if(!fields[i].test(j)) {
        fields[i].set(j); total += 1;
    }
}

void Bitmap::reset(Bitmap::bit_t index) {
    resize(index + 1);
    uint32_t i = field(index);
    uint32_t j = index % bits();
    if(fields[i].test(j)) {
        fields[i].reset(j); total -= 1;
    }
}

bool Bitmap::test(Bitmap::bit_t index) const {
    uint32_t i = field(index);
    if(i >= fields.size()) return false;
    uint32_t j = index % bits();
    return fields[i].test(j);
}

Bitmap::bit_t Bitmap::ffs(void) const {
    return 0;
}

uint32_t Bitmap::count(void) const {
    return total;
}

//
// Spectrum implementation
//

void Spectrum::add(subcarrier_t index, double scale) {
    carriermap.set(index);
    scalemap[index] = scale;
}

void Spectrum::remove(subcarrier_t index) {
    carriermap.reset(index);
    auto iter = scalemap.find(index);
    if(iter != scalemap.end()) scalemap.erase(iter);
}

bool Spectrum::test(subcarrier_t index) const {
    return carriermap.test(index);
}

double Spectrum::scale(subcarrier_t index) const {
    if(!carriermap.test(index)) return 0;
    auto iter = scalemap.find(index);
    return (iter != scalemap.end()) ? iter->second : 0;
}

uint32_t Spectrum::components(void) const {
    return carriermap.count();
}

Bitmap Spectrum::bitmap(void) const {
    return carriermap;
}

//
// OFDM
//

OFDM::OFDM(const Spectrum& spectrum, uint32_t fft_size, uint32_t guard_size)
    : spectrum(spectrum), fft_size(fft_size), guard_size(guard_size)
{

}

Spectrum OFDM::get_spectrum(void) const {

}

void OFDM::set_spectrum(const Spectrum& spectrum) {

}

void OFDM::send(const std::vector<std::complex<double>>& frame) {

}

void OFDM::receive(const std::vector<std::complex<double>>& frame) {

}
