/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem
{    
    stream::response stream_hello::incomming(const std::vector<byte>& data) {
        return STREAM_Okay;
    }
    
    stream::response stream_hello::outgoing(std::vector<byte>& data) {
        std::string hello("Hello world!");
        data.insert(data.begin(), hello.begin(), hello.end());
        return STREAM_Okay;
    }
}
