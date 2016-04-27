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
 *            callbacks.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Various widget callbacks
 }}} */

/* {{{ Includes */
#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include <stdlib.h>
#include <libintl.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include "config.h"

#include "variables.h"
#include "functions.h"
/* }}} */

/* {{{ destroy_event() */
void
destroy_event(GtkWidget *widget, gpointer data)
{
	if (widget == main_window)
		gtk_main_quit();
}
/* }}} */

/* {{{ delete_event() */
gboolean
delete_event(GtkWidget * widget, GdkEvent *event, gpointer data)
{
	exit_cleanly(FALSE);
	return FALSE;
}
/* }}} */

/* {{{ toolbar_quit() */
void
toolbar_quit(GtkButton *button, gpointer data)
{
	exit_cleanly(FALSE);
}
/* }}} */

/* {{{ toolbar_add_tab() */
void
toolbar_add_tab(GtkButton *button, gpointer data)
{
	add_tab(NULL);
}
/* }}} */

/* {{{ toolbar_close_active() */
void toolbar_close_active(GtkButton *button, gpointer data)
{
	close_tab(get_active_tab());
}
/* }}} */

/* {{{ toolbar_open_prefs() */
void
toolbar_open_prefs(GtkButton *button, gpointer data)
{
	display_setup_window();
}
/* }}} */

/* {{{ book_change_tab() */
void
book_change_tab(GtkNotebook *book, GtkNotebookPage *newpage, guint pagenum, gpointer data)
{
	GList *list;

	list = g_list_find_custom(tab_list, &pagenum, find_tab_by_pagenum);
	if (list)
	{
		t_tab_data *temp;
		gchar *buf;
		gboolean sidebar_visible;
		gboolean menuitem_state;
		GtkWidget *box = gtk_paned_get_child2(GTK_PANED(gtk_notebook_get_nth_page(GTK_NOTEBOOK(main_book), pagenum)));

		temp = list->data;
		Context;
		set_title(NULL, "BotCommander - %s", gtk_label_get_text(GTK_LABEL(temp->label)));
		Context;
		buf = g_malloc(strlen(_("Mode: %c")) + 1);
		sprintf(buf, _("Mode: %c"), toupper(temp->mode));
		gtk_label_set_text(GTK_LABEL(main_statuslabel_mode), buf);
		g_free(buf);

		menuitem_state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(main_menu_show_sidebar));
		sidebar_visible = GTK_WIDGET_VISIBLE(box);

		if (menuitem_state != sidebar_visible)
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(main_menu_show_sidebar), sidebar_visible);
	}
}
/* }}} */

/* {{{ close_tab_cb() */
void
close_tab_cb(GtkWidget *button, gpointer data)
{
	GList *to_remove = g_list_find(tab_list, data);

	if (to_remove)
		close_tab((t_tab_data *)data);
}
/* }}} */

/* {{{ entry_activate() */
gboolean
entry_activate(GtkWidget *widget, gpointer data)
{
	char this_mode;
	t_tab_data *active_tab = (t_tab_data *)data;
	gchar *text;
	gboolean add_to_history = TRUE;
	gboolean shift = FALSE;
	gboolean want_change = TRUE;

	this_mode = active_tab->mode;
	text = (gchar *)gtk_entry_get_text(GTK_ENTRY(widget));

	if (*text == '/')
		this_mode = 'b';
	else if (*text == '.')
		this_mode = 'e';
	else if (*text == '@')
		this_mode = 'm';

	switch (tolower(this_mode))
	{
		case 'b':
			Context;
			want_change = process_commandline((*text == '/') ? text + 1 : text, active_tab);
			Context;
			shift = (*text == '/');
			Context;
			break;
		case 'e':
			if (!active_tab->connected)
			{
				feed_error_to_terminal(active_tab, TRUE, _("Won't send an eggdrop command to a not connected tab"));
				add_to_history = FALSE;
				break;
			}
			else
			{
				char *buf = NULL;

				buf = g_convert(text, -1, BOT_CHARSET, "UTF-8", NULL, NULL, NULL);
				if (buf)
				{
					dprintf(active_tab->sock, "%s%s\n", (*buf != *EGG_CMD_CHAR) ? EGG_CMD_CHAR : "", buf);
					g_free(buf);
				}
			}
			shift = (*text == '.');
			break;
		case 'm':
		default:
			if (!active_tab->connected)
			{
				feed_error_to_terminal(active_tab, TRUE, _("Won't send message to a not connected tab"));
				add_to_history = FALSE;
				break;
			}
			else
			{
				char *buf = NULL;

				buf = g_convert(text, -1, BOT_CHARSET, "UTF-8", NULL, NULL, NULL);
				if (buf)
				{
					dprintf(active_tab->sock, "%s%s\n", (*buf == *EGG_CMD_CHAR) ? " " : "", buf);
					g_free(buf);
				}
			}
			shift = (*text == '@');
	}

	Context;

	if ((add_to_history) && (gtk_entry_get_visibility(GTK_ENTRY(widget))))
		history_add(active_tab, tolower(this_mode), (shift) ? text + 1 : text);
	active_tab->history_position = -1;
	Context;

	if (want_change)
		gtk_entry_set_text(GTK_ENTRY(((t_tab_data *)data)->editbox), "");

	Context;
	
	return TRUE;
}
/* }}} */

