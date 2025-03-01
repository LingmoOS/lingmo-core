/* SPDX-License-Identifier: GPL-2.0-only WITH Linux-syscall-note */
/*
 * Input event codes
 *
 *    *** IMPORTANT ***
 * This file is not only included from C-code but also from devicetree source
 * files. As such this file MUST only contain comments and defines.
 *
 * Copyright (c) 1999-2002 Vojtech Pavlik
 * Copyright (c) 2015 Hans de Goede <hdegoede@redhat.com>
 * Copyright (c) 2025 Elysia <elysia@lingmo.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */
 #ifndef _INPUT_EVENT_CODES_H
 #define _INPUT_EVENT_CODES_H

 #include <QtCore>
 
 /*
  * Device properties and quirks
  */
 
 #define QT_INPUT_PROP_POINTER		0x00	/* needs a pointer */
 #define QT_INPUT_PROP_DIRECT		0x01	/* direct input devices */
 #define QT_INPUT_PROP_BUTTONPAD		0x02	/* has button(s) under pad */
 #define QT_INPUT_PROP_SEMI_MT		0x03	/* touch rectangle only */
 #define QT_INPUT_PROP_TOPBUTTONPAD		0x04	/* softbuttons at top of pad */
 #define QT_INPUT_PROP_POINTING_STICK	0x05	/* is a pointing stick */
 #define QT_INPUT_PROP_ACCELEROMETER	0x06	/* has accelerometer */
 
 #define QT_INPUT_PROP_MAX			0x1f
 #define QT_INPUT_PROP_CNT			(QT_INPUT_PROP_MAX + 1)
 
 /*
  * Event types
  */
 
 #define QT_EV_SYN			0x00
 #define QT_EV_KEY			0x01
 #define QT_EV_REL			0x02
 #define QT_EV_ABS			0x03
 #define QT_EV_MSC			0x04
 #define QT_EV_SW			0x05
 #define QT_EV_LED			0x11
 #define QT_EV_SND			0x12
 #define QT_EV_REP			0x14
 #define QT_EV_FF			0x15
 #define QT_EV_PWR			0x16
 #define QT_EV_FF_STATUS		0x17
 #define QT_EV_MAX			0x1f
 #define QT_EV_CNT			(QT_EV_MAX+1)
 
 /*
  * Synchronization events.
  */
 
 #define QT_SYN_REPORT		0
 #define QT_SYN_CONFIG		1
 #define QT_SYN_MT_REPORT		2
 #define QT_SYN_DROPPED		3
 #define QT_SYN_MAX			0xf
 #define QT_SYN_CNT			(QT_SYN_MAX+1)
 
 /*
  * Keys and buttons
  *
  * Most of the keys/buttons are modeled after USB HUT 1.12
  * (see http://www.usb.org/developers/hidpage).
  * Abbreviations in the comments:
  * AC - Application Control
  * AL - Application Launch Button
  * SC - System Control
  */
 
 #define QT_KEY_RESERVED		Qt::Key_unknown
 #define QT_KEY_ESC			Qt::Key_Escape
 #define QT_KEY_1			Qt::Key_1
 #define QT_KEY_2			Qt::Key_2
 #define QT_KEY_3			Qt::Key_3
 #define QT_KEY_4			Qt::Key_4
 #define QT_KEY_5			Qt::Key_5
 #define QT_KEY_6			Qt::Key_6
 #define QT_KEY_7			8
 #define QT_KEY_8			9
 #define QT_KEY_9			10
 #define QT_KEY_0			11
 #define QT_KEY_MINUS		12
 #define QT_KEY_EQUAL		13
 #define QT_KEY_BACKSPACE		14
 #define QT_KEY_TAB			15
 #define QT_KEY_Q			16
 #define QT_KEY_W			17
 #define QT_KEY_E			18
 #define QT_KEY_R			19
 #define QT_KEY_T			20
 #define QT_KEY_Y			21
 #define QT_KEY_U			22
 #define QT_KEY_I			23
 #define QT_KEY_O			24
 #define QT_KEY_P			25
 #define QT_KEY_LEFTBRACE		26
 #define QT_KEY_RIGHTBRACE		27
 #define QT_KEY_ENTER		28
 #define QT_KEY_LEFTCTRL		29
 #define QT_KEY_A			30
 #define QT_KEY_S			31
 #define QT_KEY_D			32
 #define QT_KEY_F			33
 #define QT_KEY_G			34
 #define QT_KEY_H			35
 #define QT_KEY_J			36
 #define QT_KEY_K			37
 #define QT_KEY_L			38
 #define QT_KEY_SEMICOLON		39
 #define QT_KEY_APOSTROPHE		40
 #define QT_KEY_GRAVE		41
 #define QT_KEY_LEFTSHIFT		42
 #define QT_KEY_BACKSLASH		43
 #define QT_KEY_Z			44
 #define QT_KEY_X			45
 #define QT_KEY_C			46
 #define QT_KEY_V			47
 #define QT_KEY_B			48
 #define QT_KEY_N			49
 #define QT_KEY_M			50
 #define QT_KEY_COMMA		51
 #define QT_KEY_DOT			52
 #define QT_KEY_SLASH		53
 #define QT_KEY_RIGHTSHIFT		54
 #define QT_KEY_KPASTERISK		55
 #define QT_KEY_LEFTALT		56
 #define QT_KEY_SPACE		57
 #define QT_KEY_CAPSLOCK		58
 #define QT_KEY_F1			59
 #define QT_KEY_F2			60
 #define QT_KEY_F3			61
 #define QT_KEY_F4			62
 #define QT_KEY_F5			63
 #define QT_KEY_F6			64
 #define QT_KEY_F7			65
 #define QT_KEY_F8			66
 #define QT_KEY_F9			67
 #define QT_KEY_F10			68
 #define QT_KEY_NUMLOCK		69
 #define QT_KEY_SCROLLLOCK		70
 #define QT_KEY_KP7			71
 #define QT_KEY_KP8			72
 #define QT_KEY_KP9			73
 #define QT_KEY_KPMINUS		74
 #define QT_KEY_KP4			75
 #define QT_KEY_KP5			76
 #define QT_KEY_KP6			77
 #define QT_KEY_KPPLUS		78
 #define QT_KEY_KP1			79
 #define QT_KEY_KP2			80
 #define QT_KEY_KP3			81
 #define QT_KEY_KP0			82
 #define QT_KEY_KPDOT		83
 
 #define QT_KEY_ZENKAKUHANKAKU	85
 #define QT_KEY_102ND		86
 #define QT_KEY_F11			87
 #define QT_KEY_F12			88
 #define QT_KEY_RO			89
 #define QT_KEY_KATAKANA		90
 #define QT_KEY_HIRAGANA		91
 #define QT_KEY_HENKAN		92
 #define QT_KEY_KATAKANAHIRAGANA	93
 #define QT_KEY_MUHENKAN		94
 #define QT_KEY_KPJPCOMMA		95
 #define QT_KEY_KPENTER		96
 #define QT_KEY_RIGHTCTRL		97
 #define QT_KEY_KPSLASH		98
 #define QT_KEY_SYSRQ		99
 #define QT_KEY_RIGHTALT		100
 #define QT_KEY_LINEFEED		101
 #define QT_KEY_HOME		102
 #define QT_KEY_UP			103
 #define QT_KEY_PAGEUP		104
 #define QT_KEY_LEFT		105
 #define QT_KEY_RIGHT		106
 #define QT_KEY_END			107
 #define QT_KEY_DOWN		108
 #define QT_KEY_PAGEDOWN		109
 #define QT_KEY_INSERT		110
 #define QT_KEY_DELETE		111
 #define QT_KEY_MACRO		112
 #define QT_KEY_MUTE		113
 #define QT_KEY_VOLUMEDOWN		114
 #define QT_KEY_VOLUMEUP		115
 #define QT_KEY_POWER		116	/* SC System Power Down */
 #define QT_KEY_KPEQUAL		117
 #define QT_KEY_KPPLUSMINUS		118
 #define QT_KEY_PAUSE		119
 #define QT_KEY_SCALE		120	/* AL Compiz Scale (Expose) */
 
 #define QT_KEY_KPCOMMA		121
 #define QT_KEY_HANGEUL		122
 #define QT_KEY_HANGUEL		QT_KEY_HANGEUL
 #define QT_KEY_HANJA		123
 #define QT_KEY_YEN			124
 #define QT_KEY_LEFTMETA		125
 #define QT_KEY_RIGHTMETA		126
 #define QT_KEY_COMPOSE		127
 
 #define QT_KEY_STOP		128	/* AC Stop */
 #define QT_KEY_AGAIN		129
 #define QT_KEY_PROPS		130	/* AC Properties */
 #define QT_KEY_UNDO		131	/* AC Undo */
 #define QT_KEY_FRONT		132
 #define QT_KEY_COPY		133	/* AC Copy */
 #define QT_KEY_OPEN		134	/* AC Open */
 #define QT_KEY_PASTE		135	/* AC Paste */
 #define QT_KEY_FIND		136	/* AC Search */
 #define QT_KEY_CUT			137	/* AC Cut */
 #define QT_KEY_HELP		138	/* AL Integrated Help Center */
 #define QT_KEY_MENU		139	/* Menu (show menu) */
 #define QT_KEY_CALC		140	/* AL Calculator */
 #define QT_KEY_SETUP		141
 #define QT_KEY_SLEEP		142	/* SC System Sleep */
 #define QT_KEY_WAKEUP		143	/* System Wake Up */
 #define QT_KEY_FILE		144	/* AL Local Machine Browser */
 #define QT_KEY_SENDFILE		145
 #define QT_KEY_DELETEFILE		146
 #define QT_KEY_XFER		147
 #define QT_KEY_PROG1		148
 #define QT_KEY_PROG2		149
 #define QT_KEY_WWW			150	/* AL Internet Browser */
 #define QT_KEY_MSDOS		151
 #define QT_KEY_COFFEE		152	/* AL Terminal Lock/Screensaver */
 #define QT_KEY_SCREENLOCK		QT_KEY_COFFEE
 #define QT_KEY_ROTATE_DISPLAY	153	/* Display orientation for e.g. tablets */
 #define QT_KEY_DIRECTION		QT_KEY_ROTATE_DISPLAY
 #define QT_KEY_CYCLEWINDOWS	154
 #define QT_KEY_MAIL		155
 #define QT_KEY_BOOKMARKS		156	/* AC Bookmarks */
 #define QT_KEY_COMPUTER		157
 #define QT_KEY_BACK		158	/* AC Back */
 #define QT_KEY_FORWARD		159	/* AC Forward */
 #define QT_KEY_CLOSECD		160
 #define QT_KEY_EJECTCD		161
 #define QT_KEY_EJECTCLOSECD	162
 #define QT_KEY_NEXTSONG		163
 #define QT_KEY_PLAYPAUSE		164
 #define QT_KEY_PREVIOUSSONG	165
 #define QT_KEY_STOPCD		166
 #define QT_KEY_RECORD		167
 #define QT_KEY_REWIND		168
 #define QT_KEY_PHONE		169	/* Media Select Telephone */
 #define QT_KEY_ISO			170
 #define QT_KEY_CONFIG		171	/* AL Consumer Control Configuration */
 #define QT_KEY_HOMEPAGE		172	/* AC Home */
 #define QT_KEY_REFRESH		173	/* AC Refresh */
 #define QT_KEY_EXIT		174	/* AC Exit */
 #define QT_KEY_MOVE		175
 #define QT_KEY_EDIT		176
 #define QT_KEY_SCROLLUP		177
 #define QT_KEY_SCROLLDOWN		178
 #define QT_KEY_KPLEFTPAREN		179
 #define QT_KEY_KPRIGHTPAREN	180
 #define QT_KEY_NEW			181	/* AC New */
 #define QT_KEY_REDO		182	/* AC Redo/Repeat */
 
 #define QT_KEY_F13			183
 #define QT_KEY_F14			184
 #define QT_KEY_F15			185
 #define QT_KEY_F16			186
 #define QT_KEY_F17			187
 #define QT_KEY_F18			188
 #define QT_KEY_F19			189
 #define QT_KEY_F20			190
 #define QT_KEY_F21			191
 #define QT_KEY_F22			192
 #define QT_KEY_F23			193
 #define QT_KEY_F24			194
 
 #define QT_KEY_PLAYCD		200
 #define QT_KEY_PAUSECD		201
 #define QT_KEY_PROG3		202
 #define QT_KEY_PROG4		203
 #define QT_KEY_ALL_APPLICATIONS	204	/* AC Desktop Show All Applications */
 #define QT_KEY_DASHBOARD		QT_KEY_ALL_APPLICATIONS
 #define QT_KEY_SUSPEND		205
 #define QT_KEY_CLOSE		206	/* AC Close */
 #define QT_KEY_PLAY		207
 #define QT_KEY_FASTFORWARD		208
 #define QT_KEY_BASSBOOST		209
 #define QT_KEY_PRINT		210	/* AC Print */
 #define QT_KEY_HP			211
 #define QT_KEY_CAMERA		212
 #define QT_KEY_SOUND		213
 #define QT_KEY_QUESTION		214
 #define QT_KEY_EMAIL		215
 #define QT_KEY_CHAT		216
 #define QT_KEY_SEARCH		217
 #define QT_KEY_CONNECT		218
 #define QT_KEY_FINANCE		219	/* AL Checkbook/Finance */
 #define QT_KEY_SPORT		220
 #define QT_KEY_SHOP		221
 #define QT_KEY_ALTERASE		222
 #define QT_KEY_CANCEL		223	/* AC Cancel */
 #define QT_KEY_BRIGHTNESSDOWN	224
 #define QT_KEY_BRIGHTNESSUP	225
 #define QT_KEY_MEDIA		226
 
 #define QT_KEY_SWITCHVIDEOMODE	227	/* Cycle between available video
                        outputs (Monitor/LCD/TV-out/etc) */
 #define QT_KEY_KBDILLUMTOGGLE	228
 #define QT_KEY_KBDILLUMDOWN	229
 #define QT_KEY_KBDILLUMUP		230
 
 #define QT_KEY_SEND		231	/* AC Send */
 #define QT_KEY_REPLY		232	/* AC Reply */
 #define QT_KEY_FORWARDMAIL		233	/* AC Forward Msg */
 #define QT_KEY_SAVE		234	/* AC Save */
 #define QT_KEY_DOCUMENTS		235
 
 #define QT_KEY_BATTERY		236
 
 #define QT_KEY_BLUETOOTH		237
 #define QT_KEY_WLAN		238
 #define QT_KEY_UWB			239
 
 #define QT_KEY_UNKNOWN		240
 
 #define QT_KEY_VIDEO_NEXT		241	/* drive next video source */
 #define QT_KEY_VIDEO_PREV		242	/* drive previous video source */
 #define QT_KEY_BRIGHTNESS_CYCLE	243	/* brightness up, after max is min */
 #define QT_KEY_BRIGHTNESS_AUTO	244	/* Set Auto Brightness: manual
                       brightness control is off,
                       rely on ambient */
 #define QT_KEY_BRIGHTNESS_ZERO	QT_KEY_BRIGHTNESS_AUTO
 #define QT_KEY_DISPLAY_OFF		245	/* display device to off state */
 
 #define QT_KEY_WWAN		246	/* Wireless WAN (LTE, UMTS, GSM, etc.) */
 #define QT_KEY_WIMAX		QT_KEY_WWAN
 #define QT_KEY_RFKILL		247	/* Key that controls all radios */
 
 #define QT_KEY_MICMUTE		248	/* Mute / unmute the microphone */
 
 /* Code 255 is reserved for special needs of AT keyboard driver */
 
 #define QT_BTN_MISC		0x100
 #define QT_BTN_0			0x100
 #define QT_BTN_1			0x101
 #define QT_BTN_2			0x102
 #define QT_BTN_3			0x103
 #define QT_BTN_4			0x104
 #define QT_BTN_5			0x105
 #define QT_BTN_6			0x106
 #define QT_BTN_7			0x107
 #define QT_BTN_8			0x108
 #define QT_BTN_9			0x109
 
 #define QT_BTN_MOUSE		0x110
 #define QT_BTN_LEFT		0x110
 #define QT_BTN_RIGHT		0x111
 #define QT_BTN_MIDDLE		0x112
 #define QT_BTN_SIDE		0x113
 #define QT_BTN_EXTRA		0x114
 #define QT_BTN_FORWARD		0x115
 #define QT_BTN_BACK		0x116
 #define QT_BTN_TASK		0x117
 
 #define QT_BTN_JOYSTICK		0x120
 #define QT_BTN_TRIGGER		0x120
 #define QT_BTN_THUMB		0x121
 #define QT_BTN_THUMB2		0x122
 #define QT_BTN_TOP			0x123
 #define QT_BTN_TOP2		0x124
 #define QT_BTN_PINKIE		0x125
 #define QT_BTN_BASE		0x126
 #define QT_BTN_BASE2		0x127
 #define QT_BTN_BASE3		0x128
 #define QT_BTN_BASE4		0x129
 #define QT_BTN_BASE5		0x12a
 #define QT_BTN_BASE6		0x12b
 #define QT_BTN_DEAD		0x12f
 
 #define QT_BTN_GAMEPAD		0x130
 #define QT_BTN_SOUTH		0x130
 #define QT_BTN_A			QT_BTN_SOUTH
 #define QT_BTN_EAST		0x131
 #define QT_BTN_B			QT_BTN_EAST
 #define QT_BTN_C			0x132
 #define QT_BTN_NORTH		0x133
 #define QT_BTN_X			QT_BTN_NORTH
 #define QT_BTN_WEST		0x134
 #define QT_BTN_Y			QT_BTN_WEST
 #define QT_BTN_Z			0x135
 #define QT_BTN_TL			0x136
 #define QT_BTN_TR			0x137
 #define QT_BTN_TL2			0x138
 #define QT_BTN_TR2			0x139
 #define QT_BTN_SELECT		0x13a
 #define QT_BTN_START		0x13b
 #define QT_BTN_MODE		0x13c
 #define QT_BTN_THUMBL		0x13d
 #define QT_BTN_THUMBR		0x13e
 
 #define QT_BTN_DIGI		0x140
 #define QT_BTN_TOOL_PEN		0x140
 #define QT_BTN_TOOL_RUBBER		0x141
 #define QT_BTN_TOOL_BRUSH		0x142
 #define QT_BTN_TOOL_PENCIL		0x143
 #define QT_BTN_TOOL_AIRBRUSH	0x144
 #define QT_BTN_TOOL_FINGER		0x145
 #define QT_BTN_TOOL_MOUSE		0x146
 #define QT_BTN_TOOL_LENS		0x147
 #define QT_BTN_TOOL_QUINTTAP	0x148	/* Five fingers on trackpad */
 #define QT_BTN_STYLUS3		0x149
 #define QT_BTN_TOUCH		0x14a
 #define QT_BTN_STYLUS		0x14b
 #define QT_BTN_STYLUS2		0x14c
 #define QT_BTN_TOOL_DOUBLETAP	0x14d
 #define QT_BTN_TOOL_TRIPLETAP	0x14e
 #define QT_BTN_TOOL_QUADTAP	0x14f	/* Four fingers on trackpad */
 
 #define QT_BTN_WHEEL		0x150
 #define QT_BTN_GEAR_DOWN		0x150
 #define QT_BTN_GEAR_UP		0x151
 
 #define QT_KEY_OK			0x160
 #define QT_KEY_SELECT		0x161
 #define QT_KEY_GOTO		0x162
 #define QT_KEY_CLEAR		0x163
 #define QT_KEY_POWER2		0x164
 #define QT_KEY_OPTION		0x165
 #define QT_KEY_INFO		0x166	/* AL OEM Features/Tips/Tutorial */
 #define QT_KEY_TIME		0x167
 #define QT_KEY_VENDOR		0x168
 #define QT_KEY_ARCHIVE		0x169
 #define QT_KEY_PROGRAM		0x16a	/* Media Select Program Guide */
 #define QT_KEY_CHANNEL		0x16b
 #define QT_KEY_FAVORITES		0x16c
 #define QT_KEY_EPG			0x16d
 #define QT_KEY_PVR			0x16e	/* Media Select Home */
 #define QT_KEY_MHP			0x16f
 #define QT_KEY_LANGUAGE		0x170
 #define QT_KEY_TITLE		0x171
 #define QT_KEY_SUBTITLE		0x172
 #define QT_KEY_ANGLE		0x173
 #define QT_KEY_FULL_SCREEN		0x174	/* AC View Toggle */
 #define QT_KEY_ZOOM		QT_KEY_FULL_SCREEN
 #define QT_KEY_MODE		0x175
 #define QT_KEY_KEYBOARD		0x176
 #define QT_KEY_ASPECT_RATIO	0x177	/* HUTRR37: Aspect */
 #define QT_KEY_SCREEN		QT_KEY_ASPECT_RATIO
 #define QT_KEY_PC			0x178	/* Media Select Computer */
 #define QT_KEY_TV			0x179	/* Media Select TV */
 #define QT_KEY_TV2			0x17a	/* Media Select Cable */
 #define QT_KEY_VCR			0x17b	/* Media Select VCR */
 #define QT_KEY_VCR2		0x17c	/* VCR Plus */
 #define QT_KEY_SAT			0x17d	/* Media Select Satellite */
 #define QT_KEY_SAT2		0x17e
 #define QT_KEY_CD			0x17f	/* Media Select CD */
 #define QT_KEY_TAPE		0x180	/* Media Select Tape */
 #define QT_KEY_RADIO		0x181
 #define QT_KEY_TUNER		0x182	/* Media Select Tuner */
 #define QT_KEY_PLAYER		0x183
 #define QT_KEY_TEXT		0x184
 #define QT_KEY_DVD			0x185	/* Media Select DVD */
 #define QT_KEY_AUX			0x186
 #define QT_KEY_MP3			0x187
 #define QT_KEY_AUDIO		0x188	/* AL Audio Browser */
 #define QT_KEY_VIDEO		0x189	/* AL Movie Browser */
 #define QT_KEY_DIRECTORY		0x18a
 #define QT_KEY_LIST		0x18b
 #define QT_KEY_MEMO		0x18c	/* Media Select Messages */
 #define QT_KEY_CALENDAR		0x18d
 #define QT_KEY_RED			0x18e
 #define QT_KEY_GREEN		0x18f
 #define QT_KEY_YELLOW		0x190
 #define QT_KEY_BLUE		0x191
 #define QT_KEY_CHANNELUP		0x192	/* Channel Increment */
 #define QT_KEY_CHANNELDOWN		0x193	/* Channel Decrement */
 #define QT_KEY_FIRST		0x194
 #define QT_KEY_LAST		0x195	/* Recall Last */
 #define QT_KEY_AB			0x196
 #define QT_KEY_NEXT		0x197
 #define QT_KEY_RESTART		0x198
 #define QT_KEY_SLOW		0x199
 #define QT_KEY_SHUFFLE		0x19a
 #define QT_KEY_BREAK		0x19b
 #define QT_KEY_PREVIOUS		0x19c
 #define QT_KEY_DIGITS		0x19d
 #define QT_KEY_TEEN		0x19e
 #define QT_KEY_TWEN		0x19f
 #define QT_KEY_VIDEOPHONE		0x1a0	/* Media Select Video Phone */
 #define QT_KEY_GAMES		0x1a1	/* Media Select Games */
 #define QT_KEY_ZOOMIN		0x1a2	/* AC Zoom In */
 #define QT_KEY_ZOOMOUT		0x1a3	/* AC Zoom Out */
 #define QT_KEY_ZOOMRESET		0x1a4	/* AC Zoom */
 #define QT_KEY_WORDPROCESSOR	0x1a5	/* AL Word Processor */
 #define QT_KEY_EDITOR		0x1a6	/* AL Text Editor */
 #define QT_KEY_SPREADSHEET		0x1a7	/* AL Spreadsheet */
 #define QT_KEY_GRAPHICSEDITOR	0x1a8	/* AL Graphics Editor */
 #define QT_KEY_PRESENTATION	0x1a9	/* AL Presentation App */
 #define QT_KEY_DATABASE		0x1aa	/* AL Database App */
 #define QT_KEY_NEWS		0x1ab	/* AL Newsreader */
 #define QT_KEY_VOICEMAIL		0x1ac	/* AL Voicemail */
 #define QT_KEY_ADDRESSBOOK		0x1ad	/* AL Contacts/Address Book */
 #define QT_KEY_MESSENGER		0x1ae	/* AL Instant Messaging */
 #define QT_KEY_DISPLAYTOGGLE	0x1af	/* Turn display (LCD) on and off */
 #define QT_KEY_BRIGHTNESS_TOGGLE	QT_KEY_DISPLAYTOGGLE
 #define QT_KEY_SPELLCHECK		0x1b0   /* AL Spell Check */
 #define QT_KEY_LOGOFF		0x1b1   /* AL Logoff */
 
 #define QT_KEY_DOLLAR		0x1b2
 #define QT_KEY_EURO		0x1b3
 
 #define QT_KEY_FRAMEBACK		0x1b4	/* Consumer - transport controls */
 #define QT_KEY_FRAMEFORWARD	0x1b5
 #define QT_KEY_CONTEXT_MENU	0x1b6	/* GenDesc - system context menu */
 #define QT_KEY_MEDIA_REPEAT	0x1b7	/* Consumer - transport control */
 #define QT_KEY_10CHANNELSUP	0x1b8	/* 10 channels up (10+) */
 #define QT_KEY_10CHANNELSDOWN	0x1b9	/* 10 channels down (10-) */
 #define QT_KEY_IMAGES		0x1ba	/* AL Image Browser */
 #define QT_KEY_NOTIFICATION_CENTER	0x1bc	/* Show/hide the notification center */
 #define QT_KEY_PICKUP_PHONE	0x1bd	/* Answer incoming call */
 #define QT_KEY_HANGUP_PHONE	0x1be	/* Decline incoming call */
 
 #define QT_KEY_DEL_EOL		0x1c0
 #define QT_KEY_DEL_EOS		0x1c1
 #define QT_KEY_INS_LINE		0x1c2
 #define QT_KEY_DEL_LINE		0x1c3
 
 #define QT_KEY_FN			0x1d0
 #define QT_KEY_FN_ESC		0x1d1
 #define QT_KEY_FN_F1		0x1d2
 #define QT_KEY_FN_F2		0x1d3
 #define QT_KEY_FN_F3		0x1d4
 #define QT_KEY_FN_F4		0x1d5
 #define QT_KEY_FN_F5		0x1d6
 #define QT_KEY_FN_F6		0x1d7
 #define QT_KEY_FN_F7		0x1d8
 #define QT_KEY_FN_F8		0x1d9
 #define QT_KEY_FN_F9		0x1da
 #define QT_KEY_FN_F10		0x1db
 #define QT_KEY_FN_F11		0x1dc
 #define QT_KEY_FN_F12		0x1dd
 #define QT_KEY_FN_1		0x1de
 #define QT_KEY_FN_2		0x1df
 #define QT_KEY_FN_D		0x1e0
 #define QT_KEY_FN_E		0x1e1
 #define QT_KEY_FN_F		0x1e2
 #define QT_KEY_FN_S		0x1e3
 #define QT_KEY_FN_B		0x1e4
 #define QT_KEY_FN_RIGHT_SHIFT	0x1e5
 
 #define QT_KEY_BRL_DOT1		0x1f1
 #define QT_KEY_BRL_DOT2		0x1f2
 #define QT_KEY_BRL_DOT3		0x1f3
 #define QT_KEY_BRL_DOT4		0x1f4
 #define QT_KEY_BRL_DOT5		0x1f5
 #define QT_KEY_BRL_DOT6		0x1f6
 #define QT_KEY_BRL_DOT7		0x1f7
 #define QT_KEY_BRL_DOT8		0x1f8
 #define QT_KEY_BRL_DOT9		0x1f9
 #define QT_KEY_BRL_DOT10		0x1fa
 
 #define QT_KEY_NUMERIC_0		0x200	/* used by phones, remote controls, */
 #define QT_KEY_NUMERIC_1		0x201	/* and other keypads */
 #define QT_KEY_NUMERIC_2		0x202
 #define QT_KEY_NUMERIC_3		0x203
 #define QT_KEY_NUMERIC_4		0x204
 #define QT_KEY_NUMERIC_5		0x205
 #define QT_KEY_NUMERIC_6		0x206
 #define QT_KEY_NUMERIC_7		0x207
 #define QT_KEY_NUMERIC_8		0x208
 #define QT_KEY_NUMERIC_9		0x209
 #define QT_KEY_NUMERIC_STAR	0x20a
 #define QT_KEY_NUMERIC_POUND	0x20b
 #define QT_KEY_NUMERIC_A		0x20c	/* Phone key A - HUT Telephony 0xb9 */
 #define QT_KEY_NUMERIC_B		0x20d
 #define QT_KEY_NUMERIC_C		0x20e
 #define QT_KEY_NUMERIC_D		0x20f
 
 #define QT_KEY_CAMERA_FOCUS	0x210
 #define QT_KEY_WPS_BUTTON		0x211	/* WiFi Protected Setup key */
 
 #define QT_KEY_TOUCHPAD_TOGGLE	0x212	/* Request switch touchpad on or off */
 #define QT_KEY_TOUCHPAD_ON		0x213
 #define QT_KEY_TOUCHPAD_OFF	0x214
 
 #define QT_KEY_CAMERA_ZOOMIN	0x215
 #define QT_KEY_CAMERA_ZOOMOUT	0x216
 #define QT_KEY_CAMERA_UP		0x217
 #define QT_KEY_CAMERA_DOWN		0x218
 #define QT_KEY_CAMERA_LEFT		0x219
 #define QT_KEY_CAMERA_RIGHT	0x21a
 
 #define QT_KEY_ATTENDANT_ON	0x21b
 #define QT_KEY_ATTENDANT_OFF	0x21c
 #define QT_KEY_ATTENDANT_TOGGLE	0x21d	/* Attendant call on or off */
 #define QT_KEY_LIGHTS_TOGGLE	0x21e	/* Reading light on or off */
 
 #define QT_BTN_DPAD_UP		0x220
 #define QT_BTN_DPAD_DOWN		0x221
 #define QT_BTN_DPAD_LEFT		0x222
 #define QT_BTN_DPAD_RIGHT		0x223
 
 #define QT_KEY_ALS_TOGGLE		0x230	/* Ambient light sensor */
 #define QT_KEY_ROTATE_LOCK_TOGGLE	0x231	/* Display rotation lock */
 #define QT_KEY_REFRESH_RATE_TOGGLE	0x232	/* Display refresh rate toggle */
 
 #define QT_KEY_BUTTONCONFIG		0x240	/* AL Button Configuration */
 #define QT_KEY_TASKMANAGER		0x241	/* AL Task/Project Manager */
 #define QT_KEY_JOURNAL		0x242	/* AL Log/Journal/Timecard */
 #define QT_KEY_CONTROLPANEL		0x243	/* AL Control Panel */
 #define QT_KEY_APPSELECT		0x244	/* AL Select Task/Application */
 #define QT_KEY_SCREENSAVER		0x245	/* AL Screen Saver */
 #define QT_KEY_VOICECOMMAND		0x246	/* Listening Voice Command */
 #define QT_KEY_ASSISTANT		0x247	/* AL Context-aware desktop assistant */
 #define QT_KEY_KBD_LAYOUT_NEXT	0x248	/* AC Next Keyboard Layout Select */
 #define QT_KEY_EMOJI_PICKER	0x249	/* Show/hide emoji picker (HUTRR101) */
 #define QT_KEY_DICTATE		0x24a	/* Start or Stop Voice Dictation Session (HUTRR99) */
 #define QT_KEY_CAMERA_ACCESS_ENABLE	0x24b	/* Enables programmatic access to camera devices. (HUTRR72) */
 #define QT_KEY_CAMERA_ACCESS_DISABLE	0x24c	/* Disables programmatic access to camera devices. (HUTRR72) */
 #define QT_KEY_CAMERA_ACCESS_TOGGLE	0x24d	/* Toggles the current state of the camera access control. (HUTRR72) */
 #define QT_KEY_ACCESSIBILITY		0x24e	/* Toggles the system bound accessibility UI/command (HUTRR116) */
 #define QT_KEY_DO_NOT_DISTURB		0x24f	/* Toggles the system-wide "Do Not Disturb" control (HUTRR94)*/
 
 #define QT_KEY_BRIGHTNESS_MIN		0x250	/* Set Brightness to Minimum */
 #define QT_KEY_BRIGHTNESS_MAX		0x251	/* Set Brightness to Maximum */
 
 #define QT_KEY_KBDINPUTASSIST_PREV		0x260
 #define QT_KEY_KBDINPUTASSIST_NEXT		0x261
 #define QT_KEY_KBDINPUTASSIST_PREVGROUP		0x262
 #define QT_KEY_KBDINPUTASSIST_NEXTGROUP		0x263
 #define QT_KEY_KBDINPUTASSIST_ACCEPT		0x264
 #define QT_KEY_KBDINPUTASSIST_CANCEL		0x265
 
 /* Diagonal movement keys */
 #define QT_KEY_RIGHT_UP			0x266
 #define QT_KEY_RIGHT_DOWN			0x267
 #define QT_KEY_LEFT_UP			0x268
 #define QT_KEY_LEFT_DOWN			0x269
 
 #define QT_KEY_ROOT_MENU			0x26a /* Show Device's Root Menu */
 /* Show Top Menu of the Media (e.g. DVD) */
 #define QT_KEY_MEDIA_TOP_MENU		0x26b
 #define QT_KEY_NUMERIC_11			0x26c
 #define QT_KEY_NUMERIC_12			0x26d
 /*
  * Toggle Audio Description: refers to an audio service that helps blind and
  * visually impaired consumers understand the action in a program. Note: in
  * some countries this is referred to as "Video Description".
  */
 #define QT_KEY_AUDIO_DESC			0x26e
 #define QT_KEY_3D_MODE			0x26f
 #define QT_KEY_NEXT_FAVORITE		0x270
 #define QT_KEY_STOP_RECORD			0x271
 #define QT_KEY_PAUSE_RECORD		0x272
 #define QT_KEY_VOD				0x273 /* Video on Demand */
 #define QT_KEY_UNMUTE			0x274
 #define QT_KEY_FASTREVERSE			0x275
 #define QT_KEY_SLOWREVERSE			0x276
 /*
  * Control a data application associated with the currently viewed channel,
  * e.g. teletext or data broadcast application (MHEG, MHP, HbbTV, etc.)
  */
 #define QT_KEY_DATA			0x277
 #define QT_KEY_ONSCREEN_KEYBOARD		0x278
 /* Electronic privacy screen control */
 #define QT_KEY_PRIVACY_SCREEN_TOGGLE	0x279
 
 /* Select an area of screen to be copied */
 #define QT_KEY_SELECTIVE_SCREENSHOT	0x27a
 
 /* Move the focus to the next or previous user controllable element within a UI container */
 #define QT_KEY_NEXT_ELEMENT               0x27b
 #define QT_KEY_PREVIOUS_ELEMENT           0x27c
 
 /* Toggle Autopilot engagement */
 #define QT_KEY_AUTOPILOT_ENGAGE_TOGGLE    0x27d
 
 /* Shortcut Keys */
 #define QT_KEY_MARK_WAYPOINT              0x27e
 #define QT_KEY_SOS                                0x27f
 #define QT_KEY_NAV_CHART                  0x280
 #define QT_KEY_FISHING_CHART              0x281
 #define QT_KEY_SINGLE_RANGE_RADAR         0x282
 #define QT_KEY_DUAL_RANGE_RADAR           0x283
 #define QT_KEY_RADAR_OVERLAY              0x284
 #define QT_KEY_TRADITIONAL_SONAR          0x285
 #define QT_KEY_CLEARVU_SONAR              0x286
 #define QT_KEY_SIDEVU_SONAR               0x287
 #define QT_KEY_NAV_INFO                   0x288
 #define QT_KEY_BRIGHTNESS_MENU            0x289
 
 /*
  * Some keyboards have keys which do not have a defined meaning, these keys
  * are intended to be programmed / bound to macros by the user. For most
  * keyboards with these macro-keys the key-sequence to inject, or action to
  * take, is all handled by software on the host side. So from the kernel's
  * point of view these are just normal keys.
  *
  * The QT_KEY_MACRO# codes below are intended for such keys, which may be labeled
  * e.g. G1-G18, or S1 - S30. The QT_KEY_MACRO# codes MUST NOT be used for keys
  * where the marking on the key does indicate a defined meaning / purpose.
  *
  * The QT_KEY_MACRO# codes MUST also NOT be used as fallback for when no existing
  * QT_KEY_FOO define matches the marking / purpose. In this case a new QT_KEY_FOO
  * define MUST be added.
  */
 #define QT_KEY_MACRO1			0x290
 #define QT_KEY_MACRO2			0x291
 #define QT_KEY_MACRO3			0x292
 #define QT_KEY_MACRO4			0x293
 #define QT_KEY_MACRO5			0x294
 #define QT_KEY_MACRO6			0x295
 #define QT_KEY_MACRO7			0x296
 #define QT_KEY_MACRO8			0x297
 #define QT_KEY_MACRO9			0x298
 #define QT_KEY_MACRO10			0x299
 #define QT_KEY_MACRO11			0x29a
 #define QT_KEY_MACRO12			0x29b
 #define QT_KEY_MACRO13			0x29c
 #define QT_KEY_MACRO14			0x29d
 #define QT_KEY_MACRO15			0x29e
 #define QT_KEY_MACRO16			0x29f
 #define QT_KEY_MACRO17			0x2a0
 #define QT_KEY_MACRO18			0x2a1
 #define QT_KEY_MACRO19			0x2a2
 #define QT_KEY_MACRO20			0x2a3
 #define QT_KEY_MACRO21			0x2a4
 #define QT_KEY_MACRO22			0x2a5
 #define QT_KEY_MACRO23			0x2a6
 #define QT_KEY_MACRO24			0x2a7
 #define QT_KEY_MACRO25			0x2a8
 #define QT_KEY_MACRO26			0x2a9
 #define QT_KEY_MACRO27			0x2aa
 #define QT_KEY_MACRO28			0x2ab
 #define QT_KEY_MACRO29			0x2ac
 #define QT_KEY_MACRO30			0x2ad
 
 /*
  * Some keyboards with the macro-keys described above have some extra keys
  * for controlling the host-side software responsible for the macro handling:
  * -A macro recording start/stop key. Note that not all keyboards which emit
  *  QT_KEY_MACRO_RECORD_START will also emit QT_KEY_MACRO_RECORD_STOP if
  *  QT_KEY_MACRO_RECORD_STOP is not advertised, then QT_KEY_MACRO_RECORD_START
  *  should be interpreted as a recording start/stop toggle;
  * -Keys for switching between different macro (pre)sets, either a key for
  *  cycling through the configured presets or keys to directly select a preset.
  */
 #define QT_KEY_MACRO_RECORD_START		0x2b0
 #define QT_KEY_MACRO_RECORD_STOP		0x2b1
 #define QT_KEY_MACRO_PRESET_CYCLE		0x2b2
 #define QT_KEY_MACRO_PRESET1		0x2b3
 #define QT_KEY_MACRO_PRESET2		0x2b4
 #define QT_KEY_MACRO_PRESET3		0x2b5
 
 /*
  * Some keyboards have a buildin LCD panel where the contents are controlled
  * by the host. Often these have a number of keys directly below the LCD
  * intended for controlling a menu shown on the LCD. These keys often don't
  * have any labeling so we just name them QT_KEY_KBD_LCD_MENU#
  */
 #define QT_KEY_KBD_LCD_MENU1		0x2b8
 #define QT_KEY_KBD_LCD_MENU2		0x2b9
 #define QT_KEY_KBD_LCD_MENU3		0x2ba
 #define QT_KEY_KBD_LCD_MENU4		0x2bb
 #define QT_KEY_KBD_LCD_MENU5		0x2bc
 
 #define QT_BTN_TRIGGER_HAPPY		0x2c0
 #define QT_BTN_TRIGGER_HAPPY1		0x2c0
 #define QT_BTN_TRIGGER_HAPPY2		0x2c1
 #define QT_BTN_TRIGGER_HAPPY3		0x2c2
 #define QT_BTN_TRIGGER_HAPPY4		0x2c3
 #define QT_BTN_TRIGGER_HAPPY5		0x2c4
 #define QT_BTN_TRIGGER_HAPPY6		0x2c5
 #define QT_BTN_TRIGGER_HAPPY7		0x2c6
 #define QT_BTN_TRIGGER_HAPPY8		0x2c7
 #define QT_BTN_TRIGGER_HAPPY9		0x2c8
 #define QT_BTN_TRIGGER_HAPPY10		0x2c9
 #define QT_BTN_TRIGGER_HAPPY11		0x2ca
 #define QT_BTN_TRIGGER_HAPPY12		0x2cb
 #define QT_BTN_TRIGGER_HAPPY13		0x2cc
 #define QT_BTN_TRIGGER_HAPPY14		0x2cd
 #define QT_BTN_TRIGGER_HAPPY15		0x2ce
 #define QT_BTN_TRIGGER_HAPPY16		0x2cf
 #define QT_BTN_TRIGGER_HAPPY17		0x2d0
 #define QT_BTN_TRIGGER_HAPPY18		0x2d1
 #define QT_BTN_TRIGGER_HAPPY19		0x2d2
 #define QT_BTN_TRIGGER_HAPPY20		0x2d3
 #define QT_BTN_TRIGGER_HAPPY21		0x2d4
 #define QT_BTN_TRIGGER_HAPPY22		0x2d5
 #define QT_BTN_TRIGGER_HAPPY23		0x2d6
 #define QT_BTN_TRIGGER_HAPPY24		0x2d7
 #define QT_BTN_TRIGGER_HAPPY25		0x2d8
 #define QT_BTN_TRIGGER_HAPPY26		0x2d9
 #define QT_BTN_TRIGGER_HAPPY27		0x2da
 #define QT_BTN_TRIGGER_HAPPY28		0x2db
 #define QT_BTN_TRIGGER_HAPPY29		0x2dc
 #define QT_BTN_TRIGGER_HAPPY30		0x2dd
 #define QT_BTN_TRIGGER_HAPPY31		0x2de
 #define QT_BTN_TRIGGER_HAPPY32		0x2df
 #define QT_BTN_TRIGGER_HAPPY33		0x2e0
 #define QT_BTN_TRIGGER_HAPPY34		0x2e1
 #define QT_BTN_TRIGGER_HAPPY35		0x2e2
 #define QT_BTN_TRIGGER_HAPPY36		0x2e3
 #define QT_BTN_TRIGGER_HAPPY37		0x2e4
 #define QT_BTN_TRIGGER_HAPPY38		0x2e5
 #define QT_BTN_TRIGGER_HAPPY39		0x2e6
 #define QT_BTN_TRIGGER_HAPPY40		0x2e7
 
 /* We avoid low common keys in module aliases so they don't get huge. */
 #define QT_KEY_MIN_INTERESTING	QT_KEY_MUTE
 #define QT_KEY_MAX			0x2ff
 #define QT_KEY_CNT			(QT_KEY_MAX+1)
 
 /*
  * Relative axes
  */
 
 #define QT_REL_X			0x00
 #define QT_REL_Y			0x01
 #define QT_REL_Z			0x02
 #define QT_REL_RX			0x03
 #define QT_REL_RY			0x04
 #define QT_REL_RZ			0x05
 #define QT_REL_HWHEEL		0x06
 #define QT_REL_DIAL		0x07
 #define QT_REL_WHEEL		0x08
 #define QT_REL_MISC		0x09
 /*
  * 0x0a is reserved and should not be used in input drivers.
  * It was used by HID as QT_REL_MISC+1 and userspace needs to detect if
  * the next QT_REL_* event is correct or is just QT_REL_MISC + n.
  * We define here QT_REL_RESERVED so userspace can rely on it and detect
  * the situation described above.
  */
 #define QT_REL_RESERVED		0x0a
 #define QT_REL_WHEEL_HI_RES	0x0b
 #define QT_REL_HWHEEL_HI_RES	0x0c
 #define QT_REL_MAX			0x0f
 #define QT_REL_CNT			(QT_REL_MAX+1)
 
 /*
  * Absolute axes
  */
 
 #define QT_ABS_X			0x00
 #define QT_ABS_Y			0x01
 #define QT_ABS_Z			0x02
 #define QT_ABS_RX			0x03
 #define QT_ABS_RY			0x04
 #define QT_ABS_RZ			0x05
 #define QT_ABS_THROTTLE		0x06
 #define QT_ABS_RUDDER		0x07
 #define QT_ABS_WHEEL		0x08
 #define QT_ABS_GAS			0x09
 #define QT_ABS_BRAKE		0x0a
 #define QT_ABS_HAT0X		0x10
 #define QT_ABS_HAT0Y		0x11
 #define QT_ABS_HAT1X		0x12
 #define QT_ABS_HAT1Y		0x13
 #define QT_ABS_HAT2X		0x14
 #define QT_ABS_HAT2Y		0x15
 #define QT_ABS_HAT3X		0x16
 #define QT_ABS_HAT3Y		0x17
 #define QT_ABS_PRESSURE		0x18
 #define QT_ABS_DISTANCE		0x19
 #define QT_ABS_TILT_X		0x1a
 #define QT_ABS_TILT_Y		0x1b
 #define QT_ABS_TOOL_WIDTH		0x1c
 
 #define QT_ABS_VOLUME		0x20
 #define QT_ABS_PROFILE		0x21
 
 #define QT_ABS_MISC		0x28
 
 /*
  * 0x2e is reserved and should not be used in input drivers.
  * It was used by HID as QT_ABS_MISC+6 and userspace needs to detect if
  * the next QT_ABS_* event is correct or is just QT_ABS_MISC + n.
  * We define here QT_ABS_RESERVED so userspace can rely on it and detect
  * the situation described above.
  */
 #define QT_ABS_RESERVED		0x2e
 
 #define QT_ABS_MT_SLOT		0x2f	/* MT slot being modified */
 #define QT_ABS_MT_TOUCH_MAJOR	0x30	/* Major axis of touching ellipse */
 #define QT_ABS_MT_TOUCH_MINOR	0x31	/* Minor axis (omit if circular) */
 #define QT_ABS_MT_WIDTH_MAJOR	0x32	/* Major axis of approaching ellipse */
 #define QT_ABS_MT_WIDTH_MINOR	0x33	/* Minor axis (omit if circular) */
 #define QT_ABS_MT_ORIENTATION	0x34	/* Ellipse orientation */
 #define QT_ABS_MT_POSITION_X	0x35	/* Center X touch position */
 #define QT_ABS_MT_POSITION_Y	0x36	/* Center Y touch position */
 #define QT_ABS_MT_TOOL_TYPE	0x37	/* Type of touching device */
 #define QT_ABS_MT_BLOB_ID		0x38	/* Group a set of packets as a blob */
 #define QT_ABS_MT_TRACKING_ID	0x39	/* Unique ID of initiated contact */
 #define QT_ABS_MT_PRESSURE		0x3a	/* Pressure on contact area */
 #define QT_ABS_MT_DISTANCE		0x3b	/* Contact hover distance */
 #define QT_ABS_MT_TOOL_X		0x3c	/* Center X tool position */
 #define QT_ABS_MT_TOOL_Y		0x3d	/* Center Y tool position */
 
 
 #define QT_ABS_MAX			0x3f
 #define QT_ABS_CNT			(QT_ABS_MAX+1)
 
 /*
  * Switch events
  */
 
 #define QT_SW_LID			0x00  /* set = lid shut */
 #define QT_SW_TABLET_MODE		0x01  /* set = tablet mode */
 #define QT_SW_HEADPHONE_INSERT	0x02  /* set = inserted */
 #define QT_SW_RFKILL_ALL		0x03  /* rfkill master switch, type "any"
                      set = radio enabled */
 #define QT_SW_RADIO		QT_SW_RFKILL_ALL	/* deprecated */
 #define QT_SW_MICROPHONE_INSERT	0x04  /* set = inserted */
 #define QT_SW_DOCK			0x05  /* set = plugged into dock */
 #define QT_SW_LINEOUT_INSERT	0x06  /* set = inserted */
 #define QT_SW_JACK_PHYSICAL_INSERT 0x07  /* set = mechanical switch set */
 #define QT_SW_VIDEOOUT_INSERT	0x08  /* set = inserted */
 #define QT_SW_CAMERA_LENS_COVER	0x09  /* set = lens covered */
 #define QT_SW_KEYPAD_SLIDE		0x0a  /* set = keypad slide out */
 #define QT_SW_FRONT_PROXIMITY	0x0b  /* set = front proximity sensor active */
 #define QT_SW_ROTATE_LOCK		0x0c  /* set = rotate locked/disabled */
 #define QT_SW_LINEIN_INSERT	0x0d  /* set = inserted */
 #define QT_SW_MUTE_DEVICE		0x0e  /* set = device disabled */
 #define QT_SW_PEN_INSERTED		0x0f  /* set = pen inserted */
 #define QT_SW_MACHINE_COVER	0x10  /* set = cover closed */
 #define QT_SW_MAX			0x10
 #define QT_SW_CNT			(QT_SW_MAX+1)
 
 /*
  * Misc events
  */
 
 #define QT_MSC_SERIAL		0x00
 #define QT_MSC_PULSELED		0x01
 #define QT_MSC_GESTURE		0x02
 #define QT_MSC_RAW			0x03
 #define QT_MSC_SCAN		0x04
 #define QT_MSC_TIMESTAMP		0x05
 #define QT_MSC_MAX			0x07
 #define QT_MSC_CNT			(QT_MSC_MAX+1)
 
 /*
  * LEDs
  */
 
 #define LED_NUML		0x00
 #define LED_CAPSL		0x01
 #define LED_SCROLLL		0x02
 #define LED_COMPOSE		0x03
 #define LED_KANA		0x04
 #define LED_SLEEP		0x05
 #define LED_SUSPEND		0x06
 #define LED_MUTE		0x07
 #define LED_MISC		0x08
 #define LED_MAIL		0x09
 #define LED_CHARGING		0x0a
 #define LED_MAX			0x0f
 #define LED_CNT			(LED_MAX+1)
 
 /*
  * Autorepeat values
  */
 
 #define QT_REP_DELAY		0x00
 #define QT_REP_PERIOD		0x01
 #define QT_REP_MAX			0x01
 #define QT_REP_CNT			(QT_REP_MAX+1)
 
 /*
  * Sounds
  */
 
 #define QT_SND_CLICK		0x00
 #define QT_SND_BELL		0x01
 #define QT_SND_TONE		0x02
 #define QT_SND_MAX			0x07
 #define QT_SND_CNT			(QT_SND_MAX+1)
 
 #endif
 