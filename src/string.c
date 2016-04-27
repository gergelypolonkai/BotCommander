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
 *            string.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * String manipulation functions
 }}} */

/* TODO: Insert this into one of the makefiles, perhaps in rules? */
#define _GNU_SOURCE
/* {{{ Includes */
#include <gtk/gtk.h>
#include <string.h>
#include <ctype.h>
#include <iconv.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "defines.h"
/* }}} */

/* {{{ streq() */
gboolean
streq(gchar *s1, gchar *s2, gboolean cs)
{
	gchar *a;

	if (!s1 && !s2)
		return TRUE;
	if (!s1 || !s2)
		return FALSE;
	if (strlen(s1) != strlen(s2))
		return FALSE;
	if (cs)
		return strcmp(s1, s2) == 0 ? TRUE : FALSE;
	for (a = s1; *a; a++)
		if (tolower(*a) != tolower(*(s2 + (a - s1))))
			return FALSE;

	return TRUE;
}
/* }}} */

/* {{{ wordwrap() */
int
wordwrap(gchar *sentence, gchar *delim, gchar ***retval, gint *wnum)
{
	gboolean prev_delim = TRUE;
	gchar *a;
	gint words = 0;
	gchar **starts;
	gchar **newstarts;
	gint *lens;
	signed int cword = -1;
	int i;

	for (a = sentence; *a; a++)
	{
		gboolean is_delim = (strchr(delim, *a)) ? TRUE : FALSE;

		if (!is_delim && prev_delim)
			words++;
		prev_delim = is_delim;
	}

	if (wnum)
		*wnum = words;

	if (words == 0)
	{
		if (retval)
			*retval = NULL;
		return BCE_SUCCESS;
	}

	if (!retval)
		return BCE_SUCCESS;

	if ((starts = (gchar **)g_malloc(sizeof(gchar *))) == NULL)
		return BCE_NOMEM;
	if ((lens = (gint *)g_malloc(sizeof(gint))) == NULL)
	{
		g_free(starts);
		return BCE_NOMEM;
	}
	if ((newstarts = (gchar **)g_malloc(sizeof(gchar *))) == NULL)
	{
		g_free(starts);
		g_free(lens);
		return BCE_NOMEM;
	}

	prev_delim = TRUE;

	for (a = sentence; *a; a++)
	{
		gboolean is_delim = (strchr(delim, *a)) ? TRUE : FALSE;

		if (!is_delim)
		{
			if (prev_delim)
			{
				cword++;
				starts[cword] = a;
				lens[cword] = 0;
			}
			else
				lens[cword]++;
		}
		prev_delim = is_delim;
	}

	for (i = 0; i < words; i++)
	{
		if ((newstarts[i] = g_malloc((sizeof(gchar) * (lens[i] + 2)))) == NULL)
		{
			int j;

			for (j = 0; j < i; j++)
				g_free(newstarts[i]);
			g_free(starts);
			g_free(lens);
			g_free(newstarts);
			return BCE_NOMEM;
		}
		memset(newstarts[i], 0, (sizeof(gchar) * (lens[i] + 2)));
		strncpy(newstarts[i], starts[i], lens[i] + 1);
	}
	g_free(lens);
	g_free(starts);

	*retval = newstarts;

	return BCE_SUCCESS;
}
/* }}} */

/* {{{ wl_free() */
void
wl_free(gchar ***owl, gint wnum)
{
	gint i;

	if (*owl)
	{
		for (i = 0; i < wnum; i++)
			g_free((*owl)[i]);
		g_free(*owl);
	}
}
/* }}} */

/* {{{ is_numeric() */
gboolean
is_numeric(gchar *s, gint *retval)
{
	gchar *a;

	if (!s)
		return FALSE;
	for (a = s; *a; a++)
		if (!isdigit(*a))
			return FALSE;

	if (retval)
		*retval = atoi(s);

	return TRUE;
}
/* }}} */

/* {{{ trim() */
gchar *
trim(gchar *s)
{
	gchar *start;
	gchar *end;
	gchar *ret;

	if (s == NULL)
		return NULL;
	if (*s == 0)
		return "";

	for (start = s; *start; start++)
		if (!isspace(*start))
			break;

	if (!*start)
		return "";

	for (end = s + strlen(s) - 1; end >= s; end--)
		if (!isspace(*end))
			break;

	ret = g_malloc((end - start) + 2);
	memset(ret, 0, (end - start) + 2);
	strncpy(ret, start, (end - start) + 1);

	return ret;
}
/* }}} */

// Under FreeBSD, dprintf doesn't exist :(
#ifndef dprintf
/* {{{ dprintf() */
int
dprintf(int fd, const char *format, ...)
{
	va_list ap;
	char *buf;
	int len;

	va_start(ap, format);
	vasprintf(&buf, format, ap);
	len = strlen(buf);
	write(fd, buf, len);
	g_free(buf);
	va_end(ap);

	return len;
}
/* }}} */
#endif

