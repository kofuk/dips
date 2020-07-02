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
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>

#include "disassembler.hh"

namespace dips {
    disassembler::disassembler(const std::string infile,
                               const std::uint32_t base)
        : infile(infile), base(base) {}

    std::string disassembler::new_label(const std::uint32_t jta) {
        auto itr = labels.find(jta);
        if (itr != labels.end()) {
            return (*itr).second;
        }

        std::string label = "L" + std::to_string(label_index++);
        labels[jta] = label;
        return label;
    }

    void disassembler::emit_asm_i(const instruction &inst) {
        std::uint8_t op = inst.op;
        if (op <= 7 || op == 17) {
            std::string label = new_label(inst.jta);
            if (op == 1) {
                if (inst.rt == 0) {
                    std::cout << (inst.rt == 0 ? "\tbltz\t" : "\tbgez\t")
                              << regs[inst.rs] << ", " << label << std::endl;
                }
            } else if (op <= 5) {
                std::cout << (op == 4 ? "\tbeq\t" : "\tbne\t") << regs[inst.rs]
                          << ", " << regs[inst.rt] << ", " << label
                          << std::endl;
            } else if (op <= 7) {
                std::cout << (op == 6 ? "\tblez\t" : "\tbgtz\t")
                          << regs[inst.rs] << "," << label << std::endl;
            } else {
                std::cout << (inst.rt == 0 ? "\tbc1f\t" : "\tbc1t\t") << label
                          << std::endl;
            }
        } else if (op <= 14) {
            std::string mnemonic;
            switch (op) {
            case 8:
                mnemonic = "addi";
                break;
            case 9:
                mnemonic = "addiu";
                break;
            case 10:
                mnemonic = "slti";
                break;
            case 11:
                mnemonic = "sltiu";
                break;
            case 12:
                mnemonic = "andi";
                break;
            case 13:
                mnemonic = "ori";
                break;
            case 14:
                mnemonic = "xori";
                break;
            }
            std::cout << '\t' << mnemonic << '\t' << regs[inst.rt] << ", "
                      << regs[inst.rs] << ", 0x" << std::setw(4)
                      << std::setfill('0') << inst.imm << std::endl;
        } else if (op == 15) {
            std::cout << "\tlui\t" << regs[inst.rt] << "0x" << std::setw(4)
                      << std::setfill('0') << inst.imm << std::endl;
        } else if (op <= 43) {
            std::string mnemonic;
            switch (op) {
            case 32:
                mnemonic = "lb";
                break;
            case 33:
                mnemonic = "lh";
                break;
            case 35:
                mnemonic = "lw";
                break;
            case 36:
                mnemonic = "lbu";
                break;
            case 37:
                mnemonic = "lhu";
                break;
            }
            std::cout << '\t' << mnemonic << '\t' << regs[inst.rt] << ", "
                      << "0x" << std::setw(4) << std::setfill('0') << inst.imm
                      << '(' << regs[inst.rs] << ')' << std::endl;
        }
    }

    void disassembler::emit_asm_r(const instruction &inst) {
        std::uint8_t op = inst.op;
        if (op == 16) {
            std::cout << (inst.rs == 0 ? "\tmfc0\t" : "\tmtc0\t")
                      << regs[inst.rt] << ", " << regs[inst.rd] << std::endl;
        } else if (op == 28) {
            std::cout << "\tmul\t" << regs[inst.rd] << ", " << regs[inst.rs]
                      << ", " << regs[inst.rt] << std::endl;
        } else {
            std::uint8_t funct = inst.funct;
            if (funct <= 3) {
                std::string mnemonic;
                switch (funct) {
                case 0:
                    mnemonic = "sll";
                    break;
                case 2:
                    mnemonic = "srl";
                    break;
                case 3:
                    mnemonic = "sra";
                    break;
                }
                std::cout << '\t' << mnemonic << '\t' << regs[inst.rd] << ", "
                          << regs[inst.rt] << ", 0x" << std::setw(4)
                          << std::setfill('0') << inst.shamt << std::endl;
            } else if (funct <= 7) {
                std::string mnemonic;
                switch (funct) {
                case 4:
                    mnemonic = "sllv";
                    break;
                case 6:
                    mnemonic = "srlv";
                    break;
                case 7:
                    mnemonic = "srav";
                    break;
                }
                std::cout << '\t' << mnemonic << '\t' << regs[inst.rd] << ", "
                          << regs[inst.rt] << regs[inst.rs] << std::endl;
            } else if (funct <= 9) {
                std::cout << (funct == 8 ? "\tjr\t" : "\tjalr\t")
                          << regs[inst.rs] << std::endl;
            } else if (funct <= 13) {
                std::cout << (funct == 12 ? "\tsyscall" : "\tbreak")
                          << std::endl;
            } else if (funct == 16) {
                std::cout << "\tmfhi\t" << regs[inst.rd] << std::endl;
            } else if (funct == 17) {
                std::cout << "\tmthi\t" << regs[inst.rs] << std::endl;
            } else if (funct == 18) {
                std::cout << "\tmflo\t" << regs[inst.rd] << std::endl;
            } else if (funct == 19) {
                std::cout << "\tmtlo\t" << regs[inst.rs] << std::endl;
            } else if (funct <= 27) {
                std::string mnemonic;
                switch (funct) {
                case 24:
                    mnemonic = "mult";
                    break;
                case 25:
                    mnemonic = "multu";
                    break;
                case 26:
                    mnemonic = "div";
                    break;
                case 27:
                    mnemonic = "divu";
                    break;
                }
                std::cout << '\t' << mnemonic << '\t' << regs[inst.rt]
                          << regs[inst.rs] << std::endl;
            } else if (funct <= 43) {
                std::string mnemonic;
                switch (funct) {
                case 32:
                    mnemonic = "add";
                    break;
                case 33:
                    mnemonic = "addu";
                    break;
                case 34:
                    mnemonic = "sub";
                    break;
                case 35:
                    mnemonic = "subu";
                    break;
                case 36:
                    mnemonic = "and";
                    break;
                case 37:
                    mnemonic = "or";
                    break;
                case 38:
                    mnemonic = "xor";
                    break;
                case 39:
                    mnemonic = "nor";
                    break;
                case 42:
                    mnemonic = "slt";
                    break;
                case 43:
                    mnemonic = "sltu";
                    break;
                }
                std::cout << '\t' << mnemonic << '\t' << regs[inst.rd] << ", "
                          << regs[inst.rs] << ", " << regs[inst.rt]
                          << std::endl;
            }
        }
    }

