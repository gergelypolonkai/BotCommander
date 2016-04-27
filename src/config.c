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
 *            config.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Configuration backend handlers
 }}} */

/* {{{ Includes */
#include <stdio.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <gconf/gconf-client.h>

#include "config.h"
#include "defines.h"
#include "typedefs.h"
#include "variables.h"
#include "functions.h"
/* }}} */

/* {{{ Defines */
#define TA_NAME 1
#define TA_HOST 2
#define TA_PORT 3
#define TA_HANDLE 4
/* }}} */

/* {{{ Variables */
GConfClient *gconf_client;
t_config_data config_data;

static gboolean gconf_inited = FALSE;
static int internal_value;
/* }}} */

/* {{{ set_gconf_values() */
gboolean
set_gconf_values(GConfClient *client)
{
	int err = 0;
	GError *error = NULL;

	error = NULL;
	gconf_client_set_float(client, GCONF2_ROOT "/background_saturation", DEFAULT_BACKGROUND_SATURATION, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_bool(client, GCONF2_ROOT "/change_to_message", DEFAULT_CHANGE_TO_MESSAGE, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_bool(client, GCONF2_ROOT "/debug", DEFAULT_DEBUG, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_string(client, GCONF2_ROOT "/default_mode", DEFAULT_DEFAULT_MODE, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_string(client, GCONF2_ROOT "/sanity", "Needed for gconf sanity check, please don't remove!", &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_bool(client, GCONF2_ROOT "/scroll_on_output", DEFAULT_SCROLL_ON_OUTPUT, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_bool(client, GCONF2_ROOT "/transparent_background", DEFAULT_TRANSPARENT_BACKGROUND, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_string(client, GCONF2_ROOT "/vte_font", DEFAULT_VTE_FONT, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_bool(client, GCONF2_ROOT "/change_to_new_tab", DEFAULT_CHANGE_TO_NEW_TAB, &error);
	if (error) err = 1;

	error = NULL;
	gconf_client_set_int(client, GCONF2_ROOT "/history_length", DEFAULT_HISTORY_LENGTH, &error);
	if (error) err = 1;

	if (err)
	{
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, _("There was an error accessing GConf2: %s"), error->message);
		gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
		gtk_dialog_run(GTK_DIALOG(dialog));

		return FALSE;
	}

	return TRUE;
}
/* }}} */

/* {{{ gconf_sanity_check_string() */
gboolean
gconf_sanity_check_string(GConfClient *client, const gchar *key)
{
	gchar *string;
	GError *error = NULL;

	string = gconf_client_get_string(client, key, &error);

	if (error)
	{
		gboolean temp = set_gconf_values(client);
		if (temp == FALSE)
			return FALSE;
	}

	if (!string)
	{
		gboolean temp = set_gconf_values(client);
		if (temp == FALSE)
			return FALSE;
	} else
		g_free(string);

	return TRUE;
}
/* }}} */

/* {{{ init_config() */
gboolean
init_config(void)
{
	GError *error = NULL;

	gconf_client = gconf_client_get_default();
	if (!gconf_sanity_check_string(gconf_client, GCONF2_ROOT "/sanity"))
		return FALSE;
	gconf_client_add_dir(gconf_client, GCONF2_ROOT, GCONF_CLIENT_PRELOAD_RECURSIVE, &error);

	return TRUE;
}
/* }}} */

/* {{{ get_boolean_conf_value() */
gboolean
get_boolean_conf_value(gchar *key, gboolean *retval)
{
	GError *error = NULL;;
	GConfValue *value = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);

	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);

	value = gconf_client_get(gconf_client, full_key, &error);
	if (error && !value)
	{
		g_free(full_key);
		return FALSE;
	}
	if (value)
	{
		if (value->type == GCONF_VALUE_BOOL)
		{
			if (retval)
				*retval = gconf_client_get_bool(gconf_client, full_key, &error);
			g_free(full_key);
			return TRUE;
		}
	}
	return FALSE;
}
/* }}} */

/* {{{ get_float_conf_value() */
gboolean
get_float_conf_value(gchar *key, gfloat *retval)
{
	GError *error = NULL;
	GConfValue *value = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;
	
	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);
	
	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);
	
	value = gconf_client_get(gconf_client, full_key, &error);
	
	if ((error != NULL) || (value == NULL)) {
		g_free(full_key);
		return FALSE;
	}

	if (value) {
		if (value->type == GCONF_VALUE_FLOAT) {
			if (retval)
				*retval = gconf_client_get_float(gconf_client, full_key, &error);
			g_free(full_key);
			return TRUE;
		}
	}
	g_free(full_key);
	return FALSE;
}
/* }}} */

/* {{{ get_int_conf_value() */
gboolean
get_int_conf_value(gchar *key, gint *retval)
{
	GError *error = NULL;
	GConfValue *value = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);

	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);

	value = gconf_client_get(gconf_client, full_key, &error);

	if ((error != NULL) || (value == NULL)) {
		g_free(full_key);
		return FALSE;
	}

	if (value) {
		if (value->type == GCONF_VALUE_INT) {
			if (retval)
				*retval = gconf_client_get_int(gconf_client, full_key, &error);
			g_free(full_key);
			return TRUE;
		}
	}
	g_free(full_key);
	return FALSE;
}
/* }}} */

/* {{{ get_string_conf_value() */
gboolean
get_string_conf_value(gchar *key, gchar **retval)
{
	GError *error = NULL;
	GConfValue *value = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);
	
	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);
	
	value = gconf_client_get(gconf_client, full_key, &error);
	
	if (error || !value) {
		g_free(full_key);
		return FALSE;
	}

	if (value) {
		if (value->type == GCONF_VALUE_STRING) {
			if (retval)
				*retval = g_strdup(gconf_client_get_string(gconf_client, full_key, &error));
			g_free(full_key);
			return TRUE;
		}
	}
	g_free(full_key);
	return FALSE;
}
/* }}} */

