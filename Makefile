PROJECT := deek_robot

NUVOTON_ROOT=/extrahd1/mydata/projects/nuvoton

BSPROOT := ${NUVOTON_ROOT}/N76E003_BSP
BSP_DEVINC := ${BSPROOT}/Library/Device/Include
BSP_DRVINC := ${BSPROOT}/Library/StdDriver/inc
BSP_DRVSRC := ${BSPROOT}/Library/StdDriver/src

SOURCES := \
	${BSP_DRVSRC}/common.c \
	${BSP_DRVSRC}/sys.c \
	${BSP_DRVSRC}/uart.c \
	${BSP_DRVSRC}/sdcc_stdio.c \
	main.c

OBJECTS := $(notdir $(SOURCES:.c=.rel))

vpath %.c $(dir $(SOURCES))

CC := sdcc -c --model-small --debug -V -D__SDCC__ -I ${BSP_DEVINC} -I ${BSP_DRVINC}

LD := sdcc -mmcs51 --out-fmt-ihx --model-small

%.rel: %.c
	${CC} -c $< -o $@

all: ${OBJECTS}
	${LD} -o ${PROJECT}.ihx ${OBJECTS}

clean:
	rm -rf *.ihx *.lk *.map *.mem *.adb *.asm *.lst *.rel *.rst *.sym
