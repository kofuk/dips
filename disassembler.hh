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

#ifndef DISASSEMBLER_HH
#define DISASSEMBLER_HH

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace dips {
    namespace {
        const std::string regs[] = {
            "$0",  "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
            "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
            "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
            "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
        };
    }

    struct instruction {
        std::uint8_t op;
        std::uint8_t rs;
        std::uint8_t rt;
        std::uint8_t rd;
        std::uint8_t shamt;
        std::uint8_t funct;
        std::uint16_t imm;

        std::uint32_t jta;
    };

    class disassembler {
        std::string infile;
        std::uint32_t base;
        bool ascii_input = false;

        std::uint32_t pc;
        std::unordered_map<std::uint32_t, std::string> labels;
        int label_index = 0;
        std::vector<instruction> instructions;

        void emit_asm_i(const instruction &inst);
        void emit_asm_r(const instruction &inst);
        void emit_asm_j(const instruction &inst);
        void emit_asm(const instruction &inst);

        std::string new_label(const std::uint32_t jta);

        void decode_i(const std::uint32_t inst);
        void decode_r(const std::uint32_t inst);
        void decode_j(const std::uint32_t inst);

        void decode_instruction(const std::uint32_t inst) noexcept(false);
        void decode_instructions(const char *data,
                                 const size_t length) noexcept(false);

    public:
        disassembler(const std::string infile, const std::uint32_t base);

        void disassemble() noexcept(false);
        void read_ascii();
    };
} // namespace dips

#endif
