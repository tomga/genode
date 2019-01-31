
        .macro MEMTRACE_INIT
        mov r7, #0x30
        lsl r7, #24

        mov r9, #0x30
        lsl r9, #24
        add r9, #4
        str r9, [r7]
        .endm

        .macro MEMTRACE_STR4 char0=0, char1=0, char2=0, char3=0
        mov r7, #0x30
        lsl r7, #24
        ldr r9, [r7]

        mov r8, \char3
        lsl r8, #8
        orr r8, \char2
        lsl r8, #8
        orr r8, \char1
        lsl r8, #8
        orr r8, \char0
        str r8, [r9]
        add r9, #4

        mov r8, #0x1f
        lsl r8, #8
        orr r8, #0x1f
        lsl r8, #8
        orr r8, #0x1f
        lsl r8, #8
        orr r8, #0x1f
        str r8, [r9]

        str r9, [r7]

        .endm


        .macro MEMTRACE_REG4 reg0=r8
        mov r7, #0x30
        lsl r7, #24
        ldr r9, [r7]

        str \reg0, [r9]
        add r9, #4

        mov r8, #0x1f
        lsl r8, #8
        orr r8, #0x1f
        lsl r8, #8
        orr r8, #0x1f
        lsl r8, #8
        orr r8, #0x1f
        str r8, [r9]

        str r9, [r7]
        .endm

