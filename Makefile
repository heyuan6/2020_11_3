#CROSS_COMPILE = arm-none-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib
LIBS=-lsqlite3

#MODULE_NAME = led_drv
#MODULE_NAME1 = buzzer_drv
#MODULE_NAME2 = adc_drv
#MODULE_NAME3 = mpu6050_drv

ifeq ($(KERNELRELEASE), )

NFSBOA_CGIBIN=~/source/rootfs/boa/cgi-bin

all: info_client info_server		

info_client: info_client.o
	$(CC) info_client.o -o info_client ${LIBS}
info_server: info_server.o 
	$(CC) info_server.o -o info_server ${LIBS}	
	
	
CUR_DIR = $(shell pwd)

KERNEL_DIR = /home/wu/linux-3.14/linux-3.14-fs4412/
install:
#	sudo cp *.cgi $(NFSBOA_CGIBIN)
#	cp *.ko ~/source/rootfs/drv/
	
.PHONY:clean	
clean:
	
#	make -C  $(KERNEL_DIR) M=$(CUR_DIR) clean
	rm info_client info_server *.o
else

#obj-m += $(MODULE_NAME).o
endif
