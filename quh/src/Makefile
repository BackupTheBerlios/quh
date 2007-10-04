.PHONY: all clean distclean install uninstall

CC=gcc
CFLAGS=-I. -Wall -W -O3 -DHAVE_CONFIG_H
LDFLAGS=-s

include config.mak


LINK_COMPILER=$(CC)
TARGET=quh
OBJECTS=quh.o quh_misc.o quh_filter.o \
	misc/cache.o misc/base64.o \
	misc/file.o misc/filter.o misc/getopt.o misc/getopt2.o \
	misc/misc.o misc/property.o misc/string.o misc/term.o \
	filter/cache.o filter/console.o filter/decode.o filter/demux.o \
	filter/raw.o filter/read.o filter/wav.o \
	filter/stdout.o
#	filter/fade.o \
#	filter/lyrics.o \
#	filter/cdxa.o \
#	filter/nsf.o \
#	filter/gbs.o \
#	filter/gsf.o \
#	filter/hes.o \
#	filter/psf.o \
#	filter/usf.o \
#	filter/vgm.o 
ifdef USE_JOYSTICK
OBJECTS+=filter/joy.o misc/event.o misc/event_joystick.o
endif
ifdef USE_SPC
OBJECTS+=filter/spc.o
endif
ifdef USE_ID3
OBJECTS+=filter/id3.o
endif
ifdef USE_MAD
OBJECTS+=filter/mad.o filter/mad/xing.o
endif
ifdef USE_OGG
OBJECTS+=filter/vorbis.o
endif
ifdef USE_FLAC
OBJECTS+=filter/flac.o
endif
ifdef USE_APE
OBJECTS+=filter/ape.o
endif
ifdef USE_SNDFILE
OBJECTS+=filter/sndfile.o
endif
ifdef USE_XMP
OBJECTS+=filter/xmp.o
endif
ifdef USE_SID
OBJECTS+=filter/sid.o
LINK_COMPILER=$(CPP)
endif
ifdef USE_AUDIOFILE
OBJECTS+=filter/audiofile.o
endif
ifdef USE_MIKMOD
OBJECTS+=filter/mikmod.o
endif
ifdef USE_FESTIVAL
OBJECTS+=filter/festival.o
endif
ifdef USE_OSS
OBJECTS+=filter/oss.o
endif
ifdef USE_PCSPKR
OBJECTS+=filter/speaker.o
endif
ifdef USE_ALSA
OBJECTS+=filter/alsa.o
endif
ifdef USE_SDL
OBJECTS+=filter/sdl.o #misc/audio.o misc/audio_sdl.o
endif
ifdef USE_ESD
OBJECTS+=filter/esd.o
endif
ifdef USE_LIBAO
OBJECTS+=filter/ao.o
endif
ifdef USE_LIBMPEG
OBJECTS+=filter/mpeg.o
endif
ifdef USE_SNDLIB
OBJECTS+=filter/sndlib.o
endif
ifdef USE_ST
//OBJECTS+=filter/echo.o
endif
ifdef USE_FFMPEG 
OBJECTS+=filter/ffmpeg.o
endif
ifdef USE_GAIM
OBJECTS+=filter/gaim.o
endif
ifdef USE_WAVPACK
OBJECTS+=filter/wavpack.o
endif
ifdef USE_ZIP
CFLAGS+=-DUSE_GYM
OBJECTS+=filter/gym.o filter/gym/sn76496.o filter/gym/ym2151.o filter/gym/ym2612.o
OBJECTS+=misc/unzip.o misc/zip.o
endif
#ifdef USE_TCP
#CFLAGS+=-DUSE_SERVER
#OBJECTS+=filter/server.o
#endif
ifdef USE_TCP
OBJECTS+=misc/net.o
endif
ifdef USE_CDDA
OBJECTS+=filter/cdda.o
endif
ifdef USE_TCP
CFLAGS+=-DUSE_CDDB
OBJECTS+=filter/cddb.o
endif
ifdef USE_AL
OBJECTS+=filter/al.o
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


install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	cp -vf $(TARGET) $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

# Dependencies

# Most source files include these
QUH_STD_H=quh.h misc/misc.h config.h misc/getopt.h #quh_defines.h

misc/getopt.o: misc/getopt.h
quh.o: quh.h $(QUH_STD_H)
quh_misc.o: quh_misc.h $(QUH_STD_H)
quh_opts.o: quh_opts.h $(QUH_STD_H)
misc/misc.o: misc/misc.h $(QUH_STD_H)