/* {{{ menu_exit() */
void
menu_exit(GtkWidget *widget, gpointer data)
{
	exit_cleanly(FALSE);
}
/* }}} */

/* {{{ menu_mode_changed() */
void
menu_mode_changed(GtkWidget *widget, gpointer data)
{
	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
	{
		char m = 0;

		if (widget == main_menu_mode_b)
			m = 'b';
		else
		{
			if (widget == main_menu_mode_e)
				m = 'e';
			else
			{
				if (widget == main_menu_mode_m)
					m = 'm';
			}
		}
		if (m)
			change_mode(m, TRUE);
	}
}
/* }}} */

/* {{{ menu_botlist_open() */
void
menu_botlist_open(GtkWidget *widget, gpointer data)
{
	display_botlist_window();
}
/* }}} */

/* {{{ menu_prefs_open() */
void
menu_prefs_open(GtkWidget *widget, gpointer data)
{
	display_setup_window();
}
/* }}} */

/* {{{ menu_new_tab() */
void
menu_new_tab(GtkWidget *widget, gpointer data)
{
	add_tab(NULL);
}
/* }}} */

/* {{{ menu_close_tab() */
void
menu_close_tab(GtkWidget *widget, gpointer data)
{
	close_tab(get_active_tab());
}
/* }}} */

/* {{{ menu_about() */
void
menu_about(GtkWidget *widget, gpointer data)
{
	display_about_box();
}
/* }}} */

/* {{{ entry_keypress() */
int
entry_keypress(GtkEntry *entry, GdkEventKey *event, gpointer data)
{
	if (data)
	{
		t_tab_data *tab = (t_tab_data *)data;
		if (entry == (GtkEntry *)tab->editbox)
		{
			if (event->keyval == GDK_Up)
			{
				gint count = get_history_len(tab);
				gint steps;
				t_history_data *temp = tab->history;

				if (count == 0)
				{
					tab->history_position = -1;
					return TRUE;
				}

				if ((count > 0) && (count > tab->history_position + 1))
					tab->history_position++;

				steps = count - tab->history_position - 1;
				while (steps)
				{
					if (!temp)
						return TRUE;
					temp = temp->next;
					steps--;
				}
				gtk_entry_set_text(GTK_ENTRY(tab->editbox), temp->line);
				gtk_entry_set_position(GTK_ENTRY(tab->editbox), -1);
				return TRUE;
			}
			else if (event->keyval == GDK_Down)
			{
				gint count = get_history_len(tab);

				if (count == 0)
				{
					tab->history_position = -1;
					return TRUE;
				}

				if (tab->history_position > -1)
					tab->history_position--;

				if (tab->history_position == -1)
					gtk_entry_set_text(GTK_ENTRY(tab->editbox), "");
				else
				{
					t_history_data *temp = tab->history;
					gint steps = count - tab->history_position - 1;

					while (steps)
					{
						if (!temp)
							return TRUE;
						temp = temp->next;
						steps--;
					}
					gtk_entry_set_text(GTK_ENTRY(tab->editbox), temp->line);
					gtk_entry_set_position(GTK_ENTRY(tab->editbox), -1);
				}
				return TRUE;
			}
		}
	}
	switch (event->keyval)
	{
		case GDK_0:
		case GDK_1:
		case GDK_2:
		case GDK_3:
		case GDK_4:
		case GDK_5:
		case GDK_6:
		case GDK_7:
		case GDK_8:
		case GDK_9:
			if (event->state & GDK_MOD1_MASK)
			{
				gint num = event->keyval - GDK_0;
				if (num == 0)
					num = 9;
				else
					num--;

				change_active_tab(num);
				return TRUE;
			}
			break;
		case GDK_Tab:
			return TRUE;
			break;
	}
	if (event->keyval == GDK_Tab)
		return TRUE;

	return FALSE;
}
/* }}} */

