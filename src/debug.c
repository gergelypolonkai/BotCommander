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
 *            debug.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Debugging functions
 }}} */

#ifdef DEBUG

/* {{{ Includes */
# include <signal.h>
# include <gtk/gtk.h>
# include <stdlib.h>
/* }}} */

/* {{{ Variables */
char *last_file = NULL;
int last_line = 0;
/* }}} */

/* {{{ fatal_signal() */
void fatal_signal_handler(int sig)
{
	if (last_file)
		fprintf(stderr, "Fatal signal cought (%d). Last context was at %s:%d\n", sig, last_file, last_line);
	exit(1);
}
/* }}} */

/* {{{ debug_init() */
void
debug_init()
{
	signal(SIGSEGV, fatal_signal_handler);
}
/* }}} */

/* {{{ set_context() */
void
set_context(char *file, int line)
{
	if (last_file)
		g_free(last_file);
	last_file = g_strdup(file);
	last_line = line;
}
/* }}} */

#endif