    void disassembler::emit_asm_j(const instruction &inst) {
        if (inst.op == 2) {
            std::cout << "\tj\t";
        } else {
            std::cout << "\tjal\t";
        }
        std::cout << new_label(inst.jta) << std::endl;
    }

    void disassembler::emit_asm(const instruction &inst) {
        if (!inst.op || inst.op == 16 || inst.op == 28) {
            emit_asm_r(inst);
        } else if (inst.op == 2 || inst.op == 3) {
            emit_asm_j(inst);
        } else {
            emit_asm_i(inst);
        }
    }

    void disassembler::decode_i(const std::uint32_t inst) {
        std::uint8_t op = (inst & 0xfc000000) >> 26;
        std::uint8_t rs = (inst & 0x03e00000) >> 21;
        std::uint8_t rt = (inst & 0x001f0000) >> 16;
        std::uint16_t imm = inst & 0x0000ffff;

        std::uint32_t jta = 0;
        if (op <= 7 || op == 17) {
            std::int16_t diff = static_cast<std::int16_t>(imm) * 4;
            jta = static_cast<std::uint32_t>(static_cast<std::int64_t>(pc) +
                                             diff);
            new_label(jta);
        }

        instructions.push_back(
            {.op = op, .rs = rs, .rt = rt, .imm = imm, .jta = jta});
    }

    void disassembler::decode_r(const std::uint32_t inst) {
        std::uint8_t op = (inst & 0xfc000000) >> 26;
        std::uint8_t rs = (inst & 0x03e00000) >> 21;
        std::uint8_t rt = (inst & 0x001f0000) >> 16;
        std::uint8_t rd = (inst & 0x0000f800) >> 11;
        std::uint8_t shamt = (inst & 0x000007c0) >> 6;
        std::uint8_t funct = (inst & 0x0000003f);

        instructions.push_back({.op = op,
                                .rs = rs,
                                .rt = rt,
                                .rd = rd,
                                .shamt = shamt,
                                .funct = funct});
    }

    void disassembler::decode_j(const std::uint32_t inst) {
        std::uint8_t op = inst & 0xfc000000 >> 26;
        std::uint32_t jta =
            ((pc + 4) & 0xf0000000) | ((inst & 0x03ffffff) << 2);

        instructions.push_back({.op = op, .jta = jta});
        new_label(jta);
    }

    void disassembler::decode_instruction(const std::uint32_t inst) {
        pc += 4;

        std::uint32_t op = (inst & 0xfc000000) >> 26;
        if (!op || op == 16 || op == 28) {
            decode_r(inst);
        } else if (op == 2 || op == 3) {
            decode_j(inst);
        } else {
            decode_i(inst);
        }
    }

    void
    disassembler::decode_instructions(const char *data,
                                      const size_t length) noexcept(false) {
        if (length % 4) {
            throw std::length_error("data is well aligned");
        }
        for (size_t i = 0; i < length; i += 4) {
            char d[4] = {data[i + 3], data[i + 2], data[i + 1], data[i]};
            decode_instruction(*reinterpret_cast<std::uint32_t *>(d));
        }
    }

    void disassembler::disassemble() noexcept(false) {
        std::ifstream strm = std::ifstream(infile);
        if (!strm) {
            throw std::runtime_error("failed to open input file");
        }

        pc = base;
        std::cout << std::hex;
        char buf[4096];
        do {
            strm.read(buf, 4096);
            decode_instructions(buf, static_cast<size_t>(strm.gcount()));
        } while (!strm.eof());

        pc = base;
        for (const instruction &i : instructions) {
            auto itr = labels.find(pc);
            if (itr != labels.end()) {
                std::cout << (*itr).second << ':' << std::endl;
            }
            emit_asm(i);
            pc += 4;
        }
    }
} // namespace dips