/* {{{ set_boolean_conf_value() */
gboolean
set_boolean_conf_value(gchar *key, gboolean value)
{
	GError *error = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);
	
	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);

	gconf_client_set_bool(gconf_client, full_key, value, &error);
	if (error)
	{
		g_free(full_key);
		return FALSE;
	}
	
	return TRUE;
}
/* }}} */

/* {{{ set_float_conf_value() */
gboolean
set_float_conf_value(gchar *key, gdouble value)
{
	GError *error = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);
	
	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);

	gconf_client_set_float(gconf_client, full_key, value, &error);
	if (error)
	{
		g_free(full_key);
		return FALSE;
	}
	
	return TRUE;
}
/* }}} */

/* {{{ set_int_conf_value() */
gboolean
set_int_conf_value(gchar *key, gint value)
{
	GError *error = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);
	
	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);

	gconf_client_set_int(gconf_client, full_key, value, &error);
	if (error)
	{
		g_free(full_key);
		return FALSE;
	}
	
	return TRUE;
}
/* }}} */

/* {{{ set_string_conf_value() */
gboolean
set_string_conf_value(gchar *key, gchar *value)
{
	GError *error = NULL;
	gchar *full_key;

	if ((full_key = g_malloc(strlen(GCONF2_ROOT) + strlen(key) + 2)) == NULL)
		return FALSE;

	memset(full_key, 0, strlen(GCONF2_ROOT) + strlen(key) + 2);
	
	strcpy(full_key, GCONF2_ROOT);
	strcat(full_key, "/");
	strcat(full_key, key);

	gconf_client_set_string(gconf_client, full_key, value, &error);
	if (error)
	{
		printf("%s\n", error->message);
		g_free(full_key);
		return FALSE;
	}
	
	return TRUE;
}
/* }}} */

/* {{{ read_config_data() */
void
read_config_data(void)
{
	gboolean setting_b;
	gfloat setting_f;
	gchar *setting_s;
	gint setting_i;

	if (get_boolean_conf_value("scroll_on_output", &setting_b))
		config_data.scroll_on_output = setting_b;
	else
		config_data.scroll_on_output = DEFAULT_SCROLL_ON_OUTPUT;

	if (get_boolean_conf_value("change_to_message", &setting_b))
		config_data.change_to_message = setting_b;
	else
		config_data.change_to_message = DEFAULT_CHANGE_TO_MESSAGE;

#ifdef DEBUG
	if (get_boolean_conf_value("debug", &setting_b))
		config_data.debug = setting_b;
	else
		config_data.debug = DEFAULT_DEBUG;
#endif /* DEBUG */

	if (get_boolean_conf_value("transparent_background", &setting_b))
		config_data.transparent_background = setting_b;
	else
		config_data.transparent_background = DEFAULT_TRANSPARENT_BACKGROUND;

	if (get_float_conf_value("background_saturation", &setting_f))
		config_data.background_saturation = setting_f;
	else
		config_data.background_saturation = DEFAULT_BACKGROUND_SATURATION;

	if (get_string_conf_value("default_mode", &setting_s))
		config_data.default_mode = *setting_s;
	else
		config_data.default_mode = *DEFAULT_DEFAULT_MODE;

	if (get_string_conf_value("vte_font", &setting_s))
		config_data.vte_font = setting_s;
	else
		config_data.vte_font = DEFAULT_VTE_FONT;

	if (get_boolean_conf_value("change_to_new_tab", &setting_b))
		config_data.change_to_new_tab = setting_b;
	else
		config_data.change_to_new_tab = DEFAULT_CHANGE_TO_NEW_TAB;

	if (get_int_conf_value("history_length", &setting_i))
		config_data.history_len = setting_i;
	else
		config_data.history_len = DEFAULT_HISTORY_LENGTH;
}
/* }}} */

