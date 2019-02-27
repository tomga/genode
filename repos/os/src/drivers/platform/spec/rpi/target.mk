TARGET   = platform_drv
SRC_CC   = main.cc
INC_DIR += $(REP_DIR)/include/spec/rpi
INC_DIR += ${PRG_DIR}
LIBS     = base

# enable C++11 support
CC_CXX_OPT += -std=gnu++11
