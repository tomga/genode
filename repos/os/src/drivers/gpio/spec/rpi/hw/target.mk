TARGET   = hw_gpio_drv
SRC_CC  += main.cc
LIBS     = base
INC_DIR += $(REP_DIR)/include/spec/rpi
INC_DIR += $(PRG_DIR) $(PRG_DIR)/..

vpath main.cc $(PRG_DIR)/..
