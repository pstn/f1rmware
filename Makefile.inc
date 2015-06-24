RPATH?=.

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
AS = $(CROSS_COMPILE)as
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size
DFUTOOL= $(RPATH)/tools/make-dfu.py
DFUUTIL= dfu-util

INCLUDE_PATHS += -I$(RPATH)/libopencm3/include
INCLUDE_PATHS += -I$(RPATH)/hackrf/firmware
INCLUDE_PATHS += -I$(RPATH)/r0ketlib
INCLUDE_PATHS += -I$(RPATH)/rad1olib

CFLAGS += -std=c99 -c -g -Os -Wall -Wno-unused-function
CFLAGS += -fno-common -ffunction-sections -fdata-sections -fno-builtin -ffreestanding
#CFLAGS += -fmessage-length=0 
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

LDFLAGS += --gc-sections
LDFLAGS += -T$(RPATH)/ld/LPC4330_M4_memory.ld 
LDFLAGS += -T$(RPATH)/libopencm3/lib/libopencm3_lpc43xx.ld
LDFLAGS += -T$(RPATH)/ld/LPC43xx_M4_M0_image_from_text.ld
LDFLAGS += -Map=$(APP).map

LIBS += -L$(RPATH)/libopencm3/lib -lopencm3_lpc43xx
LIBS += -L/usr/arm-none-eabi/lib/armv7e-m/fpu -Os -lc -lnosys -lm 

OCFLAGS = --strip-unneeded

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -o $@ $<

%.bin: %.elf
	$(OBJCOPY) $(OCFLAGS) -O binary $< $@

%.dfu: %.bin
	$(DFUTOOL) $< $@

all: $(APP).dfu

clean::
	-rm -f $(OBJS) $(APP).elf $(APP).bin $(APP).dfu $(APP).map

program: flash

flash: $(APP).dfu
	$(DFUUTIL) --device 1fc9:000c --alt 0 --download $(APP).dfu

$(APP).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $(APP).elf $(OBJS) $(LIB_PATHS) $(LIBS)
	$(SIZE) $(APP).elf