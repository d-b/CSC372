/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem {
    namespace soundfile {
        int save(std::string filename, const signal& sig);
        std::shared_ptr<signal> load(std::string filename);
    }
}
