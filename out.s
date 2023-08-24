.global _start
.align 3

_start:
    mov X0, 7
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