/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem {
    class exception : public std::exception {
    private:
        std::string message;

    protected:
        virtual std::string compute(const std::string& file, int line) {
            std::stringstream ss;
            ss << "Exception in \"" << file << "\" @ line" << line;
            return ss.str();
        }

    public:
        exception& update(const std::string& file, int line) {
            if(message.empty()) message = compute(file, line);
            return *this;
        }
    
        const char* what() const noexcept {
            return message.c_str();
        }
    };
}


#define throwx(excep) throw excep.update(std::string(__FILE__), __LINE__);
