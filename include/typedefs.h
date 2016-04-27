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
 *            typedefs.h
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Type definitions
 }}} */

/* {{{ _BC_TYPEDEFS_H */
#ifndef _BC_TYPEDEFS_H
# define _BC_TYPEDEFS_H

/* {{{ Includes */
# include <gtk/gtk.h>

# include "defines.h"
/* }}} */

/* {{{ Bot data */
typedef struct _t_bot_data {
	/* The bot's name. NULL if now known */
	gchar *botname;
	/* The hostname of the bot */
	gchar *host;
	/* The port number of the bot */
	guint port;
	/* The handle in the bot (may be NULL) */
	gchar *handle;
	/* The config key value for this bot (used only when loading bot data from config */
	guint id;
} t_bot_data;
/* }}} */

/* {{{ History data */
typedef struct _t_history_data {
	gchar *line;
	struct _t_history_data *next;
} t_history_data;
/* }}} */

/* {{{ Tab data */
typedef struct _t_tab_data {
	/* General data */

	/* The number of the tab */
	gint num;
	/* The current mode in the tab (b|e|m)*/
	char mode;
	/* The bot assigned to this tab */
	t_bot_data *bot;
	/* The history data */
	t_history_data *history;
	/* The current position in the history list */
	gint history_position;

	/* BC script related data */

	/* Currently we are in the middle of the channel list */
	gboolean chanlist_process;
	/* Currently we are in the middle of the user list */
	gboolean userlist_process;
	/* Currently we are processing the properties of this channel */
	gchar *chanprops_process;
	/* This is number x in the property list */
	gint chanprops_list_num;
	/* The channel list */
	GList *actual_channel_list;
	/* The user list */
	GList *actual_user_list;

	/* Networking data */

	/* TRUE if the tab is connected, FALSE otherwise */
	gboolean connected;
	/* The socket descriptor */
	int sock;
	/* The GDK socket tag */
	gint socktag;
	/* Set to TRUE when the first byte arrives from the other side */
	gboolean data_arrived;

	/* Widgets */

	/* The virtual terminal */
	GtkWidget *vt;
	/* Command line */
	GtkWidget *editbox;
	/* The hbox containing the tab label and the close button */
	GtkWidget *labelbox;
	/* The tab label */
	GtkWidget *label;
	/* The close button */
	GtkWidget *closebutton;
	/* List of channels */
	GtkWidget *channel_list;
	/* List of users (handles) */
	GtkWidget *user_list;
	/* Channel store */
	GtkListStore *channel_store;
	/* User store */
	GtkListStore *user_store;
} t_tab_data;
/* }}} */

/* {{{ Configuration data */
typedef struct _t_config_data {
	/* GUI options */
	gboolean change_to_new_tab;
	/* VTE options */
	gboolean scroll_on_output;
	gboolean transparent_background;
	gfloat background_saturation;
	gchar *vte_font;
	/* General options */
	gboolean change_to_message;
#ifdef DEBUG
	gboolean debug;
#endif /* DEBUG */
	gchar default_mode;
	gboolean save_prefs;
	gint history_len;
} t_config_data;
/* }}} */

/* {{{ Command Data */
typedef struct _t_command {
	/* The command verb */
	gchar *command_verb;
	/* Type of the command: 0 - Internal; 1 - From a module; 2 - From a script */
	guint type;
	/* How many parameters do we require? */
	gint min_paramnum;
	/* Can this command be abbreviated? */
	gboolean can_abbrev;
	/* The commands function */
	BC_COMMAND_DEF(func);
} t_command;
/* }}} */

/* {{{ Channel data */
typedef struct _t_channel {
	/* The channel name */
	char *name;
	/* TRUE if then channel is active */
	gboolean active;
	/* The property list */
	GList *properties;
} t_channel;
/* }}} */

/* {{{ User data */
typedef struct _t_user {
	/* The use name */
	char *name;
} t_user;
/* }}} */

/* {{{ Channel properties */
typedef struct _t_chan_prop {
	char *name;
	gboolean bool_prop;
	gboolean bool_value;
	gchar *string_value;
} t_chan_prop;
/* }}} */

#endif
/* }}} */

