# This Makefile is meant to be used by the wr-switch. When built for wr-node
# the relevant Makefile lived in the parent directory.

ccflags-y += -DWR_SWITCH

obj-m := wr-nic.o
wr-nic-objs := module.o device.o nic-core.o endpoint.o ethtool.o \
		pps.o timestamp.o

# accept WRN_DEBUG from the environment. It turns pr_debug() into printk.
ifdef WRN_DEBUG
	ccflags-y += -DDEBUG
endif

ccflags-y += -I$(src)/../wr_pstats

# What follows is standard stuff
export ARCH ?= arm
export CROSS_COMPILE ?= $(CROSS_COMPILE_ARM)

all modules:
	$(MAKE) CONFIG_DEBUG_SECTION_MISMATCH=y \
		-C $(LINUX) SUBDIRS=$(shell /bin/pwd) modules

# looking at preprocessed output is helpful for bug hunting
preprocess:
	$(MAKE) CONFIG_DEBUG_SECTION_MISMATCH=y \
		-C $(LINUX) SUBDIRS=$(shell /bin/pwd) $(wr-nic-objs:.o=.i)

# We might "$(MAKE) -C $(LINUX)" but "make clean" with no LINUX defined
# is sometimes useful to have
clean:
	rm -f *.mod.c *.o *.ko *.i .*cmd Module.symvers modules.order *~
	rm -rf .tmp_versions
