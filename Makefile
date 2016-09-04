obj-m	=smc_access.o
KDIR := ../../src/arm-src-kernel-2015-11-04-9e845bea41/
PWD := $(shell pwd)

.PHONY: default
default:
	$(MAKE) -C $(KDIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD)
