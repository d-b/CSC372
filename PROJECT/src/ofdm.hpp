/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

class Bitmap {
    // Types
    typedef uint32_t field_t;

    // Internal fields
    std::vector<std::bitset<sizeof(field_t)*8>> fields;
    uint32_t total;

    // Resize internal bitmap
    void resize(uint32_t bits);

public:
    // Types
    typedef uint32_t bit_t;

    // Constructor & copy constructor
    Bitmap() {}
    Bitmap(const Bitmap& bitmap)
        : fields(bitmap.fields) {};

    // Methods
    void     set(bit_t index);
    void     reset(bit_t index);
    bool     test(bit_t index) const;
    bit_t    ffs(void) const;
    uint32_t count(void) const;

private:
    inline static uint32_t bits(void) {
        return sizeof(field_t)*8;}
    inline uint32_t field(bit_t index) const {
        return index/bits();}
};

class Spectrum {
public:    
    // Types
    typedef Bitmap::bit_t subcarrier_t;
    typedef double scale_t;
    typedef std::unordered_map<subcarrier_t, scale_t> Scalemap;

    // Constructor & copy constructor
    Spectrum() {};
    Spectrum(const Spectrum& spectrum)
        : carriermap(spectrum.carriermap), scalemap(spectrum.scalemap) {};

    // Methods
    void add(subcarrier_t index, double scale);
    void remove(subcarrier_t index);
    bool test(subcarrier_t index) const;
    double scale(subcarrier_t index) const;
    uint32_t components(void) const;
    Bitmap bitmap(void) const;

private:
    Bitmap carriermap;
    Scalemap scalemap;
};

class OFDM {
    // OFDM parameters
    Spectrum spectrum;
    const uint32_t fft_size;
    const uint32_t guard_size;

    // FFTW context
    fftw_plan* plan;
    fftw_complex* buffer;

public:
    // OFDM instance with given spectrum, fft size and guard size
    OFDM(const Spectrum& spectrum, uint32_t fft_size, uint32_t guard_size);

    // Spectrum control
    Spectrum get_spectrum(void) const;
    void     set_spectrum(const Spectrum& spectrum);

    // Send and receive
    void send(const std::vector<std::complex<double>>& frame);
    void receive(const std::vector<std::complex<double>>& frame);
};