/* {{{ socket_event() */
void
socket_event(gpointer data, gint fd, GdkInputCondition cond)
{
	t_tab_data *tab = data;
	ssize_t len;
	char buf[MAX_READ_LINE_LEN];
	static char *current_line = NULL;
	static int current_line_pointer = 0;
	static char current_telnet_code[4];
	static int current_telnet_code_pointer = 0;

	memset(&buf, 0, MAX_READ_LINE_LEN);
	memset(&current_telnet_code, 0, 4);

	len = recv(tab->sock, &buf, MAX_READ_LINE_LEN - 1, 0);

	if (len == 0)
		disconnect_tab(tab);
	else if (len > 0)
	{
		char *obuf = NULL, *a;

		if (tab->data_arrived == FALSE)
		{
			if (config_data.change_to_message)
				change_mode('M', FALSE);
			tab->data_arrived = TRUE;
			feed_info_to_terminal(tab, TRUE, _("Connected"));
		}

		if (current_line == NULL)
		{
			current_line = g_malloc(MAX_READ_LINE_LEN + 1);
			memset(current_line, 0, MAX_READ_LINE_LEN + 1);
		}

		for (a = (char *)&buf; *a; a++)
		{
			if (*a == '\r')
				continue;
			if (*a == '\n')
			{
				process_incoming_data(tab, current_line);
				memset(current_line, 0, MAX_READ_LINE_LEN + 1);
				current_line_pointer = 0;
				continue;
			}
			if ((*a == *TELNET_IAC) || (*a == *TELNET_WILL) || (*a == *TELNET_WONT) || (*a == *TELNET_ECHO))
			{
				current_telnet_code[current_telnet_code_pointer++] = *a;
				if (strlen((char *)&current_telnet_code) == 3)
				{
					if (strcmp((char *)&current_telnet_code, TELNET_WILL_ECHO) == 0)
						gtk_entry_set_visibility(GTK_ENTRY(tab->editbox), FALSE);
					else if (strcmp((char *)&current_telnet_code, TELNET_WONT_ECHO) == 0)
						gtk_entry_set_visibility(GTK_ENTRY(tab->editbox), TRUE);
					memset(&current_telnet_code, 0, 4);
					current_telnet_code_pointer = 0;
				}
				continue;
			}
			current_line[current_line_pointer++] = *a;
		}

		obuf = g_convert(buf, -1, "UTF-8", BOT_CHARSET, NULL, NULL, NULL);
		g_free(obuf);
	}
#ifdef DEBUG
	else if (config_data.debug)
	{
		char *buf = NULL;
		char *orig = strerror(errno);

		buf = g_locale_to_utf8(orig, -1, NULL, NULL, NULL);
		feed_error_to_terminal(tab, TRUE, "%s", buf);
		g_free(buf);

		disconnect_tab(tab);
	}
#endif /* DEBUG */
}
/* }}} */

/* {{{ menu_showhide_sidebar() */
void
menu_showhide_sidebar(GtkWidget *menuitem, gpointer data)
{
	gint current_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(main_book));

	if (current_page >= 0)
	{
		gboolean new_state,
			 current_state;
		GtkWidget *box = gtk_paned_get_child2(GTK_PANED(gtk_notebook_get_nth_page(GTK_NOTEBOOK(main_book), current_page)));

		new_state = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));
		current_state = GTK_WIDGET_VISIBLE(box);

		feed_info_to_terminal(get_active_tab(), TRUE, "Current state: %s; new state: %s\n", (current_state) ? "active" : "not active", (new_state) ? "active" : "not active");

		if (current_state != new_state)
		{
			if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem)) == TRUE)
				gtk_widget_show_all(box);
			else
				gtk_widget_hide_all(box);
		}

	}
}
/* }}} */

