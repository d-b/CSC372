/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{
    class stream_hello : public stream {
        response incomming(const std::vector<byte>& data);
        response outgoing(std::vector<byte>& data);
    };
}
