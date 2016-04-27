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
 *            bots.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Bot list handling
 }}} */

/* TODO: Put this into a Makefile */
#define _GNU_SOURCE
/* {{{ Includes */
#include <libintl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "config.h"

#include "typedefs.h"
#include "functions.h"
#include "variables.h"
/* }}} */

/* {{{ Variables */
GList *internal_bot_list;
/* }}} */

#ifdef DEBUG
/* {{{ print_this_bot() */
void print_this_bot(gpointer data, gpointer user_data)
{
	t_bot_data *temp = (t_bot_data *)data;

	printf("%s:%s:%d:%s\n", temp->botname, temp->host, temp->port, temp->handle);
}
/* }}} */

/* {{{ list_bots() */
void
list_bots(void)
{
	g_list_foreach(internal_bot_list, print_this_bot, NULL);
}
/* }}} */
#endif

/* {{{ search_function() */
static gint
search_function(gconstpointer a, gconstpointer b)
{
	t_bot_data *bot = (t_bot_data *)a;
	char *botname = (char *)b;

	return strcmp(bot->botname, botname);
}
/* }}} */

/* {{{ add_bot() */
gint
add_bot(gchar *botname, gchar *host, guint port, gchar *handle)
{
	t_bot_data *temp;

	if (g_list_find_custom(internal_bot_list, botname, search_function))
	{
		// TODO: Here we should notify the user that this bot already exists.
#ifdef DEBUG
		if (config_data.debug)
			fprintf(stderr, _("There is already a bot named %s in the list.\n"), botname);
#endif /* DEBUG */
		return BCE_BOT_EXISTS;
	}

#ifdef DEBUG
	printf("Adding bot %s at %s:%d with user %s\n", botname, host, port, handle);
#endif

	temp = g_malloc(sizeof(t_bot_data));

	temp->botname = g_strdup(botname);
	temp->host = g_strdup(host);
	temp->port = port;
	temp->handle = g_strdup(handle);

	internal_bot_list = g_list_append(internal_bot_list, temp);

	return BCE_SUCCESS;
}
/* }}} */

/* {{{ del_bot() */
gint
del_bot(gchar *botname)
{
	return BCE_SUCCESS;
}
/* }}} */

/* {{{ get_bot_by_botname() */
t_bot_data *
get_bot_by_botname(gchar *name)
{
	GList *temp = g_list_find_custom(internal_bot_list, name, search_function);

	if (temp)
		return (t_bot_data *)(temp->data);
	
	return NULL;
}
/* }}} */

/* {{{ create_bot_record() */
gint
create_bot_record(gchar *host, gint port, t_bot_data **rec)
{
	t_bot_data *temp;
	
	if (rec == NULL)
		return BCE_NOMEM;

	temp = g_malloc(sizeof(t_bot_data));

	if (temp)
	{
		temp->botname = NULL;
		temp->host = g_strdup(host);
		temp->port = port;
		temp->handle = NULL;

		*rec = temp;
		return BCE_SUCCESS;
	}
	
	return BCE_NOMEM;
}
/* }}} */

/* {{{ update_bot() */
gint
update_bot(t_bot_data *bot, gchar *botname, gchar *host, guint port, gchar *handle)
{
	if (bot)
	{
		t_bot_data *temp = g_malloc(sizeof(t_bot_data));
		memset(temp, 0, sizeof(t_bot_data));
	
		if (bot->botname)
		{
			temp->botname = g_strdup(bot->botname);
			g_free(bot->botname);
		}
		if (bot->host)
		{
			temp->host = g_strdup(bot->host);
			g_free(bot->host);
		}
		temp->port = bot->port;
		if (bot->handle)
		{
			temp->handle = g_strdup(bot->handle);
			g_free(bot->handle);
		}

		bot->botname = g_strdup(botname);
		bot->host = g_strdup(host);
		bot->port = port;
		bot->handle = g_strdup(handle);

		if (temp->botname)
			g_free(temp->botname);
		if (temp->host)
			g_free(temp->host);
		if (temp->handle)
			g_free(temp->handle);
	}

	return BCE_SUCCESS;
}
/* }}} */

/* {{{ get_bot_by_name() */
gint
get_bot_by_name(gconstpointer a, gconstpointer b)
{
	GConfEntry *entry = (GConfEntry *)a;
	gchar *name = (gchar *)b;
	gchar *i;
	GConfValue *value;
	gchar *this_name;
	gchar *key;
	gint len = strlen(GCONF2_ROOT "/bots/");
	
	/* TODO: Would be easier with regex? */
	key = (gchar *)gconf_entry_get_key(entry);
	if (strncmp(key + len, "bot", 3) != 0)
		return 1;
	for (i = key + len + 3; *i && isdigit(*i); i++);
	if (*i != '_')
		return 1;
	if (strcmp(++i, "name") != 0)
		return 1;
	value = gconf_entry_get_value(entry);
	if (value->type != GCONF_VALUE_STRING)
		return 1;
	this_name = (char *)gconf_value_get_string(value);
	return strcmp(this_name, name);
}
/* }}} */

/* {{{ free_bot_data() */
void
free_bot_data(gpointer data, gpointer user_data)
{
	t_bot_data *bot = (t_bot_data *)data;

	if (bot)
	{
		if (bot->botname)
			g_free(bot->botname);
		if (bot->host)
			g_free(bot->host);
		if (bot->handle)
			g_free(bot->handle);
		g_free(bot);
	}
}
/* }}} */

/* {{{ is_this_num_bot() */
gint
is_this_num_bot(gconstpointer a, gconstpointer b)
{
	if (((t_bot_data *)a)->id == *((guint *)b))
		return 0;

	return 1;
}
/* }}} */

/* {{{ get_bot_by_conf_num() */
t_bot_data *
get_bot_by_conf_num(guint num)
{
	GList *entry;

	entry = g_list_find_custom(internal_bot_list, &num, is_this_num_bot);
	if (entry)
		return entry->data;
	return NULL;
}
/* }}} */

