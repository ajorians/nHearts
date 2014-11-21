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

#Because JGit does not do symbolic links and TeamCity uses JGit
prepare:
	mkdir -p include
	mkdir -p include/ArchiveLib
	mkdir -p include/CardLib
	mkdir -p include/HeartsLib
	mkdir -p include/HeartsAI
	mkdir -p lib
	ln -s ../../ArchiveLib/include/ArchiveLib.h include/ArchiveLib/ArchiveLib.h
	ln -s ../ArchiveLib/libArchive.a lib/libArchive.a
	ln -s ../../CardLib/include/CardLib.h include/CardLib/CardLib.h
	ln -s ../CardLib/libCards.a lib/libCards.a
	ln -s ../../HeartsLib/include/HeartsLib.h include/HeartsLib/HeartsLib.h
	ln -s ../HeartsLib/libHearts.a lib/libHearts.a
	ln -s ../../HeartsAI/include/HeartsAI.h include/HeartsAI/HeartsAI.h
	ln -s ../HeartsAI/libHeartsAI.a lib/libHeartsAI.a

%.o: %.cpp
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $@

clean:
	rm -rf include
	rm -rf lib
	rm -f *.o *.elf
	rm -f $(EXE)

