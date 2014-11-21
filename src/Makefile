GCC = nspire-g++
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm -Wno-unused-parameter -I include
LDFLAGS = -L lib -lndls -lHearts -lHeartsAI -lCards -lArchive
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = nHearts-sdl.tns
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
vpath %.tns $(DISTDIR)

all:	prepare $(EXE)

prepare:        
	cd ArchiveLib; make; cd ..
	cd CardLib; make; cd ..
	cd HeartsLib; make; cd ..
	cd HeartsAI; make; cd ..

%.o: %.cpp
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $@

clean:
	cd ArchiveLib; make clean
	cd CardLib; make clean
	cd HeartsLib; make clean
	cd HeartsAI; make clean
	rm -rf lib
	rm -f *.o *.elf
	rm -f $(EXE)

