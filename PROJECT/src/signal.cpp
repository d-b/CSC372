/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem
{
    //
    // FFT
    //

    std::unordered_map<std::tuple<uint16_t, bool>, fft::plan> fft::plans;

    void fft::init(void) {}

    void fft::prepare(uint16_t points, bool forwards) {
        std::tuple<uint16_t, bool> params(points, forwards);
        auto iter = plans.find(params);
        if(iter == plans.end()) {
            fft::plan plan = {
                points,
                forwards,
                (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * points),
                (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * points),
                NULL
            }; plan.plan = fftw_plan_dft_1d(
                points, plan.input, plan.output, forwards ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_MEASURE);
            plans[params] = plan;
        }
    }

    void fft::forwards(const channel& input, channel& output, uint16_t points) {
        prepare(points, true);
        plan &p = plans[std::tuple<uint16_t, bool>(points, true)];
        memcpy(p.input, input.data(), sizeof(fftw_complex)*points);
        fftw_execute(p.plan);
        memcpy(output.data(), p.output, sizeof(fftw_complex)*points);
    }
    
    void fft::inverse(const channel& input, channel& output, uint16_t points) {
        prepare(points, false);
        plan &p = plans[std::tuple<uint16_t, bool>(points, false)];
        memcpy(p.input, input.data(), sizeof(fftw_complex)*points);
        fftw_execute(p.plan);
        memcpy(output.data(), p.output, sizeof(fftw_complex)*points);        
    }

    void fft::exit() {
        // TODO: Add cleanup code
    }

    //
    // Signal
    //

    signal::signal(const signal& sig)
        : channels(sig.channels), rate(sig.rate), data(sig.data)
    {};

    signal& signal::operator=(const signal& other) {
        assert(channels == other.channels);
        assert(rate == other.rate);
        data = other.data;
    }

    signal::signal(uint16_t channels, uint32_t rate)
        : channels(channels), rate(rate)
    {
        assert(channels > 0);
        data.resize(channels);
    }

    spectrum signal::fft(uint16_t points) const {
        return spectrum(*this, points);
    }

    void signal::lowpass(double cutoff) {
        // Spectrum parameters
        uint16_t size    = samples();
        uint16_t n       = (size - 1)/2;
        uint16_t nyquist = rate/2;
        double   width   = (double)nyquist/n;

        // Compute spectrum
        // TODO: windowed application over entire signal
        spectrum spec(*this, size);

        for(int i = 0; i < channels; i++) {
            // Kill bins above cutoff
            uint16_t start = (int)ceil(cutoff/width);
            for(int j = start; j <= n; j++) {
                spec[i][j]        = 0;
                spec[i][size - j] = 0;
            }

            // Kill Nyquist bin as necessary
            if((size - 1) % 2) spec[i][n + 1] = 0;
        }

        *this = spec;
    }

    void signal::upconvert(double frequency) {
        // Choose nearest frequency bin
        uint16_t size    = samples();
        uint16_t n       = size/2;
        uint16_t nyquist = rate/2;
        double   width   = (double)nyquist/n;
        frequency        = width * ceil(frequency/width);
        // Perform frequency shift and return real value result
        double period = (nyquist/frequency)*2.0;
        for(int i = 0; i < channels; i++) {
            for(int j = 0; j < size; j++) {
                data[i][j] *= exp(std::complex<double>(0, 2*PI*(j/period)));
                data[i][j]  = std::real(data[i][j])*2.0;
            }
        }
    }

    void signal::downconvert(double frequency, double bandwidth) {
        // Choose nearest frequency bin
        uint16_t size    = samples();
        uint16_t n       = size/2;
        uint16_t nyquist = rate/2;
        double   width   = (double)nyquist/n;
        frequency        = width * ceil(frequency/width);
        // Perform frequency shift and return real value result
        double period = (nyquist/frequency)*2.0;
        for(int i = 0; i < channels; i++)
            for(int j = 0; j < size; j++)
                data[i][j] *= exp(std::complex<double>(0, -2*PI*(j/period)));
        lowpass(bandwidth/2);
    }

    size_t signal::samples() const {
        return data[0].size();
    }

    channel& signal::operator[] (uint16_t channel) {
        return data[channel];
    }
    const channel& signal::operator[] (uint16_t channel) const {
        return data[channel];
    }

    signal& signal::operator*=(std::complex<double> value) {
        for(int i = 0; i < channels; i++)
            for(int j = 0; j < samples(); j++)
                data[i][j] *= value;
        return *this;
    }
    
    signal& signal::operator/=(std::complex<double> value) {
        for(int i = 0; i < channels; i++)
            for(int j = 0; j < samples(); j++)
                data[i][j] /= value;
        return *this;
    }

    signal signal::operator+(const signal& other) {
        // Sanity checks on other signal
        assert(rate == other.rate);
        assert(channels == other.channels);
        // Compute resulting signal
        signal result(*this);
        for(int i = 0; i < result.channels; i++)
            result[i].insert(result[i].end(), other[i].begin(), other[i].end());
        return result;
    }

    signal signal::operator*(uint32_t repetitions) {
        signal result(channels, rate);
        for(int i = 0; i < channels; i++) {
            // Build signal of repetitions
            size_t size = result[i].size();
            result[i].resize(size * repetitions);
            for(int j = 0; j < repetitions; j++)
                result[i].insert(result[i].end(), (*this)[i].begin(), (*this)[i].end());
        } return result;
    }

    //
    // Window
    //

    // TODO: implement window

    //
    // Spectrum
    //

    spectrum::spectrum(const spectrum& spec)
        : channels(spec.channels), rate(spec.rate), componenets(spec.componenets), data(spec.data)
    {};
    
    spectrum::spectrum(const signal& sig, uint16_t componenets)
        : channels(sig.channels), rate(sig.rate), componenets(componenets)
    {
        // Perform forwards fast-fourier-transform
        data.resize(channels);
        for(int i = 0; i < channels; i++) {
            data[i].resize(componenets);
            fft::forwards(sig[i], data[i], componenets);
        }
    }
    
    spectrum::spectrum(uint16_t channels, uint32_t rate, uint16_t componenets)
        : channels(channels), rate(rate), componenets(componenets)
    {
        data.resize(channels);
        for(int i = 0; i < channels; i++)
            data[i].resize(componenets);
    }

    spectrum::operator signal(void) const {
        return ifft();
    }
    
    signal spectrum::ifft(void) const {
        // Perform inverse fast-fourier-transform
        signal sig(channels, rate);
        for(int i = 0; i < channels; i++) {
            sig[i].resize(componenets);
            fft::inverse(data[i], sig[i], componenets);
        }
        // Normalize and return the synthesized signal
        sig /= componenets;
        return sig;
    }

    size_t spectrum::samples() const {
        return componenets;
    }

    channel& spectrum::operator[] (uint16_t channel) {
        return data[channel];
    }
    const channel& spectrum::operator[] (uint16_t channel) const {
        return data[channel];
    }

    spectrum spectrum::conjugate(void) const {
        spectrum result(*this);
        std::for_each(result.data.begin(), result.data.end(), [](channel& chan){
            std::transform(chan.begin(), chan.end(), chan.begin(),
                [](std::complex<double> sample){
                    return conj(sample);
            });
        });
    }

    std::complex<double> spectrum::dot(const spectrum& other) const {
        assert(channels == other.channels);
        assert(samples() == other.samples());
        std::complex<double> result = 0.0;
        for(int i = 0; i < channels; i++)
            for(int j = 0; j < samples(); j++)
                result += data[i][j] * conj(other[i][j]);
        return result;
    }

    std::complex<double> spectrum::operator^(const spectrum& other) const {
        return this->dot(other);
    }

    spectrum& spectrum::operator*=(std::complex<double> value) {
        std::for_each(data.begin(), data.end(), [&value](channel& chan){
            std::transform(chan.begin(), chan.end(), chan.begin(),
                [&value](std::complex<double> sample){
                    return sample * value;
            });
        });
    }
    
    spectrum& spectrum::operator/=(std::complex<double> value) {
        std::for_each(data.begin(), data.end(), [&value](channel& chan){
            std::transform(chan.begin(), chan.end(), chan.begin(),
                [&value](std::complex<double> sample){
                    return sample / value;
            });
        });
    }

    spectrum& spectrum::operator*=(const spectrum& other) {
        assert(channels == other.channels);
        assert(samples() == other.samples());
        for(int i = 0; i < channels; i++)
            for(int j = 0; j < samples(); j++)
                data[i][j] *= other[i][j];
        return *this;
    }

    double spectrum::correlation(const spectrum& other) const {
        return 0;
    }    
}
