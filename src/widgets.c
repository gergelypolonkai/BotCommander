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
/* {{{
 *            widgets.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Widget creation
 }}} */

// TODO: Insert this in one of the Makefiles
#define _GNU_SOURCE
/* {{{ Includes */
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdarg.h>
#include <string.h>
#include <libintl.h>
#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>

#include "functions.h"
#include "typedefs.h"
#include "defines.h"
#include "config.h"

/* }}} */

/* {{{ Variables */
GtkWidget *main_window;
GtkWidget *main_vbox;
GtkWidget *main_menu;
GtkWidget *main_toolbar;
GtkWidget *main_book;
GtkWidget *main_statusbox;
GtkWidget *main_statuslabel_message;
GtkWidget *main_statuslabel_mode;
GtkWidget *main_menu_mode_b;
GtkWidget *main_menu_mode_e;
GtkWidget *main_menu_mode_m;
GtkAccelGroup *accel_group;
GtkWidget *main_menu_show_sidebar;
/* }}} */

/* {{{ set_title() */
void
set_title(t_tab_data *what, char *format, ...)
{
	gchar *ptbuf;
	va_list args;

	Context;
	va_start(args, format);
	vasprintf(&ptbuf, format, args);
	va_end(args);
	Context;

	if (!what)
		gtk_window_set_title(GTK_WINDOW(main_window), (const gchar *)ptbuf);
	else
	{
		gtk_label_set_text(GTK_LABEL(what->label), ptbuf);
		if (get_active_tab() == what)
			set_title(NULL, "BotCommander - %s", ptbuf);
	}
}
/* }}} */

/* {{{ create_menu() */
GtkWidget *
create_menu(void)
{
	GtkWidget *menubar;
	GtkWidget *submenu;
	GtkWidget *menuitem;
	GtkWidget *image;
	GtkWidget *hbox;
	GtkWidget *child;
	GSList *group = NULL;

	menubar = gtk_menu_bar_new();

	/* Bot submenu */
	submenu = gtk_menu_new();

	/* Exit menu item */
	menuitem = gtk_menu_item_new_with_mnemonic(_("E_xit"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(menuitem));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(menuitem), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_exit), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Append the submenu to the menu bar */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Bot"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

	/* Mode submenu */
	submenu = gtk_menu_new();

	/* BotCommander Command Mode menu item */
	menuitem = gtk_radio_menu_item_new_with_mnemonic(group, _("_BotCommander command mode"));
	gtk_signal_connect(GTK_OBJECT(menuitem), "toggled", GTK_SIGNAL_FUNC(menu_mode_changed), NULL);
	group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem), TRUE);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	main_menu_mode_b = menuitem;
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_b, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Eggdrop Command Mode menu item */
	menuitem = gtk_radio_menu_item_new_with_mnemonic(group, _("_Eggdrop command mode"));
	gtk_signal_connect(GTK_OBJECT(menuitem), "toggled", GTK_SIGNAL_FUNC(menu_mode_changed), NULL);
	group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	main_menu_mode_e = menuitem;
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_e, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Message Mode menu item */
	menuitem = gtk_radio_menu_item_new_with_mnemonic(group, _("_Message mode"));
	gtk_signal_connect(GTK_OBJECT(menuitem), "toggled", GTK_SIGNAL_FUNC(menu_mode_changed), NULL);
	group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	main_menu_mode_m = menuitem;
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_m, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Append the submenu to the menu bar */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Mode"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

	/* Setup submenu */
	submenu = gtk_menu_new();

	/* Bot List menu item */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Bot list"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_PROPERTIES, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(menuitem));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(menuitem), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_botlist_open), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_l, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	
	/* Preferences menu item */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Preferences"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(menuitem));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(menuitem), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_prefs_open), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Append the submenu to the menu bar */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Setup"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

	/* Tab submenu */
	submenu = gtk_menu_new();

	/* New Tab menu item */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_New tab"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(menuitem));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(menuitem), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_new_tab), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_t, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Close Tab menu item */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Close tab"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(menuitem));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(menuitem), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_close_tab), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

	/* Show/hide the channel and handle list */
	main_menu_show_sidebar = gtk_check_menu_item_new_with_mnemonic(_("Show sidebar"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(main_menu_show_sidebar));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(main_menu_show_sidebar), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(main_menu_show_sidebar), hbox);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(main_menu_show_sidebar), TRUE);
	gtk_signal_connect(GTK_OBJECT(main_menu_show_sidebar), "toggled", GTK_SIGNAL_FUNC(menu_showhide_sidebar), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), main_menu_show_sidebar);
	gtk_widget_add_accelerator(main_menu_show_sidebar, "activate", accel_group, GDK_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_set_sensitive(main_menu_show_sidebar, FALSE);

	/* Append the submenu to the menu bar */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Tab"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

	/* Help submenu */
	submenu = gtk_menu_new();

	/* About menu item */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_About"));
	hbox = gtk_hbox_new(FALSE, 0);
	image = gtk_image_new_from_stock(GTK_STOCK_ABOUT, GTK_ICON_SIZE_MENU);
	child = gtk_bin_get_child(GTK_BIN(menuitem));
	g_object_ref(child);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_container_remove(GTK_CONTAINER(menuitem), child);
	gtk_box_pack_start(GTK_BOX(hbox), child, TRUE, TRUE, 2);
	g_object_unref(child);
	gtk_container_add(GTK_CONTAINER(menuitem), hbox);
	gtk_signal_connect(GTK_OBJECT(menuitem), "activate", GTK_SIGNAL_FUNC(menu_about), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);

	/* Append the submenu to the menu bar */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
	gtk_menu_item_set_right_justified(GTK_MENU_ITEM(menuitem), TRUE);

	return menubar;
}
/* }}} */

