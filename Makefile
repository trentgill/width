TARGET=main
EXECUTABLE=main.elf

CUBE=submodules/STM32_Cube_F7/Drivers
HAL=$(CUBE)/STM32F7xx_HAL_Driver/Src

COMPILER=arm-none-eabi
#COMPILER=~/dev/arm-gcc-8/bin/arm-none-eabi
COMPILER_VERSION=-4.9.3

DEFS = -DSTM32F7XX -DARM_MATH_CM7 -DHSE_VALUE=8000000
STARTUP = $(CUBE)/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f765xx.s

## shouldn't need to change much until source files

CC=$(COMPILER)-gcc$(COMPILER_VERSION)
LD=$(COMPILER)-gcc$(COMPILER_VERSION)
AR=$(COMPILER)-ar
AS=$(COMPILER)-as
CP=$(COMPILER)-objcopy
OBJDUMP=$(COMPILER)-objdump

BIN = $(TARGET).bin


MCFLAGS = -mthumb -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16

STM32_INCLUDES = \
	-I$(CUBE)/CMSIS/Device/ST/STM32F7xx/Include/ \
	-I$(CUBE)/CMSIS/Include/ \
	-I$(CUBE)/STM32F7xx_HAL_Driver/Inc/ \
	-I$(CARP_DIR)/core/ \
#	-I$(WRLIB)/
#	-I$(WRDSP)/

OPTIMIZE       = -O2

CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += $(MCFLAGS)
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEFS) -I. -I./ $(STM32_INCLUDES)
CFLAGS += -fsingle-precision-constant -Wdouble-promotion

LDFLAGS = -Wl,-T,stm32_flash.ld
LIBS    = -lm -lc -lnosys

SRC = out/main.c \
	system_stm32f7xx.c \
	$(HAL)/stm32f7xx_hal.c \
	$(HAL)/stm32f7xx_hal_cortex.c \
	$(HAL)/stm32f7xx_hal_flash.c \
	$(HAL)/stm32f7xx_hal_flash_ex.c \
	$(HAL)/stm32f7xx_hal_gpio.c \
	$(HAL)/stm32f7xx_hal_rcc.c \
	$(HAL)/stm32f7xx_hal_rcc_ex.c \

R ?= 0
ifeq ($(R), 1)
    CFLAGS += -DRELEASE
else
    CFLAGS += -DDEBUG
endif


OBJDIR = .
OBJS = $(SRC:%.c=$(OBJDIR)/%.o) 
OBJS += Startup.o

# C dependencies echoed into Makefile
DEP = $(OBJS:.o=.d)

# OS dependent size printing
UNAME := $(shell uname)

GETSIZE = stat

ifeq ($(UNAME), Darwin)
	GETSIZE = stat -x
endif


all: $(TARGET).hex $(BIN)

-include $(DEP)

$(TARGET).hex: $(EXECUTABLE)
	@$(CP) -O ihex $^ $@

$(EXECUTABLE): $(OBJS)
	@$(LD) -g $(MCFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
	@echo "linked:       $@"
	@$(OBJDUMP) --disassemble $@ > $@.lst
	@echo "disassembly:  $@.lst"

$(BIN): $(EXECUTABLE)
	@$(CP) -O binary $< $@
	@echo "binary:       $@"
	@$(OBJDUMP) -x --syms $< > $(addsuffix .dmp, $(basename $<))
	@echo "symbol table: $@.dmp"
	@$(GETSIZE) main.bin | grep 'Size'
	@echo "        ^ must be less than 1MB (1,000,000)"

flash: $(BIN)
	st-flash write $(BIN) 0x08018000

debug: $(BIN)
	st-flash write $(BIN) 0x08018000
	stlink-trace -c 216

%.o: %.c
	@$(CC) -ggdb -fno-common $(CFLAGS) -c $< -o $@
	@echo $@
	@$(GETSIZE) $@ | grep 'Size'

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.s: %.c
	@$(CC) -ggdb $(CFLAGS) -S $< -o $@

out/main.c: main.carp
	carp build.carp --no-core

Startup.o: $(STARTUP)
	@$(CC) -ggdb $(CFLAGS) -c $< -o $@
	@echo $@

clean:
	@rm -f Startup.lst $(TARGET).elf.lst $(OBJS) $(AUTOGEN) \
		$(TARGET).out  $(TARGET).hex  $(TARGET).map \
		$(TARGET).dmp  $(EXECUTABLE) out/main.c \
		$(TARGET).bin $(DEP)
