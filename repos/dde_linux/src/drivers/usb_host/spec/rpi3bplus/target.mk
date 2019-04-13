include $(REP_DIR)/src/drivers/usb_host/target.inc

TARGET   = rpi3bplus_usb_host_drv
REQUIRES = rpi3bplus

INC_DIR += $(REP_DIR)/src/drivers/usb_host/spec/arm
INC_DIR += $(REP_DIR)/src/include/spec/arm

INC_DIR += $(call select_from_repositories,include/spec/rpi)

SRC_CC  += spec/arm/platform.cc
SRC_CC  += spec/rpi3bplus/platform.cc

SRC_C   += usb/dwc2/core.c
## SRC_C   += usb/dwc2/core.h
SRC_C   += usb/dwc2/core_intr.c
## SRC_C   += usb/dwc2/debugfs.c
## SRC_C   += usb/dwc2/debug.h
## SRC_C   += usb/dwc2/gadget.c
SRC_C   += usb/dwc2/hcd.c
SRC_C   += usb/dwc2/hcd_ddma.c
## SRC_C   += usb/dwc2/hcd.h
SRC_C   += usb/dwc2/hcd_intr.c
SRC_C   += usb/dwc2/hcd_queue.c
## SRC_C   += usb/dwc2/hw.h
SRC_C   += usb/dwc2/params.c
SRC_C   += usb/dwc2/platform.c

CC_OPT  += -DCONFIG_USB_EHCI_TT_NEWSCHED=1
CC_OPT  += -DCONFIG_USB_DWC2_HOST=1
## CC_OPT  += -DDWC2_LOG_READS
## CC_OPT  += -DDWC2_LOG_WRITES
## CC_OPT  += -DDEBUG
## CC_OPT  += -DVERBOSE_DEBUG
