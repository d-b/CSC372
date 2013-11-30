/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{
    // Forward declarations
    class signal;
    class spectrum;
    class window;

    // Signal processing types
    typedef std::vector<std::complex<double>> channel;


    class fft {
        struct plan {
            uint16_t      points;
            bool          forwards;
            fftw_complex* input;
            fftw_complex* output;
            fftw_plan     plan;
        };

        static std::unordered_map<std::tuple<uint16_t, bool>, plan> plans;
        static plan& prepare(uint16_t points, bool forwards);

    public:
        // Static initialization and shutdown
        static void init(void);
        static void exit(void);

        // Perform transform
        static void forwards(const channel& input, channel& output, uint16_t points);
        static void inverse (const channel& input, channel& output, uint16_t points);
    };

    class signal {
        std::vector<channel> data;

    public:
        // Signal parameters
        const uint16_t channels;
        const uint32_t rate;

        // Construct a signal
        signal(const signal& sig);
        signal(uint16_t channels, uint32_t rate);
        signal& operator=(const signal&);

        // Time to frequency domain
        spectrum fft(uint16_t points) const;

        // Signal manipulation
        void lowpass(double cutoff);
        void upconvert(double frequency);
        void downconvert(double frequency, double bandwidth);

        // Data access
        size_t samples() const;
        channel& operator[] (uint16_t channel);
        const channel& operator[] (uint16_t channel) const;

        // Mathematical operations
        signal& operator*=(std::complex<double> value);
        signal& operator/=(std::complex<double> value);

        // Signal concatenation
        signal operator+(const signal& other) const;
        signal operator*(uint32_t repetitions) const;
    };

    class window : public signal {
        size_t  start;
        size_t  end;
        signal& target;

    public:
        // Construct a window of a signal
        window(signal& target, int start, int end);

        // Commit changes
        void commit(void);
    };

    class spectrum {
    private:    
        std::vector<channel> data;

    public:
        // Spectrum parameters
        const uint16_t channels;
        const uint32_t rate;
        const uint16_t components;

        // Construct a spectrum
        spectrum(const spectrum& spec);
        spectrum(const signal& sig, uint16_t components);
        spectrum(uint16_t channels, uint32_t rate, uint16_t components);

        // Frequency to time domain
        operator signal(void) const;
        signal ifft(void) const;

        // Data access
        size_t samples() const;
        channel& operator[] (uint16_t channel);
        const channel& operator[] (uint16_t channel) const;

        // Mathematical operations
        std::complex<double> abs(void) const;
        std::complex<double> dot(const spectrum& other) const;
        std::complex<double> operator^(const spectrum& other) const;
        std::complex<double> correlation(const spectrum& other) const;
        spectrum& operator*=(std::complex<double> value);
        spectrum& operator/=(std::complex<double> value);
        spectrum& operator*=(const spectrum& other);
    };
}
