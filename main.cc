/*
 * dips -- A toy MIPS disassembler.
 * Copyright (C) 2020  Koki Fukuda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "disassembler.hh"

namespace {
    void print_usage() {
        std::cout << "usage: dips [OPTIONS]... FILE" << std::endl;
        std::cout << "  --base N    Base address (e.g. 0x10000000)"
                  << std::endl;
    }

    void print_version() {
        std::cout << "dips" << std::endl;
        std::cout << "A toy MIPS disassembler." << std::endl;
    }
} // namespace

int main(int argc, char **argv) {
    std::uint32_t base = 0;
    std::string infile;
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "--", 2)) {
            if (!strcmp(argv[i], "--base")) {
                if (i + 1 < argc) {
                    try {
                        base = static_cast<std::uint32_t>(
                            std::stol(argv[i + 1], nullptr, 0));
                    } catch (...) {
                        print_usage();
                        return 1;
                    }
                } else {
                    print_usage();
                    return 1;
                }
            } else if (!strcmp(argv[i], "--version")) {
                print_version();
                return 0;
            } else if (!strcmp(argv[i], "--help")) {
                print_usage();
                return 0;
            } else {
                print_usage();
                return 1;
            }
        } else {
            infile = argv[i];
        }
    }

    if (infile.empty()) {
        print_usage();
        return 1;
    }

    dips::disassembler d(infile, base);
    try {
        d.disassemble();
    } catch (const std::runtime_error &e) {
        std::cerr << "Fatal: " << e.what() << std::endl;
        return 1;
    }
}
