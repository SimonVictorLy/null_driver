CONFIG_MODULE_SIG=n
obj-m := ofcd.o
ifeq (${KERNALRELEASE},)
	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean
else
	obj-m := ofcd.o
endif
