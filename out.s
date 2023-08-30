.global _start
.align 3

_start:
    mov X0, 1
    stp X0, XZR, [SP, #-16]!
    mov X0, 2
    stp X0, XZR, [SP, #-16]!
    mov X0, 3
    stp X0, XZR, [SP, #-16]!
    ldp X0, XZR, [SP], #16
    ldp X1, XZR, [SP], #16
    add X0, X0, X1
    stp X0, XZR, [SP, #-16]!
    ldp X0, XZR, [SP], #16
    ldp X1, XZR, [SP], #16
    add X0, X0, X1
    stp X0, XZR, [SP, #-16]!
    mov X0, 8
    stp X0, XZR, [SP, #-16]!
    ldr X0, [SP, #16]
    stp X0, XZR, [SP, #-16]!
    ldp X0, XZR, [SP], #16
    mov X16, #1
    svc #0x80
    mov X0, #0
    mov X16, #1
    svc #0x80