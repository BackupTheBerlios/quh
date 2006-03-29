.PHONY: all clean distclean install uninstall

CFLAGS=-I. -Wall -W -O3 -DHAVE_CONFIG_H
LDFLAGS=-s

include config.mak

LINK_COMPILER=$(CC)
TARGET=quh
OBJECTS=quh.o quh_misc.o quh_filter.o \
	misc/audio.o misc/cache.o  misc/event.o misc/file.o misc/filter.o \
	misc/getopt.o misc/getopt2.o misc/misc.o misc/net.o \
	misc/property.o misc/string.o misc/term.o \
	filter/decode.o filter/stdin.o filter/joy.o filter/pipe.o \
	filter/cache.o filter/echo.o filter/console.o filter/raw.o \
	filter/vol.o filter/stdout.o filter/wav.o \
	filter/id3.o filter/read.o filter/demux.o filter/cdda.o \
	filter/cddb.o filter/lyrics.o
ifdef USE_MAD
OBJECTS+=filter/libmad.o
OBJECTS+=filter/xing.o
endif
ifdef USE_OGG
OBJECTS+=filter/vorbis.o
endif
ifdef USE_FLAC
OBJECTS+=filter/flac.o
endif
ifdef USE_SNDFILE
OBJECTS+=filter/sndfile.o
endif
ifdef USE_XMP
OBJECTS+=filter/libxmp.o
endif
ifdef USE_SID
OBJECTS+=filter/sid.o
LINK_COMPILER=$(CPP)
endif
ifdef USE_AUDIOFILE
OBJECTS+=filter/audiofile.o
endif
ifdef USE_MIKMOD
OBJECTS+=filter/libmikmod.o
endif
ifdef USE_FLITE
OBJECTS+=filter/libflite.o
endif
ifdef USE_OSS
OBJECTS+=filter/oss.o
endif
ifdef USE_PCSPEAKER
OBJECTS+=filter/speaker.o
endif
ifdef USE_ALSA
OBJECTS+=filter/alsa.o
endif
ifdef USE_SDL
OBJECTS+=filter/sdl.o
endif
ifdef USE_ESD
OBJECTS+=filter/esd.o
endif
ifdef USE_LIBAO
OBJECTS+=filter/libao.o
endif
ifdef USE_LIBMPEG
OBJECTS+=filter/libmpeg.o
endif
ifdef USE_SNDLIB
OBJECTS+=filter/sndlib.o
endif
ifdef USE_ST
OBJECTS+=filter/libst.o
endif
ifdef USE_FFMPEG 
OBJECTS+=filter/ffmpeg.o
endif


all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS) *.core *.stackdump DEADJOE */DEADJOE *~ */*~ */*/*~

distclean: clean
	rm -f config.mak config.log config.status config.cache config.h

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@


.cpp.o:
	$(CPP) $(CFLAGS) -c $< -o $@


$(TARGET): $(OBJECTS)
	$(LINK_COMPILER) $(OBJECTS) $(LDFLAGS) -o $@


install:
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Dependencies

# Most source files include these
QUH_STD_H=quh.h misc/misc.h config.h misc/getopt.h

misc/getopt.o: misc/getopt.h
quh.o: quh.h $(QUH_STD_H)
quh_misc.o: quh_misc.h $(QUH_STD_H)
quh_opts.o: quh_opts.h $(QUH_STD_H)
misc/misc.o: misc/misc.h $(QUH_STD_H)
