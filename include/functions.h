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
 *            functions.h
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright  2004  Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Function prototypes
 }}} */

/* {{{ _BC_FUNCTIONS_H */
#ifndef _BC_FUNCTIONS_H
# define _BC_FUNCTIONS_H

/* {{{ Includes */
# include <gtk/gtk.h>
# include <stdarg.h>
# include "typedefs.h"
/* }}} */

/* {{{ widgets.c */
void set_title(t_tab_data *, char *, ...);
int create_main_window(void);
void feed_message_to_terminal(t_tab_data *, gboolean, gchar *, ...);
void feed_error_to_terminal(t_tab_data *, gboolean, gchar *, ...);
void feed_info_to_terminal(t_tab_data *, gboolean, gchar *, ...);
/* }}} */

/* {{{ callbacks.c */
void destroy_event(GtkWidget *, gpointer);
gboolean delete_event(GtkWidget *, GdkEvent *, gpointer);
void toolbar_quit(GtkButton *, gpointer);
void toolbar_add_tab(GtkButton *, gpointer);
void toolbar_close_active(GtkButton *, gpointer);
void toolbar_open_prefs(GtkButton *, gpointer);
void book_change_tab(GtkNotebook *, GtkNotebookPage *newpage, guint, gpointer);
void close_tab_cb(GtkWidget *, gpointer);
gboolean entry_activate(GtkWidget *, gpointer);
void menu_exit(GtkWidget *, gpointer);
void menu_mode_changed(GtkWidget *, gpointer);
void menu_botlist_open(GtkWidget *, gpointer);
void menu_prefs_open(GtkWidget *, gpointer);
void menu_new_tab(GtkWidget *, gpointer);
void menu_close_tab(GtkWidget *, gpointer);
void menu_about(GtkWidget *, gpointer);
int vte_clicked(GtkWidget *, gpointer);
int entry_keypress(GtkEntry *, GdkEventKey *, gpointer);
void socket_event(gpointer, gint, GdkInputCondition);
void menu_showhide_sidebar(GtkWidget *, gpointer);
/* }}} */

/* {{{ main.c */
void exit_cleanly(gboolean);
void change_mode(char, gboolean);
/* }}} */

/* {{{ tabs.c */
int add_tab(t_tab_data **);
t_tab_data *get_active_tab(void);
gboolean close_tab(t_tab_data *);
gboolean assign_bot_to_tab_by_name(t_tab_data *, gchar *);
void assign_bot_to_tab(t_tab_data *, t_bot_data *);
gint connect_tab(t_tab_data *);
void update_channel_list(t_tab_data *);
void update_user_list(t_tab_data *);
gint find_tab_by_pagenum(gconstpointer, gconstpointer);
void disconnect_tab(t_tab_data *);
void change_active_tab(gint);
/* }}} */

/* {{{ string.c */
gboolean streq(gchar *, gchar *, gboolean);
int wordwrap(gchar *, gchar *, gchar ***, gint *);
void wl_free(gchar ***, gint);
gboolean is_numeric(gchar *, gint *);
gchar *trim(gchar *);
/* }}} */

/* {{{ bots.c */
#ifdef DEBUG
void list_bots(void);
#endif
gint add_bot(gchar *, gchar *, guint, gchar *);
t_bot_data *get_bot_by_botname(gchar *);
gint create_bot_record(gchar *, gint, t_bot_data **);
gint update_bot(t_bot_data *, gchar *, gchar *, guint, gchar *);
void save_bot(t_bot_data *, t_bot_data *);
void free_bot_data(gpointer, gpointer);
t_bot_data *get_bot_by_conf_num(guint);
/* }}} */

/* {{{ commands.c */
gboolean process_commandline(gchar *, t_tab_data *);
void process_botcommander_script_commands(t_tab_data *, char *);
/* }}} */

/* {{{ config.c */
gboolean init_config(void);
void read_config_data(void);
gboolean set_int_conf_value(gchar *, gint);
gboolean set_string_conf_value(gchar *, gchar *);
void read_bot_list_from_config(void);
void save_bot_list_to_config(void);
/* }}} */

/* {{{ about.c */
void display_about_box(void);
/* }}} */

/* {{{ setup.c */
void display_setup_window(void);
/* }}} */

/* {{{ networking.c */
gint connect_to_host(gchar *, guint, int *, gint *, t_tab_data *);
void process_incoming_data(t_tab_data *, char *);
/* }}} */

/* {{{ history.c */
void history_add(t_tab_data *, gchar, gchar *);
gint get_history_len(t_tab_data *);
/* }}} */

/* {{{ botlist.c */
void display_botlist_window(void);
void create_bot_editor(t_bot_data *);
/* }}} */

/* {{{ chanprops.c */
void update_channel_property(t_tab_data *, gchar *, gchar *);
/* }}} */

/* {{{ debug.c */
/* This one is only compiled if DEBUG is defined */
# ifdef DEBUG
void debug_init(void);
void set_context(char *file, int line);
#  define Context set_context(__FILE__, __LINE__)
# else /* DEBUG */
#  define Context
# endif /* DEBUG */
/* }}} */

#endif /* _BC_FUNCTIONS_H */
/* }}} */
