// license:BSD-3-Clause
/*
 * libretro-same: settings of each single-system core.
 *
 * `make -f Makefile.libretro SAME_CORE=<name>` builds only the MAME sources listed for
 * <name> in Makefile.libretro and defines SAME_CORE_<name>, which selects one entry here.
 * Without SAME_CORE nothing is defined and the build is the regular MAME core.
 *
 * Each entry defines:
 *   CORE_NAME          core name: option keys, system/ and saves/ folders, file name
 *   SAME_LIBRARY_NAME  name shown by the frontend
 *   SAME_SYSTEM        MAME machine to run
 *   SAME_MEDIA         MAME media option the content is loaded with (-cart, -cass, -flop...)
 *   SAME_EXTENSIONS    content extensions accepted by the frontend
 */

#ifndef SAME_CORES_H
#define SAME_CORES_H

#if defined(SAME_CORE_a7800)
#define CORE_NAME          "same_a7800"
#define SAME_LIBRARY_NAME  "SAME_A7800"
#define SAME_SYSTEM        "a7800"
#define SAME_MEDIA         "-cart"
#define SAME_EXTENSIONS    "a78|bin"

#elif defined(SAME_CORE)
#error "Unknown SAME_CORE_<name>: add an entry to same_cores.h"
#endif

#endif