/* {{{ create_main_window() */
int
create_main_window(void)
{
	GtkWidget *frame;
	gchar *buf;

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(destroy_event), NULL);
	g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(delete_event), NULL);
	set_title(NULL, "BotCommander");
	gtk_window_set_default_icon_from_file(PACKAGE_PIXMAPS_DIR "/bc48_trans.png", NULL);

	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);

	main_vbox = gtk_vbox_new(FALSE, 0);

	main_menu = create_menu();
	gtk_signal_connect(GTK_OBJECT(main_window), "key_press_event", GTK_SIGNAL_FUNC(entry_keypress), NULL);
	gtk_box_pack_start(GTK_BOX(main_vbox), main_menu, FALSE, TRUE, 0);

	main_toolbar = gtk_toolbar_new();
	gtk_toolbar_insert_stock(GTK_TOOLBAR(main_toolbar), GTK_STOCK_QUIT, _("Exit BotCommander"), NULL, G_CALLBACK(toolbar_quit), NULL, -1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(main_toolbar), GTK_STOCK_ADD, _("New tab"), NULL, G_CALLBACK(toolbar_add_tab), NULL, -1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(main_toolbar), GTK_STOCK_CLOSE, _("Close active tab"), NULL, G_CALLBACK(toolbar_close_active), NULL, -1);
	gtk_toolbar_insert_stock(GTK_TOOLBAR(main_toolbar), GTK_STOCK_PREFERENCES, _("Preferences"), NULL, G_CALLBACK(toolbar_open_prefs), NULL, -1);
	gtk_toolbar_set_style(GTK_TOOLBAR(main_toolbar), GTK_TOOLBAR_ICONS);
	gtk_box_pack_start(GTK_BOX(main_vbox), main_toolbar, FALSE, TRUE, 0);

	main_book = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(main_book), GTK_POS_TOP);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(main_book), TRUE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(main_book), TRUE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(main_book), TRUE);
	gtk_signal_connect(GTK_OBJECT(main_book), "switch-page", GTK_SIGNAL_FUNC(book_change_tab), NULL);
	gtk_box_pack_start(GTK_BOX(main_vbox), main_book, TRUE, TRUE, 0);

	main_statusbox = gtk_hbox_new(FALSE, 0);

	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
	main_statuslabel_message = gtk_label_new("BotCommander");
	gtk_label_set_justify(GTK_LABEL(main_statuslabel_message), GTK_JUSTIFY_RIGHT);
	gtk_container_add(GTK_CONTAINER(frame), main_statuslabel_message);
	gtk_box_pack_start(GTK_BOX(main_statusbox), frame, TRUE, TRUE, 0);

	buf = g_malloc(strlen(_("Mode: %c")) + 1);
	sprintf(buf, _("Mode: %c"), 0);
	frame = gtk_frame_new(NULL);
	main_statuslabel_mode = gtk_label_new(buf);
	g_free(buf);
	gtk_label_set_justify(GTK_LABEL(main_statuslabel_mode), GTK_JUSTIFY_LEFT);
	gtk_container_add(GTK_CONTAINER(frame), main_statuslabel_mode);
	gtk_box_pack_end(GTK_BOX(main_statusbox), frame, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(main_vbox), main_statusbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

	gtk_widget_show_all(main_window);

	return TRUE;
}
/* }}} */

/* {{{ feed_line_to_terminal() */
/* Feeds some text to the terminal widget. */
static void
feed_line_to_terminal(t_tab_data *tab, gboolean need_crlf, gchar *line, va_list list)
{
	gchar *string;
	gint len;
	
	len = vasprintf(&string, line, list);

	vte_terminal_feed((VteTerminal *)tab->vt, string, strlen(string));
	if (need_crlf)
		vte_terminal_feed((VteTerminal *)tab->vt, "\r\n", 2);
}
/* }}} */

/* {{{ feed_crlf_to_terminal() */
/* Feeds a CD LF pair to the terminal, if needed. */
static void
feed_crlf_to_terminal(t_tab_data *tab, gboolean need_it)
{
	if (need_it)
		vte_terminal_feed((VteTerminal *)tab->vt, "\r\n", 2);
}
/* }}} */

/* {{{ feed_message_to_terminal() */
/* Feeds some informational (un-coloured) text to the terminal, always appending a line-feed. */
void
feed_message_to_terminal(t_tab_data *tab, gboolean need_crlf, gchar *line, ...)
{
	va_list pl;
	
	va_start(pl, line);
	feed_line_to_terminal(tab, FALSE, line, pl);
	va_end(pl);
	feed_crlf_to_terminal(tab, need_crlf);
}
/* }}} */

/* {{{ feed_error_to_terminal() */
/* Feeds some error (red) text to the terminal, always appending a line-feed */
void
feed_error_to_terminal(t_tab_data *tab, gboolean need_crlf, gchar *line, ...)
{
	va_list pl;

	va_start(pl, line);
	feed_message_to_terminal(tab, FALSE, "\033[1;31m");
	feed_line_to_terminal(tab, FALSE, line, pl);
	va_end(pl);
	feed_message_to_terminal(tab, need_crlf, "\033[0m");
}
/* }}} */

/* {{{ feed_info_to_terminal() */
/* Feeds some informational (green) text to the terminal, always appending a line-feed. */
void
feed_info_to_terminal(t_tab_data *tab, gboolean need_crlf, gchar *line, ...)
{
	va_list pl;

	feed_message_to_terminal(tab, FALSE, "\033[1;32m");

	va_start(pl, line);
	feed_line_to_terminal(tab, FALSE, line, pl);
	va_end(pl);

	feed_message_to_terminal(tab, need_crlf, "\033[0m");
}
/* }}} */

