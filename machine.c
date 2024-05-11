#include "machine.h"

#include <stdint.h>

#include "core.h"
#include "memory.h"

#define sp 2

// https://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
int32_t sext20(uint32_t x) {
    struct {int32_t x: 20;} s;
    return s.x = x;
}

int32_t sext12(uint32_t x) {
    struct {int32_t x: 12;} s;
    return s.x = x;
}

int32_t sext8(uint32_t x) {
    struct {int32_t x: 8;} s;
    return s.x = x;
}

int32_t sext16(uint32_t x) {
    struct {int32_t x: 16;} s;
    return s.x = x;
}

int32_t sext32(uint32_t x) {
    return (int32_t)x;
}

struct machine machine_new(enum cache_replacement_policy policy) {
    struct machine m;

    m.pc = 0;
    for(int i = 0; i < 32; ++i) m.rs[i] = 0;

    m.mem.hits = 0;
    m.mem.total = 0;

    m.mem.cache.rpolicy = policy;
    m.mem.cache.t = 0;
    for (int i = 0; i < CACHE_SETS; ++i) {
        for (int j = 0; j < CACHE_WAY; ++j) {
            m.mem.cache.time[i][j] = 0;
            m.mem.cache.tags[i][j] = UINT32_MAX;
        }
    }

    return m;
}

#define XLEN 32
#define U(a, op, b) (((uint32_t)(a)) op ((uint32_t)(b)))
#define S(a, op, b) (((int32_t)(a)) op ((int32_t)(b)))

