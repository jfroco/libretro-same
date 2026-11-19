// license:BSD-3-Clause
/*
 * libretro-same: NTSC or PAL machine, from the "Region" core option.
 *
 * MAME has one machine per region (a7800 / a7800p) and it is chosen before the machine
 * starts, so a change of the option takes effect the next time the content is loaded.
 *
 *   auto  same_a7800: byte 57 of the .a78 header (1 = PAL, anything else = NTSC). For a
 *         file without header, the region of the No-Intro table (bus/a7800/a78_nointro.h).
 *         A file that is neither runs as NTSC.
 *   ntsc  SAME_SYSTEM, whatever the header says.
 *   pal   SAME_SYSTEM_PAL, whatever the header says.
 *
 * Usage (retro_init.cpp): same_system_name(content path) instead of SAME_SYSTEM.
 */

#ifndef SAME_REGION_H
#define SAME_REGION_H

#ifdef SAME_SYSTEM_PAL

#include "corefile.h"
#include "hashing.h"

#ifdef SAME_CORE_a7800
#include "bus/a7800/a78_nointro.h"
#endif

#include <cstring>
#include <vector>

static bool same_content_is_pal(const char *path)
{
#ifdef SAME_CORE_a7800
   std::vector<uint8_t> data;
   if (util::core_file::load(path, data) || data.size() < 128)
      return false;

   if (!memcmp(&data[1], "ATARI7800", 9))
      return data[57] == 1;

   uint32_t const crc = util::crc32_creator::simple(data.data(), data.size());
   for (const a78_nointro_entry &entry : a78_nointro)
      if (entry.crc == crc)
         return entry.pal == 1;
#endif
   return false;
}

static const char *same_system_name(const char *path)
{
   if (!strcmp(same_region, "pal"))
      return SAME_SYSTEM_PAL;
   if (!strcmp(same_region, "ntsc"))
      return SAME_SYSTEM;
   return same_content_is_pal(path) ? SAME_SYSTEM_PAL : SAME_SYSTEM;
}

#endif // SAME_SYSTEM_PAL

#endif
