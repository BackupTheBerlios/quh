#ifdef  USE_NETGUI
  if (!quh_load_netgui())
    return -1;
#endif
#ifdef  USE_NETGUI
      case QUH_GUI:
        if (optarg)
          {
            libng_gui (optarg);
            exit (0);
          }
#endif

#ifdef  USE_NETGUI
  if (quh.netgui) 
    ng_close (quh.netgui);
#endif

#ifdef  USE_NETGUI
#include <libnetgui.h>
extern int libng_gui (const char *optarg);
extern int quh_load_netgui (void);
extern const st_usage_t libng_usage[];
#endif

#ifdef  USE_NETGUI
  netgui_t *netgui;
#endif



const st_getopt2_t quh_filter_write_usage[] =
{
  {
    NULL, 0, 0, 0,
    NULL, "Filter", NULL
  },
  {
    "example", 0, 0, QUH_EXAMPLE,
    NULL, "example filter that does nothing", (void *) FILTER_EXAMPLE
  },
#ifdef  USE_NETGUI
  {
    "gui", 0, 0, QUH_GUI,
    "URL", "start httpd with host and port from URL;\n"
    "quh: "OPTION_LONG_S "gui localhost:8081\n"
    "or just start 'quhgui' for a non-httpd GUI", (void *) FILTER_GUI
  },
#endif  // USE_NETGUI
//    "(default: enable build-in Osci and Spectrum analyzer)",
  {
    "ctrl", 0, 0, QUH_CTRL,
    NULL, "enable control keys (default: enabled)", (void *) FILTER_CTRL
  },
  {
    "progress", 0, 0, QUH_PROGRESS,
    NULL, "enable progress display (default: enabled)", (void *) FILTER_PROGRESS
  },
  {
    "joy", 2, 0, QUH_JOY,
    "DEVICE", "enable native joystick DEVICE control (default: /dev/js0)", (void *) FILTER_JOY
  },
#ifdef  USE_SDL
  {
    "joysdl", 2, 0, QUH_JOY,
    "N", "enable joystick N control using SDL (default: 0)", (void *) FILTER_JOYSDL
  },
#endif  // USE_SDL
  {
    "lirc", 2, 0, QUH_LIRC,
    "DEVICE", "enable remote control (default: /dev/video0)", (void *) FILTER_LIRC
  },
  {
    "convert", 1, 0, QUH_CONVERT,
    "FREQ[:BIT[:SIG[:SWAP]]]",
    "\nconvert INPUT to FREQuenzy, BITs, SIGned and SWAP bytes\n"
    "FREQ=N   set FREQuenzy to N Hz\n"
    "FREQ=def default from INPUT\n"
    "BIT=N    set to N BITs\n"
    "BIT=def  default from INPUT\n"
    "SIG=0    is unSIGned\n"
    "SIG=1    is SIGned\n"
    "SIG=def  default from INPUT\n"
    "SWAP=1   SWAP bytes (depending on BIT size)\n"
    "SWAP=def default from INPUT\n"
    OPTION_LONG_S "convert 44100:def:0 would change FREQ and SIG only",
    (void *) FILTER_CONVERT
  },
#if 0
  {
    "pitch", 1, 0, QUH_PITCH,
    "N",   "pitchs the output by factor N", (void *) FILTER_PITCH
  },
#endif
  {
    "raw", 2, 0, QUH_RAW,
    "FILE", "write as raw (1:1) FILE (default: audiodump.raw)", (void *) FILTER_RAW
  },
  {
    "wav", 2, 0, QUH_WAV,
    "FILE", "write as wav FILE (default: audiodump.wav)", (void *) FILTER_WAV
  },
#if 0
  {
    "img", 2, 0, QUH_IMG,
    "MB", "write as CD-DA image(s) with MB size and cue sheet\n"
    "(default: 700)", (void *) FILTER_IMG
  },
#endif
  {
    "stdout", 0, 0, QUH_STDOUT,
    NULL, "write to stdout", (void *) FILTER_STDOUT
  },
  {
    "stderr", 0, 0, QUH_STDERR,
    NULL, "write to stderr", (void *) FILTER_STDERR
  },
  {
    "pipe", 2, 0, QUH_PIPE,
    "PATH", "pipe through external FILTER with PATH as filename",
#if 0
    "WinAmp and QBase/VST plug-ins are detected and\n"
    "supported automatically",
#endif
    (void *) FILTER_PIPE
  },
  {
    "set", 1, 0, QUH_SET,
    "FREQ[:BIT[:SIG[:SWAP]]]", "\nlike " OPTION_LONG_S "convert but changes not the INPUT",
    (void *) FILTER_SET
  },
// TODO: OUT=... dynamical
  {
    "libao", 2, 0, QUH_LIBAO,
    "N[:OUT]", "write to soundcard N using libao (default: 0)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_LIBAO
  },
  {
    "alsa", 2, 0, QUH_ALSA,
    "DEVICE[:OUT]", "write to soundcard DEVICE using ALSA (default: /dev/dsp)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_ALSA
  },
  {
    "oss", 2, 0, QUH_OSS,
    "DEVICE[:OUT]", "write to soundcard DEVICE using OSS (default: /dev/dsp)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_OSS
  },
#ifdef  USE_SDL
  {
    "sdl", 2, 0, QUH_SDL,
    "N[:OUT]", "write to soundcard N using SDL (default: 0)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_SDL
  },
#endif  // USE_SDL
  {
    "speaker", 0, 0, QUH_SPEAKER,
    NULL, "write to build-in PC speaker (if present)", (void *) FILTER_SPEAKER
  },
#if 0
  {
    "af", 1, 0, QUH_AF,
    "FILTER[:OPTION]", "same as " OPTION_LONG_S "FILTER[:OPTION]", NULL
  },
  {
    "ao", 1, 0, QUH_AO,
    "FILTER[:OPTION]", "same as " OPTION_LONG_S "af", NULL
  },
#endif
#if 0
  {
    "killefit", 2, 0, QUH_PIPE,
    "PATH", "like " OPTION_LONG_S "pipe", (void *) FILTER_PIPE
  },
#endif
#if 0
// KILLEFIT
  {
    "osci", 0, 0, QUH_OSCI,
    NULL, "enable oscilloscope", (void *) FILTER_OSCI
  },
  {
    "spec", 0, 0, QUH_SPEC,
    NULL, "enable spectrum analyzer", (void *) FILTER_SPEC
  },
#endif
#if 0
  {
    "winamp", 1, 0, QUH_PIPE,
    "PATH", "pipe through an WinAmp plug-in with PATH as filename", (void *) FILTER_WINAMP
  },
  {
    "vst", 1, 0, QUH_PIPE,
    "PATH", "pipe through an QBase/VST plug-in with PATH as filename", (void *) FILTER_VST
  },
#endif
  {NULL, 0, 0, 0, NULL, NULL, NULL}
};
