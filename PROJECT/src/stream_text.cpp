/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem
{
    stream_text::stream_text(std::string text)
        :text(text)
    {};

    stream::response stream_text::incomming(const std::vector<byte>& data) {
        if(!data.empty()) {
            std::for_each(data.begin(), data.end(), [](byte b){
                std::cout << (char) b << std::flush;
            });
        } return STREAM_Okay;
    }
    
    stream::response stream_text::outgoing(std::vector<byte>& data) {
        data.insert(data.begin(), text.begin(), text.end());
        return STREAM_Okay;
    }
}
