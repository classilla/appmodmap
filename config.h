/* Define command bits. */

  /* Demo bit included: map Command key as an alias to Control. */
#define CTRLCMD (1<<31)

  /* Define additional local bits here in this format. Work up from 0 to
     avoid conflict with included default ones. */
/* #define BIT1CMD (1<<0) */
/* #define BIT2CMD (1<<1) */
  /* etc. */

/* For each window name specified, include the bit value of the keyboard
   settings to be enabled when that window name is active. */

mappings keymaps[] = {
	{ "epiphany", CTRLCMD },
	{ "soffice", CTRLCMD },
	{ "libreoffice", CTRLCMD },
	{ "gedit", CTRLCMD },
	{ "org.gnome.gedit", CTRLCMD },
	{ "nautilus", CTRLCMD },
	{ "org.gnome.Nautilus", CTRLCMD },
	{ "krita", CTRLCMD },
	{ "vlc", CTRLCMD },
	{ "Pinta", CTRLCMD },
	{ "inkscape", CTRLCMD },
	{ "gimp-2.8", CTRLCMD },
	{ "retext", CTRLCMD },
	{ "evince", CTRLCMD },
	{ "ghex", CTRLCMD },

/* Terminate the list with this couplet. */

	{ NULL, 0 },
};

