EE_BIN = jpg.elf
EE_OBJS = jpg.o jpegtest.o
EE_INCS = -I$(LIBJPG)/include
EE_LIBS = -L$(LIBJPG)/lib  -ljpg


all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

include $(GSKIT)/Makefile.pref
include $(GSKIT)/Makefile.global
#include $(PS2SDKSRC)/Defs.make
#include $(PS2SDKSRC)/ee/Rules.make
#include $(PS2SDKSRC)/ee/Rules.release
