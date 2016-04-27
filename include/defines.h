/* vim: set foldmethod=marker : */
/* {{{ Legal info
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 }}} */
/* {{{ Author and file info
 *            defines.h
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Macro definitions used in many places
 }}} */

/* {{{ _BC_DEFINES_H */
#ifndef _BC_DEFINES_H
# define _BC_DEFINES_H

/* {{{ Locale-related definition. It's much easier this way */
#define _(String) gettext(String)
/* }}} */

/* {{{ BC_COMMAND prototype definition */
#define BC_COMMAND(x) gboolean x (t_tab_data *tab, int paramnum, gchar **param)
/* In typedefs.h there is a struct which is for the command list.
 * To make life easier, you only have to change the functions' type here (and
 * in the previous line).
 */
#define BC_COMMAND_DEF(x) gboolean (* x )(t_tab_data *, int, gchar **)
/* }}} */

/* {{{ Command types */
#define CT_INTERNAL 0
#define CT_MODULE   1
#define CT_SCRIPT   2
/* }}} */

/* {{{ BotCommander error constants */
#define BCE_SUCCESS 0
#define BCE_BOT_EXISTS 1
#define BCE_NOMEM 2
#define BCE_NOBOT 3
#define BCE_NOPROTO 4
#define BCE_CANTRESOLV 5
#define BCE_CANTCONNECT 6
/* }}} */

/* {{{ The eggdrops character for eggdrop commands. You should never change this! */
#define EGG_CMD_CHAR "."
/* }}} */

/* {{{ The telnet codes we use, from RFC 854) */
#define TELNET_IAC  "\xff" /* Interpret As Command */
#define TELNET_WILL "\xfb" /* Will */
#define TELNET_WONT "\xfc" /* Won't */
#define TELNET_ECHO "\x01" /* Echo */

#define TELNET_WILL_ECHO TELNET_IAC TELNET_WILL TELNET_ECHO
#define TELNET_WONT_ECHO TELNET_IAC TELNET_WONT TELNET_ECHO
/* }}} */

#endif
/* }}} */

