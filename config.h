/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include <X11/keysymdef.h>
/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 12;       /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char col_gray1[]       = "#000000";
static const char col_gray2[]       = "#000000";
static const char col_gray3[]       = "#dddddd";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#202040";
static const char col_border[]      = "#32d0c0";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_border },
};
// systray
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
// alt-tab config
static const unsigned int tabModKey 		= 0x40;	/* if this key is hold the alt-tab functionality stays acitve. This key must be the same as key that is used to active functin altTabStart `*/
static const unsigned int tabCycleKey 		= 0x17;	/* if this key is hit the alt-tab program moves one position forward in clients stack. This key must be the same as key that is used to active functin altTabStart */
static const unsigned int tabEndKey 		= 0x9;	/* if this key is hit the while you're in the alt-tab mode, you'll be returned to previous state (alt-tab mode turned off and your window of origin will be selected) */
static const unsigned int tabMoveBackKey    = 0x32;	/* if this key is being hold while hitting tabCycleKey you'll move in stack of windows backwards */
static const unsigned int tabPosY 			= 1;	/* tab position on Y axis, 0 = bottom, 1 = center, 2 = top */
static const unsigned int tabPosX 			= 1;	/* tab position on X axis, 0 = left, 1 = center, 2 = right */
static const unsigned int maxWTab 			= 600;	/* tab menu width */
static const unsigned int maxHTab 			= 200;	/* tab menu height */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
/*tt class     	 instance  title     tags			 isfloating  isterminal  noswallow  monitor widht height */
	{ "st-hyperv",  NULL,     NULL,    0,         0,          1,           0,        -1, 		1280, 1024 	},
	{ "st",         NULL,     NULL,    0,         0,          1,           0,        -1, 		1280, 1024 	},
	{ NULL,      	NULL,     NULL,    0,         0,          0,           0,        -1, 		1280, 1024 	},
 };

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	//  symbol - arrange function
	// first entry is default
	{ "[M]",      monocle },
	{ "[]=",      tile },
	{ "><>",      NULL }, /* no layout function means floating behavior */
};

/* key definitions */
#define WINKEY Mod4Mask
#define ALTKEY Mod1Mask
//
#define MODKEY WINKEY
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
extern const char *dmenucmd[];
extern char stfont[];
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *termcmd[]  = { "st", "-t", "terminal", "-f", stfont, NULL };
static const char *volumeup[] = { NULL };
static const char *volumedown[] = { NULL };
static const char *screen_off[] = { "xset", "dpms", "force", "standby", NULL };
static const char *make_snippet[] = { "/home/nikita/scripts/make_snippet", NULL };
static const char *select_snippet[] = { "/home/nikita/scripts/select_snippet", NULL };
static const char *shutdown[] = { "shutdown","-h", "now", NULL };
static const char *run_chrome[] = { "google-chrome-stable", NULL };
static const char *run_chrome_incognito[] = { "/home/nikita/scripts/run_chrome_incognito.sh", NULL };
static const char *run_spotify[] = { "spotify-launcher", NULL };
static const char *run_rofi[] = { "rofi", "-show", "drun", NULL };
static const char *run_rofi_calc[] = { "st", "-A", "0", "-e", "rofi", "-show", "calc", "-modi", "calc" ,"-no-show-match", "-no-sort", NULL };
static const char *run_turn_on_laptop_monitor[] = { "/home/nikita/scripts/switch_monitor.sh", "laptop", NULL };
static const char *run_turn_on_external_monitor[] = { "/home/nikita/scripts/switch_monitor.sh", NULL };
static const char *kill_x[] = { "pkill", "X", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_p,      spawn,          {.v = run_rofi } },
	{ MODKEY,                       XK_c,      spawn,          {.v = run_rofi_calc } },
	{ MODKEY,             					XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Delete, spawn,          {.v = shutdown } },
	{ MODKEY|ShiftMask,             XK_BackSpace,   spawn,          {.v = kill_x } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_Right,  focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_Left,   focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_r,      incnmaster,     {0} },
	{ MODKEY,                       XK_Up,     setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Down,   setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,             					XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_l,      spawn,      		 {.v = screen_off } },
	{ MODKEY|ShiftMask,             XK_b,      spawn,      		 {.v = make_snippet } },
	{ MODKEY, 	         						XK_Insert, spawn,      		 {.v = select_snippet } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -8 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +8 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0  } },
	{ MODKEY|ALTKEY,                XK_Left,   shiftview,      { .i = -1 } },
	{ MODKEY|ALTKEY,                XK_Right,  shiftview,      { .i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_grave, quit,           {0} },
	{ MODKEY|ShiftMask, 						XK_s,                    spawn,	SHCMD("flameshot gui")   }, 
	{ 0,														XF86XK_AudioRaiseVolume, spawn, { .v = volumeup        } }, 
	{ 0,														XF86XK_AudioLowerVolume, spawn, { .v = volumedown      } }, 
	{ 0,														XF86XK_AudioPlay,        spawn, SHCMD("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.PlayPause")  }, 
	{ 0,														XF86XK_AudioStop,        spawn, SHCMD("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.Stop") }, 
	{ 0,														XF86XK_AudioNext,        spawn, SHCMD("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.Next") }, 
	{ 0,														XF86XK_AudioPrev,        spawn, SHCMD("dbus-send --print-reply --dest=org.mpris.MediaPlayer2.spotify /org/mpris/MediaPlayer2 org.mpris.MediaPlayer2.Player.Previous") }, 
	{ MODKEY, 	         						XK_F1,                   spawn, {.v = run_chrome } },
	{ MODKEY, 	         						XK_F2,                   spawn, {.v = run_spotify } },
	{ MODKEY, 	         						XK_F3,                   spawn, {.v = run_chrome_incognito } },
	{ MODKEY, 	         						XK_F11,                  spawn, {.v = run_turn_on_external_monitor } },
	{ MODKEY, 	         						XK_F12,                  spawn, {.v = run_turn_on_laptop_monitor } },

	{ Mod1Mask,             		XK_Tab,    altTabStart,	   {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

