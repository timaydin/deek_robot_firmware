PROJECT := deek_robot

NUVOTON_ROOT=/extrahd1/mydata/projects/nuvoton

BSPROOT := ${NUVOTON_ROOT}/N76E003_BSP
BSP_DEVINC := ${BSPROOT}/Library/Device/Include
BSP_DRVINC := ${BSPROOT}/Library/StdDriver/inc
BSP_DRVSRC := ${BSPROOT}/Library/StdDriver/src

SOURCES := \
	${BSP_DRVSRC}/common.c \
	printf_large.c \
	adc.c \
	debug_log.c \
	console.c \
	lcd.c \
	util.c \
	main.c

OBJECTS := $(notdir $(SOURCES:.c=.rel))

vpath %.c $(dir $(SOURCES))

CC := sdcc --std-c23 -mmcs51 -c --model-large -V -D__SDCC__ -DUSE_FLOATS=1 -I ${BSP_DEVINC} -I ${BSP_DRVINC}

LD := sdcc --std-c23 -mmcs51 --out-fmt-ihx --model-large

%.rel: %.c
	${CC} -c $< -o $@

all: ${OBJECTS}
	${LD} -o ${PROJECT}.ihx ${OBJECTS}

clean:
	rm -rf *.ihx *.lk *.map *.mem *.adb *.asm *.lst *.rel *.rst *.sym
