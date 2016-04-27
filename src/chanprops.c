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
 *            chanprops.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * This holds functions which belong to the Channel properties dialog
 }}} */

/* {{{ Includes */
#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>

#include "functions.h"
#include "typedefs.h"
/* }}} */

/* {{{ find_chan_in_list() */
gint
find_chan_in_list(gconstpointer a, gconstpointer b)
{
	t_channel *chanrec = (t_channel *)a;
	gchar *channame = (gchar *)b;

	if (streq(chanrec->name, channame, FALSE))
		return 0;

	return 1;
}
/* }}} */

/* {{{ find_chan_property() */
gint
find_chan_property(gconstpointer a, gconstpointer b)
{
	t_chan_prop *prop = (t_chan_prop *)a;
	gchar *propname = (gchar *)b;

	return strcmp(prop->name, propname);
}
/* }}} */

/* {{{ update_channel_property() */
void
update_channel_property(t_tab_data *tab, gchar *chan, gchar *property)
{
	GList *this_chan;

	if ((this_chan = g_list_find_custom(tab->actual_channel_list, chan, find_chan_in_list)))
	{
		t_chan_prop *prop;
		GList *this_prop;
		t_channel *chanrec = this_chan->data;

		switch (tab->chanprops_list_num)
		{
			case 1:
				/* Channel modes */
				break;
			case 2:
				/* idle-kick */
				break;
			case 3:
				/* stopnethack-mode */
				break;
			case 4:
				/* revenge-mode */
				break;
		}

		prop = g_malloc0(sizeof(t_chan_prop));

		prop->bool_prop = ((*property == '+') || (*property == '-'));

		Context;

		if (prop->bool_prop)
		{
			prop->name = g_strdup(property + 1);
			prop->bool_value = (*property == '+') ? TRUE : FALSE;
			prop->string_value = NULL;
		}
		else
		{
			gchar *a;

			Context;

			for (a = property; *a; a++)
				if (isspace(*a))
					break;

			if (*a && (a - property > 0))
			{
				prop->name = g_strndup(property, a - property);
				for (; *a; a++)
					if (!isspace(*a))
						break;
				prop->string_value = g_strdup(a);
			}

			Context;
		}

		if (prop->name)
		{
			this_prop = g_list_find_custom(chanrec->properties, prop->name, find_chan_property);
			if (this_prop)
			{
				printf("Updating %s property \"%s\" with value %s\n", (prop->bool_prop) ? "boolean" : "string", prop->name, (prop->bool_prop) ? ((prop->bool_value) ? "TRUE" : "FALSE") : prop->string_value);
			}
			else
			{
				printf("Adding %s property \"%s\" with value %s\n", (prop->bool_prop) ? "boolean" : "string", prop->name, (prop->bool_prop) ? ((prop->bool_value) ? "TRUE" : "FALSE") : prop->string_value);
			}
		}
	}
}
/* }}} */

