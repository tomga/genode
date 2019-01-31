
        .macro MEMTRACE_INIT
        mrc p15, 0, r8, c0, c0, 5   /* read multiprocessor affinity register */
        and r8, r8, #0xff
        mov r7, #0x1000
        mul r8, r8, r7              /* calculate offset */
        mov r7, #0x20
        lsl r7, #24
        add r7, r7, r8              /* calculate processor log buffer */

        mov r9, #0x20
        lsl r9, #24
        add r9, r9, r8
        add r9, #4
        str r9, [r7]
        .endm

        .macro MEMTRACE_STR4 char0=0, char1=0, char2=0, char3=0
        mrc p15, 0, r8, c0, c0, 5   /* read multiprocessor affinity register */
        and r8, r8, #0xff
        mov r7, #0x1000
        mul r8, r8, r7              /* calculate offset */
        mov r7, #0x20
        lsl r7, #24
        add r7, r7, r8              /* calculate processor log buffer */
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
        mrc p15, 0, r8, c0, c0, 5   /* read multiprocessor affinity register */
        and r8, r8, #0xff
        mov r7, #0x1000
        mul r8, r8, r7              /* calculate offset */
        mov r7, #0x20
        lsl r7, #24
        add r7, r7, r8              /* calculate processor log buffer */
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

