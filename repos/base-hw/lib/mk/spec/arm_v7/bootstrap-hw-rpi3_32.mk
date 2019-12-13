INC_DIR += $(BASE_DIR)/../base-hw/src/bootstrap/spec/rpi3_32

SRC_CC  += bootstrap/spec/arm/cortex_a15_cpu.cc
SRC_CC  += bootstrap/spec/rpi3_32/platform.cc
SRC_CC  += bootstrap/spec/arm/arm_v7_cpu.cc
SRC_CC  += hw/spec/32bit/memory_map.cc
SRC_S   += bootstrap/spec/arm/crt0.s

NR_OF_CPUS = 4

#
# we need more specific compiler hints for some 'special' assembly code
# override -march=armv7-a because it conflicts with -mcpu=cortex-a15
#
CC_MARCH = -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp

include $(REP_DIR)/lib/mk/bootstrap-hw.inc
