#
# \brief  Build config for Genodes core process
# \author Tomasz Gajewski
# \date   2019-06-20
#

# add include paths
INC_DIR += $(REP_DIR)/src/core/spec/rpi2
INC_DIR += $(REP_DIR)/src/core/spec/arm_v7/virtualization

# add C++ sources
SRC_CC += spec/rpi2/pic.cc
SRC_CC += spec/rpi2/platform_services.cc
SRC_CC += kernel/vm_thread_on.cc
SRC_CC += spec/arm_v7/virtualization/kernel/vm.cc
SRC_CC += spec/arm_v7/vm_session_component.cc
SRC_CC += spec/arm_v7/virtualization/vm_session_component.cc
SRC_CC += vm_session_common.cc
SRC_CC += spec/rpi2/timer.cc

# add assembly sources
SRC_S += spec/arm_v7/virtualization/exception_vector.s

NR_OF_CPUS = 4

#
# we need more specific compiler hints for some 'special' assembly code
# override -march=armv7-a because it conflicts with -mcpu=cortex-a15
#
CC_MARCH = -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp

# include less specific configuration
#include $(REP_DIR)/lib/mk/spec/arm_v7/core-hw.inc
include $(REP_DIR)/lib/mk/spec/cortex_a15/core-hw.inc
