/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2006 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *      Simple basic typedefs, isolated here to make it easier
 *       separating modules.
 *
 *-----------------------------------------------------------------------------*/

#ifndef __DOOMTYPE__
#define __DOOMTYPE__

#ifndef __BYTEBOOL__
#define __BYTEBOOL__
/* Fixed to use builtin bool type with C++. */
//#ifndef ndsinclude
//typedef unsigned char byte;
//#endif
#include <nds.h>

//typedef enum { false, true } boolean;
typedef bool qboolean;
typedef int boolean;
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

/* cph - Wrapper for the long long type, as Win32 used a different name.
 * Except I don't know what to test as it's compiler specific
 * Proff - I fixed it */
typedef signed long long int int_64_t;
typedef unsigned long long int uint_64_t;

#undef PATH_MAX
#define PATH_MAX 1024
#undef S_ISDIR
#define S_ISDIR(x) (((sbuf.st_mode & S_IFDIR)==S_IFDIR)?1:0)

#define CONSTFUNC __attribute__((const))
#define PUREFUNC __attribute__((pure))

/* CPhipps - use limits.h instead of depreciated values.h */
#include <limits.h>

/* cph - move compatibility levels here so we can use them in d_server.c */
typedef enum {
  compatibility_null = -1,
  doom_12_compatibility,   /* Doom v1.2 */
  doom_1666_compatibility, /* Doom v1.666 */
  doom2_19_compatibility,  /* Doom & Doom 2 v1.9 */
  ultdoom_compatibility,   /* Doom 2 v1.9 */
  finaldoom_compatibility,     /* Final & Ultimate Doom v1.9, and Doom95 */
  dosdoom_compatibility,     /* Early dosdoom & tasdoom */
  tasdoom_compatibility,     /* Early dosdoom & tasdoom */
  boom_compatibility_compatibility,      /* Boom's compatibility mode */
  boom_201_compatibility,                /* Compatible with Boom v2.01 */
  boom_202_compatibility,                /* Compatible with Boom v2.01 */
  lxdoom_1_compatibility,                /* LxDoom v1.3.2+ */
  mbf_compatibility,                     /* MBF */
  prboom_1_compatibility,                /* PrBoom 2.03beta? */
  prboom_2_compatibility,                /* PrBoom 2.1.0-2.1.1 */
  prboom_3_compatibility,                /* PrBoom 2.2.x */
  prboom_4_compatibility,                /* PrBoom 2.3.x */
  prboom_5_compatibility,                /* PrBoom 2.4.0 */
  prboom_6_compatibility,                /* Latest PrBoom */
  MAX_COMPATIBILITY_LEVEL,               /* Must be last entry */
  /* Aliases follow */
  boom_compatibility = boom_201_compatibility, /* Alias used by G_Compatibility */
  best_compatibility = prboom_6_compatibility,
} complevel_t;

#endif
