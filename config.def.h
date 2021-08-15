/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static unsigned int borderpx  = 1;				/* border pixel of windows */
static unsigned int snap      = 0;       		/* snap pixel */
static int showbar            = 1;        		/* 0 means no bar */
static int topbar             = 0;        		/* 0 means bottom bar */
static const unsigned int gappih    = 5;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 5;       /* vert inner gap between windows */
static const unsigned int gappoh    = 5;       /* horiz outer gap between windows and screen edge */ 
static const unsigned int gappov    = 5;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
//static const char *fonts[]          = { "iosevka:size=9" };
//static const char dmenufont[]       = "iosevka:size=9";
//static const char *fonts[]          = { "Liberation Mono:size=9:antialias=true:autohint=true" };
//static const char dmenufont[]       = "Liberation Mono:size=9:antialias=true:autohint=true";
static const char *fonts[]          = { "monospace:size=11:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=11:antialias=true:autohint=true";

//#include "/home/dizzy/.cache/wal/colors-wal-dwm.h"
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";

static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#770000";
static char selbgcolor[]            = "#005577";

static char col_urgborder[]	    = "#ff0000";
static char col_urgfg[]	    	    = "#ff0000";
static char col_urgbg[]	    	    = "#ff0000";

static char *colors[][3]      = {
	/*               fg				bg				border   */
	[SchemeNorm] = { normfgcolor,	normbgcolor,	normbordercolor },
	[SchemeSel]  = { selfgcolor,	selbgcolor,		selbordercolor  },
	[SchemeUrg]  = { col_urgfg,	col_urgbg,		col_urgborder  },
};

/* tagging */
static const char *tags[] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor    scratch keys	float x,y,w,h         floatborderpx*/
	{ "Gimp",     NULL,       NULL,       0,            1,           -1,        0,				50,50,500,500,			5 },
	{ "feh",      NULL,       NULL,       0,            1,           -1,        0,				50,50,500,500,			5 },
	{ "mpv",      NULL,       NULL,       0,            1,           -1,        0,				50,50,500,500,			5 },
	{ NULL,       NULL,   "scratchpad",   0,            1,           -1,       's',				50,50,1200,700,			4 },

	/* Normal Apps */
	{ NULL,       NULL,   "sys_monitor",  1 << 8,       0,            1,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "newsboat",     1 << 7,       0,            0,       0,				50,50,700,500,			1 },
	{ "firefox",  NULL,   NULL,           1 << 1,	    0,            0,       0,				50,50,500,500,			5 },
	{ NULL,       NULL,   "music",        1 << 8,       0,            0,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "mixer",        1 << 7,       0,            1,       0,				50,50,700,500,			1 },

	/* Development Apps */
	{ NULL,       NULL,   "code0",        1 << 0,       0,            0,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "code1",        1 << 0,       0,            0,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "code2",        1 << 2,       0,            0,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "code3",        1 << 0,       0,            1,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "code4",        1 << 1,       0,            1,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "code5",        1 << 2,       0,            1,       0,				50,50,700,500,			1 },
	{ NULL,       NULL,   "code6",        1 << 3,       0,            1,       0,				50,50,700,500,			1 },
};

/* layout(s) */
static float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define TAGMASK ((int)((1LL << LENGTH(tags)) - 1))
#define HOLDKEY XK_Super_L // replace 0 with the keysym to activate holdbar

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
//static const char *dmenucmd[] = { "dmenu_run", "-b", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *dmenucmd[] = { "dmenu_run", "-b",  "-l", "5", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
/*First arg only serves to match against key in rules*/
static const char *scratchpadcmd[] = {"s", "alacritty", "-t", "scratchpad", NULL}; 
static const char *filemanagercmd[] = {"s", "alacritty", "-e", "nnn", NULL}; 

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
//		{ "color0",        STRING,  &normbgcolor },
//		{ "color8",    STRING,  &normbordercolor },
//		{ "color15",        STRING,  &normfgcolor },
//		{ "color2",         STRING,  &selbgcolor },
//		{ "color15",     STRING,  &selbordercolor },
//		{ "color15",         STRING,  &selfgcolor },
		{ "color0",		STRING,	&normbordercolor },
		{ "color8",		STRING,	&selbordercolor },
		{ "color0",		STRING,	&normbgcolor },
		{ "color4",		STRING,	&normfgcolor },
		{ "color0",		STRING,	&selfgcolor },
		{ "color4",		STRING,	&selbgcolor },

		{ "color1",         STRING,  &col_urgborder },
		{ "color15",         STRING,  &col_urgfg },
		{ "color1",         STRING,  &col_urgbg },
		//{ "borderpx",          	INTEGER, &borderpx },
		//{ "snap",          	INTEGER, &snap },
		//{ "showbar",          	INTEGER, &showbar },
		//{ "topbar",          	INTEGER, &topbar },
		//{ "nmaster",          	INTEGER, &nmaster },
		//{ "resizehints",       	INTEGER, &resizehints },
		//{ "mfact",      	 	FLOAT,   &mfact },
};

static const char *upvol[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_u,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          SHCMD("flameshot gui") },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD("xsecurelock") },
	{ MODKEY|ControlMask|ShiftMask,             XK_s,      spawn,          SHCMD("loginctl suspend") },
	{ MODKEY,			XK_w,      spawn,          SHCMD("dweb") },
	{ MODKEY,	XK_s,      spawn,          SHCMD("dstartup") },
	{ MODKEY|ControlMask,			XK_s,      spawn,          SHCMD("dsnippets") },
	{ MODKEY|ControlMask,		XK_w,      spawn,          SHCMD("change_wallpaper") },
	{ MODKEY|ControlMask,		XK_a,      spawn,          SHCMD("add_wallpaper") },
	{ MODKEY|ControlMask|ShiftMask,		XK_w,      spawn,          SHCMD("change_vid_wallpaper") },
	{ MODKEY|ControlMask|ShiftMask,		XK_a,      spawn,          SHCMD("add_vid_wallpaper") },
	{ MODKEY,						XK_r,      		spawn,          SHCMD("dtodotxt") },
	{ MODKEY,			XK_e,      spawn,          SHCMD("demoji") },
	{ MODKEY,			XK_o,      spawn,          SHCMD("dmpv_open") },
	{ MODKEY,			XK_p,      spawn,          SHCMD("bitwarden-dmenu --dmenu-args='-i' --dmenu-pswd-args='-nf white -P' --clear-clipboard 30 --session-timeout 100 --sync-vault-after 3600 --on-error 'xargs notify-send --urgency=low'") },
	{ MODKEY,			XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_n,  		spawn,  {.v = filemanagercmd } },
	//{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_z,	   zoom,           {0} },
	{ MODKEY|Mod4Mask,              XK_u,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_u,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_i,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_i,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_o,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_6,      incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_6,      incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_7,      incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_7,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_8,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_8,      incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_9,      incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_9,      incrovgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[13]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_t,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_x,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_F4,     spawn,          { .v = (const char*[]){ "refresh_wallpaper", NULL } } },
	{ MODKEY,                       XK_F5,     xresources,     {.v = NULL}},
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ 0,							XF86XK_AudioLowerVolume,   spawn,  {.v = downvol } },
	{ 0,                       		XF86XK_AudioMute,		   spawn,  {.v = mutevol } },
	{ 0,                       		XF86XK_AudioRaiseVolume,    spawn, {.v = upvol   } },
	{ 0,                            HOLDKEY,   holdbar,        {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