/* {{{ set_bot_data_from_conf() */
void
set_bot_data_from_conf(gpointer data, gpointer discard)
{
	GConfEntry *entry = (GConfEntry *)data;
	gint len = strlen(GCONF2_ROOT "/bots/");
	gchar *key = (char *)gconf_entry_get_key(entry);
	gchar *num = NULL;
	gchar *i;
	gint bot_id;
	t_bot_data *bot;
	int to_add = 0;
	GConfValue *value;
	gchar *string_value = NULL;
	gint int_value = 0;

	if (strncmp(key + len, "bot", 3))
		return;
	num = key + len + 3;
	for (i = num; *i && isdigit(*i); i++);
	if (*i != '_')
		return;
	*i = 0;
	i++;
	bot_id = atoi((const char *)num);
	if (bot_id == 0)
		return;
	if (strcmp(i, "name") == 0)
		to_add = TA_NAME;
	else if (strcmp(i, "host") == 0)
		to_add = TA_HOST;
	else if (strcmp(i, "port") == 0)
		to_add = TA_PORT;
	else if (strcmp(i, "handle") == 0)
		to_add = TA_HANDLE;

	if (to_add == 0)
		return;

	value = gconf_entry_get_value(entry);
	if (to_add == TA_PORT)
		int_value = gconf_value_get_int((const GConfValue *)value);
	else
		string_value = (gchar *)gconf_value_get_string((const GConfValue *)value);

	if ((bot = get_bot_by_conf_num(bot_id)) == NULL)
	{
		create_bot_record(NULL, 0, &bot);
		bot->id = bot_id;
		internal_bot_list = g_list_append(internal_bot_list, bot);
	}

	switch (to_add)
	{
		case TA_NAME:
			bot->botname = g_strdup(string_value);
			break;
		case TA_HOST:
			bot->host = g_strdup(string_value);
			break;
		case TA_PORT:
			bot->port = int_value;
			break;
		case TA_HANDLE:
			bot->handle = g_strdup(string_value);
			break;
	}
}
/* }}} */

/* {{{ read_bot_list_from_config() */
void
read_bot_list_from_config(void)
{
	GSList *entries;

	if (internal_bot_list)
	{
		g_list_foreach(internal_bot_list, free_bot_data, NULL);
		g_list_free(internal_bot_list);
		internal_bot_list = NULL;
	}
	entries = gconf_client_all_entries(gconf_client, GCONF2_ROOT "/bots", NULL);
	if (entries)
	{
		g_slist_foreach(entries, set_bot_data_from_conf, NULL);
	}
}
/* }}} */

/* {{{ save_this_bot() */
void
save_this_bot(gpointer data, gpointer user_data)
{
	/* NEVER CHANGE THIS, or change the code below, either! */
	static char *config_keynames[] = { "name", "host", "port", "handle", NULL };
	gchar *key;
	gint len;
	char **a;

	internal_value++;

	for (a = config_keynames; *a; a++)
	{
		len = (int)log10((double)internal_value) + 11 + strlen(*a);
		key = g_malloc(len);

		sprintf(key, "bots/bot%d_%s", internal_value, *a);

		if (strcmp(*a, "name") == 0)
			set_string_conf_value(key, ((t_bot_data *)data)->botname);
		else if (strcmp(*a, "host") == 0)
			set_string_conf_value(key, ((t_bot_data *)data)->host);
		else if (strcmp(*a, "port") == 0)
			set_int_conf_value(key, ((t_bot_data *)data)->port);
		else if (strcmp(*a, "handle") == 0)
			set_string_conf_value(key, ((t_bot_data *)data)->handle);
		g_free(key);
	}
}
/* }}} */

/* {{{ save_bot_list_to_config() */
void
save_bot_list_to_config(void)
{
	gconf_client_recursive_unset(gconf_client, GCONF2_ROOT "/bots", GCONF_UNSET_INCLUDING_SCHEMA_NAMES, NULL);

	internal_value = 0;
	g_list_foreach(internal_bot_list, save_this_bot, NULL);
}
/* }}} */

