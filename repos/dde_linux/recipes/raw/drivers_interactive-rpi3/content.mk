content: drivers.config fb_drv.config platform_drv.config

drivers.config fb_drv.config platform_drv.config:
	cp $(REP_DIR)/recipes/raw/drivers_interactive-rpi3/$@ $@
