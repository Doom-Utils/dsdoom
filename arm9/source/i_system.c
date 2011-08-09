/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
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
 *  Misc system stuff needed by Doom, implemented for Linux.
 *  Mainly timer handling, and ENDOOM/ENDBOOM.
 *
 *-----------------------------------------------------------------------------
 */

#include <nds.h>

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#ifdef _MSC_VER
#define    F_OK    0    /* Check for file existence */
#define    W_OK    2    /* Check for write permission */
#define    R_OK    4    /* Check for read permission */
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

// #include "SDL.h"

#include "config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <io.h>
#endif
#ifdef DREAMCAST
#include <kos/fs.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#endif
#include <errno.h>

#include "i_system.h"
#include "m_argv.h"
#include "lprintf.h"
#include "doomtype.h"
#include "doomdef.h"
#include "lprintf.h"

#ifdef __GNUG__
#pragma implementation "i_system.h"
#endif
#include "i_system.h"

#include "config.h"

#define timers2ms(tlow,thigh) ((tlow>>5)+(thigh<<11))

// Handy DSdev.org timer functions
u32 GetTicks(void)
{
	return timers2ms(TIMER0_DATA, TIMER1_DATA);
} 

void Pause(u32 ms)
{
	u32 now;
	now=timers2ms(TIMER0_DATA, TIMER1_DATA);
	while((u32)timers2ms(TIMER0_DATA, TIMER1_DATA)<now+ms);
}


void I_uSleep(unsigned long usecs)
{
	Pause(usecs/1000);
}

int ms_to_next_tick;

int I_GetTime_RealTime (void)
{
  int t = GetTicks();
  int i = t*(TICRATE/5)/200;
  ms_to_next_tick = (i+1)*200/(TICRATE/5) - t;
  if (ms_to_next_tick > 1000/TICRATE || ms_to_next_tick<1) ms_to_next_tick = 1;
  return i;
}

/*
 * I_GetRandomTimeSeed
 *
 * CPhipps - extracted from G_ReloadDefaults because it is O/S based
 */
unsigned long I_GetRandomTimeSeed(void)
{
/* This isnt very random */
  return(GetTicks());
}

/* cphipps - I_GetVersionString
 * Returns a version string in the given buffer
 */
const char* I_GetVersionString(char* buf, size_t sz)
{
#ifdef HAVE_SNPRINTF
  snprintf(buf,sz,"%s v%s (http://prboom.sourceforge.net/)",PACKAGE,VERSION);
#else
  sprintf(buf,"%s v%s (http://prboom.sourceforge.net/)",PACKAGE,VERSION);
#endif
  return buf;
}

/* cphipps - I_SigString
 * Returns a string describing a signal number
 */
const char* I_SigString(char* buf, size_t sz, int signum)
{
#ifdef SYS_SIGLIST_DECLARED
  if (strlen(sys_siglist[signum]) < sz)
    strcpy(buf,sys_siglist[signum]);
  else
#endif
    sprintf(buf,"signal %d",signum);
  return buf;
}


/* 
 * I_Read
 *
 * cph 2001/11/18 - wrapper for read(2) which handles partial reads and aborts
 * on error.
 */
void I_Read(FILE *fd, void* vbuf, size_t sz)
{

	fread(vbuf,sz,1,fd);
	
/*  unsigned char* buf = vbuf;

  while (sz) {
    int rc = fread(buf,sz,1,fd);
    if (rc <= 0) {
      I_Error("I_Read: read failed: %s", rc ? strerror(errno) : "EOF");
    }
    sz -= rc; buf += rc;
  }
*/
}

/*
 * I_Filelength
 *
 * Return length of an open file.
 */

int I_Filelength(FILE *handle)
{
#ifndef DREAMCAST
  struct stat   fileinfo;
  if (fstat(handle,&fileinfo) == -1)
    I_Error("I_Filelength: %s",strerror(errno));
  return fileinfo.st_size;
#endif  
}

#ifndef PRBOOM_SERVER

// Return the path where the executable lies -- Lee Killough
// proff_fs 2002-07-04 - moved to i_system
//#ifdef _WIN32
#ifndef MACOSX
char *I_DoomExeDir(void)
{
  static const char current_dir_dummy[] = {"."}; // proff - rem extra slash 8/21/03
  static char *base;
  if (!base)        // cache multiple requests
    {
      size_t len = strlen(*myargv);
      char *p = (base = malloc(len+1)) + len - 1;
      strcpy(base,*myargv);
      while (p > base && *p!='/' && *p!='\\')
        *p--=0;
      if (*p=='/' || *p=='\\')
        *p--=0;
      if (strlen(base)<2)
      {
        free(base);
        base = malloc(PATH_MAX);
        if (!getcwd(base,PATH_MAX))
          strcpy(base, current_dir_dummy);
      }
    }
  return base;
}
#else
// cph - V.Aguilar (5/30/99) suggested return ~/.lxdoom/, creating
//  if non-existant
static const char prboom_dir[] = {"/"}; // Mead rem extra slash 8/21/03

#ifdef MACOSX
/* Defined elsewhere */
#else
/*char *I_DoomExeDir(void)
{
  return "/";
}
*/#endif
#endif

/*
 * HasTrailingSlash
 *
 * cphipps - simple test for trailing slash on dir names
 */

static boolean HasTrailingSlash(const char* dn)
{
  return (dn[strlen(dn)-1] == '/');
}

/*
 * I_FindFile
 *
 * proff_fs 2002-07-04 - moved to i_system
 *
 * cphipps 19/1999 - writen to unify the logic in FindIWADFile and the WAD
 *      autoloading code.
 * Searches the standard dirs for a named WAD file
 * The dirs are:
 * .
 * DOOMWADDIR
 * ~/doom
 * /usr/share/games/doom
 * /usr/local/share/games/doom
 * ~
 */

#ifdef MACOSX
/* Defined elsewhere */
#else
char* I_FindFile(const char* wfname, const char* ext)
{
  int   i;
  // Precalculate a length we will need in the loop
  size_t  pl = strlen(wfname) + strlen(ext) + 4;

  for (i=0; i<8; i++) {
    char  * p;
    const char  * d = NULL;
    const char  * s = NULL;
    // Each entry in the switch sets d to the directory to look in,
    // and optionally s to a subdirectory of d
    switch(i) {
    case 1:
      if (!(d = getenv("DOOMWADDIR"))) continue;
    case 0:
      break;
    case 2:
      d = DOOMWADDIR;
      break;
    case 4:
      d = "/usr/share/games/doom";
      break;
    case 5:
      d = "/usr/local/share/games/doom";
      break;
    case 6:
      d = I_DoomExeDir();
      break;
    case 3:
      s = "doom";
    case 7:
      if (!(d = getenv("HOME"))) continue;
      break;
#ifdef SIMPLECHECKS
    default:
      I_Error("FindWADFile: Internal failure");
#endif
    }

    p = malloc((d ? strlen(d) : 0) + (s ? strlen(s) : 0) + pl);
    sprintf(p, "%s%s%s%s%s", d ? d : "", (d && !HasTrailingSlash(d)) ? "/" : "",
                             s ? s : "", (s && !HasTrailingSlash(s)) ? "/" : "",
                             wfname);

    if (access(p,F_OK))
      strcat(p, ext);
    if (!access(p,F_OK)) {
      lprintf(LO_INFO, " found %s\n", p);
      return p;
    }
    free(p);
  }
  return NULL;
}

#endif

#endif // PRBOOM_SERVER
