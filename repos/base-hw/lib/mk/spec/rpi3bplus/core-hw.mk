#
# \brief  Build config for Genodes core process
# \author Stefan Kalkowski
# \date   2015-02-09
#

# add include paths
INC_DIR += $(REP_DIR)/src/core/spec/rpi3bplus
INC_DIR += $(REP_DIR)/src/core/spec/arm_v7/virtualization

# add C++ sources
SRC_CC += spec/rpi3bplus/pic.cc
SRC_CC += spec/rpi3bplus/platform_services.cc
SRC_CC += kernel/vm_thread_on.cc
SRC_CC += spec/arm_v7/virtualization/kernel/vm.cc
SRC_CC += spec/arm_v7/vm_session_component.cc
SRC_CC += spec/arm_v7/virtualization/vm_session_component.cc
SRC_CC += spec/rpi3bplus/timer.cc
SRC_CC += spec/rpi3bplus/cpu.cc

# add assembly sources
SRC_S += spec/arm_v7/virtualization/exception_vector.s

NR_OF_CPUS = 1

#
# we need more specific compiler hints for some 'special' assembly code
# override -march=armv7-a because it conflicts with -mcpu=cortex-a15
#
CC_MARCH = -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp

# include less specific configuration
#include $(REP_DIR)/lib/mk/spec/arm_v7/core-hw.inc
include $(REP_DIR)/lib/mk/spec/cortex_a15/core-hw.inc