void machine_process_ir(struct machine* m, struct intermediate ir) {
    uint32_t t;

#ifdef DEBUG
    printf("Executing [i=%i, a=%i, b=%i, c=%i]\n", ir.instr, ir.a, ir.b, ir.c);
#endif

    switch(ir.instr) {
        case INSTR_LUI: // lui rd, imm
            m->rs[ir.a] = sext20(ir.b) << 12;
            break;
        case INSTR_AUIPC: // auipc rd, imm
            m->rs[ir.a] = m->pc + (sext20(ir.b) << 12);
            break;
        case INSTR_JAL: // jal rd, offset
            m->rs[ir.a] = m->pc + 4;
            m->pc += sext20(ir.b); // TODO
            break;
        case INSTR_JALR: // jalr rd, rs1, offset
            t = m->pc + 4;
            m->pc = (m->rs[ir.b] + sext12(ir.c)) & ~1;
            m->rs[ir.a] = t;
            break;
        case INSTR_BEQ: // beq rs1, rs2, offset
            if (m->rs[ir.a] == m->rs[ir.b])
                m->pc += sext12(ir.c);
            break;
        case INSTR_BNE: // bne rs1, rs2, offset
            if (m->rs[ir.a] != m->rs[ir.b])
                m->pc += sext12(ir.c);
            break;
        case INSTR_BLT: // blt rs1, rs2, offset
            if (m->rs[ir.a] < m->rs[ir.b])
                m->pc += sext12(ir.c);
            break;
        case INSTR_BGE: // bge rs1, rs2, offset
            if (m->rs[ir.a] >= m->rs[ir.b])
                m->pc += sext12(ir.c);
            break;
        case INSTR_BLTU: // bltu rs1, rs2, offset
            if (m->rs[ir.a] < m->rs[ir.b])
                m->pc += sext12(ir.c);
            break;
        case INSTR_BGEU: // bge rs1, rs2, offset
            if (m->rs[ir.a] >= m->rs[ir.b])
                m->pc += sext12(ir.c);
            break;
            
        case INSTR_LB: // lb, rd, offset(rs1)
            m->rs[ir.a] = sext8(mem_read8(&m->mem, m->rs[ir.c] + sext12(ir.b)));
            break;
        case INSTR_LH: // lh, rd, offset(rs1)
            m->rs[ir.a] = sext16(mem_read16(&m->mem, m->rs[ir.c] + sext12(ir.b)));
            break;
        case INSTR_LW: // hw, rd, offset(rs1)
            m->rs[ir.a] = sext32(mem_read32(&m->mem, m->rs[ir.c] + sext12(ir.b)));
            break;
        case INSTR_LBU: // lbu rd, offset(rs1)
            m->rs[ir.a] = mem_read8(&m->mem, m->rs[ir.c] + sext12(ir.b));
            break;
        case INSTR_LHU: // lhu rd, offset(rs1)
            m->rs[ir.a] = mem_read16(&m->mem, m->rs[ir.c] + sext12(ir.b));
            break;

        case INSTR_SB: // sb rs2, offset(rs1)
            mem_write8(&m->mem, m->rs[ir.c] + sext12(ir.b), m->rs[ir.a]);
            break;
        case INSTR_SH: // sh rs2, offset(rs1)
            mem_write16(&m->mem, m->rs[ir.c] + sext12(ir.b), m->rs[ir.a]);
            break;
        case INSTR_SW: // sw rs2, offset(rs1)
            mem_write32(&m->mem, m->rs[ir.c] + sext12(ir.b), m->rs[ir.a]);
            break;

        case INSTR_ADDI: // addi, rd, rs1, imm
            m->rs[ir.a] = m->rs[ir.b] + sext12(ir.c);
            break;
        case INSTR_SLTI: // slti, rd, rs1, imm
            m->rs[ir.a] = m->rs[ir.b] < sext12(ir.c);
            break;
        case INSTR_SLTIU: // sltiu, rd, rs1, imm
            m->rs[ir.a] = m->rs[ir.b] < sext12(ir.c);
            break;
        case INSTR_XORI: // xori, rd, rs1, imm
            m->rs[ir.a] = m->rs[ir.b] ^ sext12(ir.c);
            break;
        case INSTR_ORI: // ori, rd, rs1, imm
            m->rs[ir.a] = m->rs[ir.b] | sext12(ir.c);
            break;
        case INSTR_ANDI: // andi, rd, rs1, imm
            m->rs[ir.a] = m->rs[ir.b] & sext12(ir.c);
            break;
        case INSTR_SLLI: // slli, rd, rs1, shamt
            m->rs[ir.a] = m->rs[ir.b] << ir.c;
            break;
        case INSTR_SRLI: // srli, rd, rs1, shamt
            m->rs[ir.a] = U(m->rs[ir.b], >>, ir.c); // u
            break;
        case INSTR_SRAI: // srli, rd, rs1, shamt
            m->rs[ir.a] = S(m->rs[ir.b], >>, ir.c); // s
            break;

        case INSTR_ADD: // add rd, rs1, rs2
            m->rs[ir.a] = m->rs[ir.b] + m->rs[ir.c];
            break;
        case INSTR_SUB: // sub rd, rs1, rs2
            m->rs[ir.a] = m->rs[ir.b] - m->rs[ir.c];
            break;
        case INSTR_SLL: // sll rd, rs1, rs2
            m->rs[ir.a] = m->rs[ir.b] << m->rs[ir.c];
            break;
        case INSTR_SLT: // slt rd, rs1, rs2
            m->rs[ir.a] = S(m->rs[ir.b], <, m->rs[ir.c]); // s
            break;
        case INSTR_SLTU: // sltu rd, rs1, rs2
            m->rs[ir.a] = U(m->rs[ir.b], <, m->rs[ir.c]); // u
            break;
        case INSTR_XOR: // xor rd, rs1, rs2
            m->rs[ir.a] = m->rs[ir.b] ^ m->rs[ir.c];
            break;
        case INSTR_SRL: // srl rd, rs1, rs2
            m->rs[ir.a] = U(m->rs[ir.b], >>, m->rs[ir.c]); // u
            break;
        case INSTR_SRA: // sra rd, rs1, rs2
            m->rs[ir.a] = S(m->rs[ir.b], >>, m->rs[ir.c]); // s
            break;
        case INSTR_OR: // or rd, rs1, rs2
            m->rs[ir.a] = m->rs[ir.b] | m->rs[ir.c];
            break;
        case INSTR_AND: // and rd, rs1, rs2
            m->rs[ir.a] = m->rs[ir.b] & m->rs[ir.c];
            break;

        case INSTR_FENCE:
        case INSTR_FENCE_TSO:
        case INSTR_PAUSE:
        case INSTR_ECALL:
        case INSTR_EBREAK:
            // basically they are nop here.
            break;
            
        case INSTR_MUL: // mul rd, rs1, rs2
            m->rs[ir.a] = U(m->rs[ir.b], *, m->rs[ir.c]);
            break;
        case INSTR_MULH: // mulh rd, rs1, rs2
            m->rs[ir.a] = ((int64_t)m->rs[ir.b] * (int64_t)m->rs[ir.c]) >> XLEN;
            break;
        case INSTR_MULHSU: // mulhsu rd, rs1, rs2
            m->rs[ir.a] = ((uint64_t)m->rs[ir.b] * (uint64_t)m->rs[ir.c]) >> XLEN;
            break;
        case INSTR_MULHU: // mulhu rd, rs1, rs2
            m->rs[ir.a] = ((int64_t)m->rs[ir.b] * (uint64_t)m->rs[ir.c]) >> XLEN;
            break;
        case INSTR_DIV: // div rd, rs1, rs2
            m->rs[ir.a] = S(m->rs[ir.b], /, m->rs[ir.c]); // s
            break;
        case INSTR_DIVU: // divu rd, rs1, rs2
            m->rs[ir.a] = U(m->rs[ir.b], /, m->rs[ir.c]); // u
            break;
        case INSTR_REM: // rem rd, rs1, rs2
            m->rs[ir.a] = S(m->rs[ir.b], %, m->rs[ir.c]); // s
            break;
        case INSTR_REMU: // rem rd, rs1, rs2
            m->rs[ir.a] = U(m->rs[ir.b], %, m->rs[ir.c]); // u
            break;
        default:
            PANIC("No such instruction: %i", ir.instr);
    }
}

void machine_debug_print(struct machine* m) {
    printf("PC: %i \n", m->pc);
    for(int i = 0; i < 32; ++i) {
        printf("x%i: %i\t", i, m->rs[i]);
    }
    printf("\n");
}

#undef S
#undef U
#undef XLEN
