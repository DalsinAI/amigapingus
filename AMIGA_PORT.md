Pingus for AmigaOS 3.x
======================

Pingus, the free Lemmings-like puzzle game (https://github.com/Pingus/pingus,
mirror of https://gitlab.com/pingus/pingus), for AmigaOS 3.x on a 68040 or
68060 with an FPU and an RTG screen, on OpenGPU's SDL 2: it needs
LIBS:OpenGPU/SDL2.module, SDL2_image.module and SDL2_mixer.module, which
OpenUp installs.

Upstream's last release (0.7.6, 2011) is SDL 1.2, Boost and SCons; its
master is SDL 2, C++23 and no Boost (libsigc++ for signals). The port is
built from master (0.8.0-dev, commit
ffe3851f1f9f9f5c245887d89ab4a56399c13769 of 29 October 2026).


Playing
-------

Double-click Pingus in its drawer, or from a Shell:

    cd Work:Pingus
    Pingus             ; a 640x480 screen of its own
    Pingus -w          ; in a window on the Workbench
    Pingus -g 800x600  ; another size
    Pingus -s -m       ; no sound, no music

`Pingus -h` lists every option. Mouse: choose an action at the bottom
(or 1 to 0, Tab), click a pingu to give it the action; the small map at
the bottom right scrolls the level, so do the cursor keys and the right
mouse button. Space or F speeds the game up, P pauses, S steps, A is
armageddon, Esc leaves a level (data/controller/default.scm).

Settings, savegames and the level cache are kept in the drawer, in
`user/` (`user/config` holds the settings the Options screen writes).
Data is read from `data/` in the drawer (PROGDIR:data).


What was done
-------------

- Build: amiga/Makefile.amigaos replaces upstream's CMake build: the same
  sources (CMakeLists.txt's PINGUS_SOURCES_CXX) and the helper libraries
  of external/ (argpp, geomcpp, logmich, priocpp, sexpcpp, strutcpp,
  tinygettext) compiled in, C++23, -noixemul, static.
- No OpenGL: the OpenGL framebuffer (src/engine/display/opengl/) is left
  out and `-r opengl` gives the SDL one (PINGUS_NO_OPENGL). The SDL
  framebuffer asks for SDL's software renderer; nothing calls SDL_GL_*,
  so the program does not pull in GL.
- Fullscreen is a screen of the size asked for (640x480 by default), not
  one as big as the Workbench (sdl_framebuffer.cpp).
- Sound: wstsound needs OpenAL and threads; amiga/amiga_sound.cpp plays
  Pingus' WAV sounds and its tracker music (.it, .s3m) with SDL2_mixer
  instead (22 kHz stereo, 16 channels, panning kept).
- libsigc++: upstream's own header-only subset (mk/android/app/jni/sigc++).
- glm (headers only) 1.0.1 is in amiga/deps/glm-1.0.1 (MIT or Happy Bunny
  licence, copying.txt there and in the drawer as glm-copying.txt).
- jsoncpp (priocpp's JSON reader) and xdgcpp are left out: Pingus' data is
  S-expressions.
- Paths: the user directory is PROGDIR:user/; Pathname::join and
  System::create_dir know a volume part ("PROGDIR:") and never write
  "PROGDIR:/" (the parent directory on AmigaDOS). System::write_file writes
  the file in place: AmigaDOS's Rename() does not replace a file.
- Big-endian: 32-bit pictures from SDL2_image are converted to R,G,B,A bytes
  if they are not (Pingus reads alpha from byte 3 for the collision masks
  and the ground); the 24-bit masks of Blitter::create_surface_rgb, the
  24-bit pixel read in Surface::get_pixel and the screenshot's pixel format
  were little-endian only. Surface::blit's "transparent pixel" test read
  the wrong pixel (upstream bug, all systems).
- argpp: no terminal width ioctl.
- Started from the Workbench (argc 0), Pingus gets a program name only.
- amiga/amiga_screens.c (from the Seven Kingdoms port): the empty "SDL"
  screens SDL2.module 0.6 leaves after a fullscreen window are closed at
  exit.
- amiga/pthread_stubs.c: what libstdc++ asks of threads (one thread);
  amiga/amiga_stubs.cpp: statvfs for <filesystem>; amiga/stack.c: 1 MB.
- The drawer's PNGs that are 8-bit RGB without alpha are stored as RGBA
  (amiga/tools/png_rgba.py, from the Starfighter port): SDL2_image.module
  0.6 damages the emulator's heap decoding some of them.


Building
--------

    make -f amiga/Makefile.amigaos -j2
    make -f amiga/Makefile.amigaos dist

The variables (STOVE, NDK, SDL2KIT, GLM) default to the paths in the
Makefile's head: GFOpenAmigaGCC 16.2.0-open4, the OpenNDK and the OpenGPU
kit. The program is build/amigaos/Pingus (build/amigaos/Pingus.debug with
symbols); `dist` makes the drawer dist/Pingus (+ dist/Pingus.info). The
icons are made with amicradle's port_icons.py from amiga/icons/icons.json.
The linker's "duplicate section ... has different contents" warnings come
from libstdc++'s <format> templates and are harmless.


Licences
--------

Pingus' code is GPL 3 or later (COPYING.txt). Its data (pictures, levels,
worldmaps, fonts, sounds, the music by H. Matthew Smith and Joseph Toscano,
the translations) ships in upstream's repository as part of the GPL'd game
and is distributed under the same licence (as Debian's pingus-data: GPL
3+); AUTHORS.txt credits the artists. data/music/README speaks of Ogg
songs by Kev Bailey (CC BY-SA 3.0); they are not in the repository and not
in the drawer. glm is MIT (glm-copying.txt). The port's own files are GPL
3 or later.


Known limits
------------

- Everything is drawn by the 68k through SDL's software renderer: expect
  it to be slow at 640x480 on a 68040, better on a 68060; bigger sizes
  cost more.
- No OpenGL; Pingus has no network play. The level editor is built in
  but untried.
- Not yet tried on a real machine or the lab: see the report for what to
  check.
