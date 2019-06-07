TARGET   = rpi_gpio_drv
REQUIRES = arm
SRC_CC  += main.cc
LIBS     = base
INC_DIR += $(PRG_DIR)

vpath main.cc $(PRG_DIR)
