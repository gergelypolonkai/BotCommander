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
 *            history.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Handles tabs' command line history
 }}} */

/* {{{ Includes */
#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>

#include "typedefs.h"
#include "variables.h"
/* }}} */

/* {{{ history_add() */
void
history_add(t_tab_data *tab, gchar mode, gchar *line)
{
	t_history_data *temp;

	if (config_data.history_len == 0)
		return;

	if (tab)
	{
		gint count = 0;
		char modechar = 0;

		switch (tolower(mode))
		{
			case 'b':
				modechar = '/';
				break;
			case 'e':
				modechar = '.';
				break;
			case 'm':
				modechar = '@';
				break;
		}

		if (modechar == 0)
			return;

		temp = tab->history;
		while (temp)
		{
			count++;
			temp = temp->next;
		}

		if (count == config_data.history_len)
		{
			temp = tab->history;
			tab->history = temp->next;
			g_free(temp->line);
			g_free(temp);
		}

		temp = g_malloc(sizeof(t_history_data));
		temp->line = g_malloc(strlen(line) + 2);
		sprintf(temp->line, "%c%s", modechar, line);
		temp->next = NULL;

		if (tab->history == NULL)
			tab->history = temp;
		else
		{
			t_history_data *t = tab->history;
			while (t->next)
				t = t->next;
			t->next = temp;
		}
	}
}
/* }}} */

/* {{{ get_history_len() */
gint
get_history_len(t_tab_data *tab)
{

	if (tab)
	{
		t_history_data *temp = tab->history;
		gint count = 0;

		if (tab->history == NULL)
			return 0;
		while (temp)
		{
			count++;
			temp = temp->next;
		}

		return count;
	}

	return 0;
}
/* }}} */

