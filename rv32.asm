addi t0, zero, 64 // M
addi t1, zero, 60 // N
addi t2, zero, 32 // K
addi s0, zero, 0x100 // s0 = 0x100 (pa, s0)
mul s1, t0, t2 // s1 = M * K
add s1, s1, s0 // s1 += s0       (pb, s1)
add t4, s1, zero // t4 = s1
addi a0, zero, 2 // a0 = 2
mul a0, a0, t2 // a0 = 2 * K
mul a0, a0, t1 // a0 = 2 * K * N
add a0, a0, s1 // a0 += s1    (pc, a0)
addi a1, zero, 0 // fory (a1) {
addi a2, zero, 0 // forx (a2) {
addi t6, zero, 0 // t6 = 0
add s1, zero, t4 // reset s1 
addi a3, zero, 0 // fork (a3) {
add s2, s0, a3 // &pa[k] = pa + k
lb s2, 0, s2 // *pa[k]
addi s3, zero, 2   // &pb[x] = 2
mul s3, s3, a2 // &pb[x] = 2 * k
add s3, s3, s1 // &pb[x] = 2 * k + pb
lh s3, 0, s3 // *pb[k]
mul s4, s2, s3 // s4 = pa[k] * pb[x]
add t6, t6, s4 // s += s4
addi t5, zero, 2
mul t5, t5, t1
add s1, s1, t5 // pb += 2 * N
addi a3, a3, 1 // ++k
blt a3, t2, -48
addi s5, zero, 4 // s5 = 4
mul s5, s5, a2 // s5 = 4 * x
add s5, s5, a0 // s5 = pc + 4 * x
sw t6, 0, s5
addi a2, a2, 1 // ++x
blt a2, t1, -84
add s0, s0, t2 // pa += K
addi t5, zero, 4 // t = 4
mul t5, t5, t1 // t = 4 * N
add a0, a0, t5 // pc += 4 * N
addi a1, a1, 1 // ++y
blt a1, t0, -112
