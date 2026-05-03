dnl Process this file with autoconf to produce a configure script.
dnl Copyright (c) 1999-2026 Philip Kendall

dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License along
dnl with this program; if not, write to the Free Software Foundation, Inc.,
dnl 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
dnl
dnl Author contact information:
dnl
dnl E-mail: philip-fuse@shadowmagic.org.uk

dnl Package version
m4_define([libspectrum_version], [1.6.0])

dnl Product full version
m4_define([libspectrum_major_version], [1])
m4_define([libspectrum_minor_version], [6])
m4_define([libspectrum_micro_version], [0])
m4_define([libspectrum_nano_version],  [0])
m4_define([libspectrum_full_version],
          [libspectrum_major_version.libspectrum_minor_version.libspectrum_micro_version.libspectrum_nano_version])
m4_define([libspectrum_rc_version],
          [libspectrum_major_version,libspectrum_minor_version,libspectrum_micro_version,libspectrum_nano_version])

dnl Package info
m4_define([libspectrum_copyright], ["(c) 1999-2026 Philip Kendall and others"])
m4_define([libspectrum_url],
          [https://fuse-emulator.sourceforge.net/libspectrum.php])
m4_define([libspectrum_bugreport],
          [https://sourceforge.net/p/fuse-emulator/bugs/])

AC_INIT([libspectrum],[libspectrum_version],[libspectrum_bugreport],
        [libspectrum],[libspectrum_url])
AC_CONFIG_SRCDIR([libspectrum.c])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST

PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

dnl Use automake to produce `Makefile.in'
AM_INIT_AUTOMAKE([1.11 subdir-objects])
AM_SILENT_RULES([yes])

dnl Substitutions for .in files
LIBSPECTRUM_FULL_VERSION=libspectrum_full_version
LIBSPECTRUM_COPYRIGHT=libspectrum_copyright
LIBSPECTRUM_URL=libspectrum_url
AC_SUBST(LIBSPECTRUM_FULL_VERSION)
AC_SUBST(LIBSPECTRUM_COPYRIGHT)
AC_SUBST(LIBSPECTRUM_URL)

dnl Definitions for config.h
AC_DEFINE([LIBSPECTRUM_RC_VERSION], [libspectrum_rc_version],
          [Define version information for win32 dll library])
AC_DEFINE([LIBSPECTRUM_COPYRIGHT], [libspectrum_copyright],
          [Define copyright of libspectrum])

dnl Checks for programs.
AC_PROG_CC

dnl Setup for compiling build tools (make-perl)
if test $cross_compiling = yes; then
  AC_CHECK_PROGS(CC_FOR_BUILD, gcc cc)
else
  CC_FOR_BUILD=$CC
fi
AC_MSG_CHECKING([for a C compiler for build tools])
AC_MSG_RESULT([$CC_FOR_BUILD])
AC_SUBST(CC_FOR_BUILD)

LT_INIT
AC_PATH_PROG(PERL, perl)
AC_SUBST(PERL)

dnl Checks for header files.
AC_CHECK_HEADERS(stdint.h strings.h unistd.h)

dnl Check for host specific programs
WINDRES_OBJ=
WINDRES_LDFLAGS=
case "$host_os" in
  mingw32*)
    AC_CHECK_TOOL([WINDRES], [windres], [no])
    if test "$WINDRES" != no; then
      WINDRES_OBJ="windres.o"
      WINDRES_LDFLAGS="-Xlinker windres.o"
    fi
    AC_DEFINE([WIN32_LEAN_AND_MEAN],, [Exclude rarely used stuff from Windows headers <windows.h>])
    ;;
esac
AC_SUBST(WINDRES_OBJ)
AC_SUBST(WINDRES_LDFLAGS)

dnl Check for big endianness
AC_C_BIGENDIAN

dnl Check for functions
AC_CHECK_FUNCS(_snprintf _stricmp _strnicmp snprintf strcasecmp strncasecmp)

dnl Allow the user to say that various libraries are in one place
AC_ARG_WITH(local-prefix,
[  --with-local-prefix=PFX local libraries installed in PFX (optional)],
CPPFLAGS="$CPPFLAGS -I$withval/include"; LDFLAGS="$LDFLAGS -L$withval/lib",
if test "$prefix" != "NONE"; then
  CPPFLAGS="$CPPFLAGS -I$prefix/include"; LDFLAGS="$LDFLAGS -L$prefix/lib"
fi)

dnl Check whether to use zlib (the UNIX version is called z, Win32 zdll)
AC_MSG_CHECKING(whether to use zlib)
AC_ARG_WITH(zlib,
[  --without-zlib          don't use zlib],
if test "$withval" = no; then zlib=no; else zlib=yes; fi,
zlib=yes)
AC_MSG_RESULT($zlib)
have_zlib="no"
if test "$zlib" = yes; then
  AC_CHECK_HEADERS(
    zlib.h,
    [AC_SEARCH_LIBS(compress2, z zdll)
     have_zlib="yes"]
  )
fi
AM_CONDITIONAL([HAVE_ZLIB], [test "$ac_cv_header_zlib_h" = yes])

dnl Check whether to use libgcrypt
AC_MSG_CHECKING(whether to use libgcrypt)
AC_ARG_WITH(libgcrypt,
[  --without-libgcrypt     don't use libgcrypt],
if test "$withval" = no; then libgcrypt=no; else libgcrypt=yes; fi,
libgcrypt=yes)
AC_MSG_RESULT($libgcrypt)
have_libgcrypt="no"
if test "$libgcrypt" = yes; then
  AC_CHECK_HEADERS(gcrypt.h,
  [LIBS="$LIBS -lgcrypt"
   have_libgcrypt="yes"])
fi

dnl Check whether to use libbz2 (1.0 or greater)
AC_MSG_CHECKING(whether to use libbz2)
AC_ARG_WITH(bzip2,
[  --without-bzip2         don't use libbz2],
if test "$withval" = no; then bzip2=no; else bzip2=yes; fi,
bzip2=yes)
AC_MSG_RESULT($bzip2)
have_bzip2="no"
if test "$bzip2" = yes; then
  AC_CHECK_HEADER(
    bzlib.h,
    [AC_CHECK_LIB(bz2,BZ2_bzDecompressInit)
     have_bzip2="yes"]
  )
fi

dnl Either find GLib or use the replacement
AC_MSG_CHECKING(whether to use internal GLib replacement)
AC_ARG_WITH(fake-glib,
[  --with-fake-glib        use internal GLib replacement],
if test "$withval" = no; then myglib=no; else myglib=yes; fi,
myglib=no)
AC_MSG_RESULT($myglib)
AS_IF([test "$myglib" = no], [
  PKG_CHECK_MODULES(
    GLIB,
    glib-2.0,
    AC_DEFINE([HAVE_LIB_GLIB], 1, [Defined if we've got GLib]),
    AC_MSG_ERROR([GLib not found])
  )
])
stdatomic_available=no
AS_IF([test "$myglib" = yes], [
  AC_MSG_CHECKING([whether stdatomic.h provides usable atomics])
  AC_COMPILE_IFELSE([
    AC_LANG_PROGRAM([[#include <stdatomic.h>]], [[
      static atomic_char lock_var = 0;
      char desired = 1;
      char expected = 0;
      return atomic_compare_exchange_strong( &lock_var, &expected, 1 ) ? 0 : 1;
    ]])
  ], [
    stdatomic_available=yes
    AC_DEFINE([HAVE_STDATOMIC_H], [1],
              [Defined if stdatomic.h provides usable atomics])
  ])
  AC_MSG_RESULT([$stdatomic_available])
])

AM_CONDITIONAL(USE_MYGLIB, test "$myglib" = yes)
AM_CONDITIONAL(HAVE_LOCK, test "$stdatomic_available" = yes)

dnl If it appears we're using gcc as our compiler, turn on warnings
if test "$ac_cv_c_compiler_gnu" = yes; then
  CFLAGS="$CFLAGS -Wall -Wwrite-strings"
  dnl And possibly lots of warnings
  AC_MSG_CHECKING(whether lots of warnings requested)
  AC_ARG_ENABLE(warnings,
  [  --enable-warnings       give lots of warnings if using gcc],
  if test "$enableval" = yes; then
    warnings=yes;
  else
    warnings=no;
  fi,
  warnings=no)
  AC_MSG_RESULT($warnings)
  if test "$warnings" = yes; then
      CFLAGS="$CFLAGS -Wstrict-prototypes -Wmissing-prototypes -Winline -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Waggregate-return -W -Wsign-compare"
  fi
fi

dnl Check whether to use libaudiofile
AC_MSG_CHECKING(whether to use libaudiofile)
AC_ARG_WITH(libaudiofile,
[  --without-libaudiofile  don't use libaudiofile],
if test "$withval" = no; then libaudiofile=no; else libaudiofile=yes; fi,
libaudiofile=yes)
AC_MSG_RESULT($libaudiofile)

AC_ARG_WITH(wav-backend,
[  --with-wav-backend=auto|audiofile|macos|none  select WAV backend],
[wav_backend_requested="$withval"],
[wav_backend_requested="auto"])

AS_CASE([$wav_backend_requested],
  [auto|audiofile|macos|none], [],
  [AC_MSG_FAILURE([invalid WAV backend '$wav_backend_requested'])])

have_libaudiofile="no"
AS_IF([test "$libaudiofile" = yes], [
  PKG_CHECK_MODULES(
    AUDIOFILE,
    audiofile,
    [AC_DEFINE([HAVE_LIB_AUDIOFILE], 1, [Defined if we've got audiofile])
     have_libaudiofile="yes"],
    [true]
  )
  if test -z "$AUDIOFILE_LIBS"; then
    AM_PATH_AUDIOFILE(
      0.2.3,
      [AC_DEFINE([HAVE_LIB_AUDIOFILE], 1, [Defined if we've got audiofile])
       have_libaudiofile="yes"],
    )
  fi
])

dnl Check for native macOS frameworks for WAV support
MACOS_WAV_LIBS=
have_macos_wav="no"
AS_CASE([$host_os], [darwin*], [
  AC_MSG_CHECKING([for native macOS WAV frameworks])
  save_LIBS="$LIBS"
  LIBS="$LIBS -framework AudioToolbox -framework CoreFoundation"
  AC_LINK_IFELSE([
    AC_LANG_PROGRAM([
      [#include <AudioToolbox/AudioToolbox.h>
        #include <CoreFoundation/CoreFoundation.h>]
    ], [
      ExtAudioFileRef file = NULL;
      CFURLRef url = NULL;
      return ExtAudioFileOpenURL( url, &file );
    ])
  ], [
    AC_DEFINE([HAVE_MACOS_NATIVE_WAV], 1,
              [Defined if native macOS WAV frameworks are available])
    MACOS_WAV_LIBS="-framework AudioToolbox -framework CoreFoundation"
    have_macos_wav="yes"
  ])
  LIBS="$save_LIBS"
  AC_MSG_RESULT([$have_macos_wav])
])
AC_SUBST(MACOS_WAV_LIBS)

WAV_BACKEND_CPPFLAGS=
WAV_BACKEND_LIBADD=
WAV_BACKEND_LDFLAGS=
WAV_BACKEND_PRIVATE_LIBS=

selected_wav_backend="none"
AS_CASE([$wav_backend_requested],
  [auto], [
    AS_CASE([$host_os],
      [darwin*], [
        if test "$have_macos_wav" = yes; then
          selected_wav_backend="macos"
        elif test "$have_libaudiofile" = yes; then
          selected_wav_backend="audiofile"
        fi
      ], [
        if test "$have_libaudiofile" = yes; then
          selected_wav_backend="audiofile"
        fi
      ])
  ],
  [audiofile], [
    if test "$have_libaudiofile" = yes; then
      selected_wav_backend="audiofile"
    else
      AC_MSG_FAILURE([libaudiofile WAV backend requested but unavailable])
    fi
  ],
  [macos], [
    if test "$have_macos_wav" = yes; then
      selected_wav_backend="macos"
    else
      AC_MSG_FAILURE([macOS WAV backend requested but unavailable])
    fi
  ])

have_wav_backend="no"
AS_CASE([$selected_wav_backend],
  [audiofile], [
    AC_DEFINE([HAVE_WAV_BACKEND], 1, [Defined if a WAV backend is available])
    AC_DEFINE([WAV_INTERNAL_AUDIOFILE], 1,
              [Defined if the libaudiofile WAV internals are selected])
    WAV_BACKEND_CPPFLAGS="$AUDIOFILE_CFLAGS"
    WAV_BACKEND_LIBADD="$AUDIOFILE_LIBS"
    WAV_BACKEND_PRIVATE_LIBS="$AUDIOFILE_LIBS"
    have_wav_backend="yes"
  ],
  [macos], [
    AC_DEFINE([HAVE_WAV_BACKEND], 1, [Defined if a WAV backend is available])
    AC_DEFINE([WAV_INTERNAL_MACOS], 1,
              [Defined if the macOS WAV internals are selected])
    WAV_BACKEND_LDFLAGS="$MACOS_WAV_LIBS"
    WAV_BACKEND_PRIVATE_LIBS="$MACOS_WAV_LIBS"
    have_wav_backend="yes"
  ])

AC_SUBST(WAV_BACKEND_CPPFLAGS)
AC_SUBST(WAV_BACKEND_LIBADD)
AC_SUBST(WAV_BACKEND_LDFLAGS)
AC_SUBST(WAV_BACKEND_PRIVATE_LIBS)

AM_CONDITIONAL([WAV_SUPPORT], [test "$have_wav_backend" = yes])
AM_CONDITIONAL([WAV_INTERNAL_AUDIOFILE],
               [test "$selected_wav_backend" = audiofile])
AM_CONDITIONAL([WAV_INTERNAL_MACOS],
               [test "$selected_wav_backend" = macos])

AC_CONFIG_FILES([
  Makefile
  libspectrum.qpg
  libspectrum.pc
])

AC_OUTPUT

dnl Status summary
echo ""
echo "*******************************************"
echo "*** libspectrum is ready to be compiled ***"
echo "*******************************************"
echo ""
echo "zlib support: $have_zlib"
echo "bzip2 support: $have_bzip2"
echo "libgcrypt support: $have_libgcrypt"
echo "libaudiofile support: $have_libaudiofile"
echo "native macOS WAV support: $have_macos_wav"
echo "selected WAV backend: $selected_wav_backend"
echo "Internal GLib replacement: $myglib"
echo ""
echo "Type 'make' to compile libspectrum"
echo ""
