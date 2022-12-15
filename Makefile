modname := keylogger
obj-m := $(modname).o

keylogger-objs := keys.o

KVERSION = $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build

ifdef DEBUG
CFLAGS_$(obj-m) := -DDEBUG
endif

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
