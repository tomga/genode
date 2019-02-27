TARGET   = rpi_fb_drv
SRC_CC   = main.cc
LIBS     = base blit
INC_DIR += $(REP_DIR)/include/spec/rpi
INC_DIR += $(PRG_DIR)
INC_DIR += $(call select_from_repositories,include/spec/rpi)
