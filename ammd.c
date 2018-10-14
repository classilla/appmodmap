/* Copyright 2018 Cameron Kaiser.
   All rights reserved.
   BSD license. */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

uint32_t bitset;
struct sigaction action;

#define MAX_STRING_SIZE 1024
char currentmap[MAX_STRING_SIZE];
char path[MAX_STRING_SIZE], basepath[MAX_STRING_SIZE];

typedef struct mapping {
  char *wclass;
  unsigned int statemask;
} mappings;

#ifndef USER_CONFIG
#warning Creating daemon with no user configuration settings.
#else
#include USER_CONFIG
#endif

void update_keymappings_for_bits(uint32_t newbitset) {
  uint32_t i, j = 1;
  int l;

  if (newbitset == bitset) return; /* nothing to do */

  /* iterate through the new bitset, running do and undo scripts
     as needed to equal the new bit state */
  for (i=0; i<32; i++, j<<=1) {
    if ((bitset & j) == (newbitset & j)) continue;

    if (newbitset & j) 
      l = snprintf(path, MAX_STRING_SIZE, "%s/%u.do", basepath, i);
    else
      l = snprintf(path, MAX_STRING_SIZE, "%s/%u.undo", basepath, i);
    if (l < 1 || l >= (MAX_STRING_SIZE-1)) {
      fprintf(stderr, "failure: oversized command line for bit %i\n", i);
      continue;
    }

#if DEBUG
    fprintf(stderr, "executing: %s\n", path);
#endif
    system(path);
  }
  bitset = newbitset;
}

void reset_all_keymappings() {
#if DEBUG
  fprintf(stderr, "terminating\n");
#endif
  update_keymappings_for_bits(0);
}
  
void find_keymappings(unsigned long wid, XClassHint *c) {
/* This is a no-op if there is no user config. */
#ifdef USER_CONFIG
  mappings m;
  uint32_t i, newbitset = 0;

  if (!strcmp(c->res_name, currentmap))
    return; /* nothing to do */

  /* leave null termination */
  (void)strncpy(currentmap, c->res_name, (MAX_STRING_SIZE - 1));
  for(i=0; ; i++) {
    m = keymaps[i];
    if (!m.statemask || !m.wclass)
      break;

    if (!strcmp(c->res_name, m.wclass)) {
      if ((newbitset & m.statemask) != m.statemask)
        newbitset |= m.statemask;
      break;
    }
  }
  update_keymappings_for_bits(newbitset);
#endif
}

static void bye(int for_great_justice_take_off_every_sig) {
#if DEBUG
  fprintf(stderr, "signal\n");
#endif
  exit(0);
}

static int xerrorh(Display *d, XErrorEvent *e) {
#if DEBUG
  fprintf(stderr, "Caught Xlib error=%d code=%d\n",
    e->error_code, e->request_code);
#endif
  return 0;
}

int main(int argc, char **argv) {
  Display *d;
  Window w;
  XEvent e;
  int i;

  if (!getenv("HOME")) {
    fprintf(stderr, "unable to determine home directory\n");
    return 1;
  }
  i = snprintf(basepath, MAX_STRING_SIZE, "%s/.appmodmaps", getenv("HOME"));
  if (i < 1 || i >= (MAX_STRING_SIZE-1)) {
    fprintf(stderr, "unable to compute base path\n");
    return 1;
  }

  d = XOpenDisplay(NULL);
  if (!d) {
    fprintf(stderr, "Failed to open X display\n");
    return 1;
  }

  w = DefaultRootWindow(d);
  XSelectInput(d, w, PropertyChangeMask);

  atexit(reset_all_keymappings);
  (void)memset(&action, 0, sizeof(action));
  action.sa_handler = bye;
  if (sigaction(SIGINT, &action, 0) || sigaction(SIGTERM, &action, 0)) {
    perror("sigaction failed");
    return 1;
  }

  (void)memset(currentmap, 0, sizeof(currentmap));
  (void)XSetErrorHandler(xerrorh);
  for (;;) {
    XNextEvent(d, &e);
    if (e.type == PropertyNotify) {
      if (!strcmp(XGetAtomName(e.xproperty.display, e.xproperty.atom), "_NET_ACTIVE_WINDOW")) {
        Atom f, a;
        int af, status;
        unsigned long ni, ba;
        unsigned long *prop; /* native endian and bit length */

        /* get the new active window */
        f = XInternAtom(d, "_NET_ACTIVE_WINDOW", True);
        status = XGetWindowProperty(
          d,
          e.xproperty.window,
          f, 0, 1000, False, AnyPropertyType,
          &a, &af, &ni, &ba, (unsigned char **)&prop);
        if (status == Success) {
          unsigned long wid = *prop;
          XClassHint *c;

          /* not at all unusual to get an XBadWindow now and then */
          if (!wid)
            continue;

          /* get class */
          c = XAllocClassHint();
          if (c) {
            status = XGetClassHint(d, (Window)wid, c);
            if (status) {
#if DEBUG
#if __LP64__
              fprintf(stdout, "0x%08lx \"%s\" \"%s\"\n",
                wid, c->res_name, c->res_class);
#else
              fprintf(stdout, "0x%08x \"%s\" \"%s\"\n",
                wid, c->res_name, c->res_class);
#endif
#endif
              find_keymappings(wid, c);
            }
#if DEBUG
            else {
              fprintf(stderr, "failure with class hint: %i\n", status);
            }
#endif
            XFree(c);
          } else {
            /* fatal */
            fprintf(stderr, "out of memory!!!!\n");
            return 1;
          }
        }
#if DEBUG
        else {
          fprintf(stderr, "failed to get window property: %i\n", status);
        }
#endif
      }
    }
  }

  return 0;
}

