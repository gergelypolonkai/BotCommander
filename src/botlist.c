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
 *            botlist.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * The Bot list dialog
 }}} */

/* {{{ Includes */
#define _GNU_SOURCE

#include <gtk/gtk.h>
#include <libintl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "variables.h"
#include "functions.h"
/* }}} */

/* {{{ Variables */
static GtkDialog *editor_window = NULL;
static GtkDialog *botlist_win = NULL;
GtkWidget *botlist_edit_button,
		  *botlist_connect_button,
		  *botlist_remove_button,
		  *botlist_editor_botname,
		  *botlist_editor_host,
		  *botlist_editor_port,
		  *botlist_editor_handle;
static GtkWidget *bot_list = NULL;
/* }}} */

/* {{{ add_this_data() */
static void
add_this_data(gpointer data, gpointer user_data)
{
	GtkTreeIter iter;
	GtkListStore *store = (GtkListStore *)user_data;
	t_bot_data *bot = (t_bot_data *)data;

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, bot->botname, -1);
}
/* }}} */

/* {{{ refresh_botlist() */
static void
refresh_botlist(void)
{
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(bot_list));
	g_object_ref(model);
	gtk_tree_view_set_model(GTK_TREE_VIEW(bot_list), NULL);
	gtk_list_store_clear(GTK_LIST_STORE(model));

	g_list_foreach(internal_bot_list, add_this_data, (gpointer)model);

	gtk_tree_view_set_model(GTK_TREE_VIEW(bot_list), model);
	g_object_unref(model);
}
/* }}} */

/* {{{ edit_bot_dialog() */
static void
edit_bot_dialog(GtkWidget *widget, gpointer data)
{
	/* Here we can be sure that we have a selected item */
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gint response;
	gboolean can_close = FALSE;

	if (!editor_window)
		create_bot_editor(NULL);
	gtk_window_set_title(GTK_WINDOW(editor_window), _("Edit bot"));

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(bot_list));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gchar *name;
		t_bot_data *bot;

		gtk_tree_model_get(model, &iter, 0, &name, -1);

		if ((bot = get_bot_by_botname(name)) != NULL)
		{
			char *portnum;

			asprintf(&portnum, "%d", bot->port);
			gtk_entry_set_text(GTK_ENTRY(botlist_editor_botname), bot->botname);
			gtk_entry_set_text(GTK_ENTRY(botlist_editor_host), bot->host);
			gtk_entry_set_text(GTK_ENTRY(botlist_editor_port), portnum);
			gtk_entry_set_text(GTK_ENTRY(botlist_editor_handle), bot->handle);

			while (!can_close)
			{
				can_close = TRUE;
				response = gtk_dialog_run(editor_window);

				if (response == GTK_RESPONSE_OK)
				{
					gint port_num;
					char *endptr;

					gchar *botname = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_botname)));
					gchar *host = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_host)));
					gchar *port = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_port)));
					gchar *handle = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_handle)));
					if ((strcmp(botname, "") == 0) || (strcmp(host, "") == 0) || (strcmp(port, "") == 0))
					{
						can_close = FALSE;
#ifdef DEBUG
						if (config_data.debug)
							fprintf(stderr, "All fields are mandatory except Handle\n");
#endif /* DEBUG */
						// TODO: Here we have to notify the user
						g_free(botname);
						g_free(host);
						g_free(port);
						g_free(handle);
						continue;
					}
					port_num = strtol(port, &endptr, 10);
					if (*endptr)
					{
						can_close = FALSE;
#ifdef DEBUG
						if (config_data.debug)
							fprintf(stderr, "Port must be a number! (%s)\n", endptr);
#endif /* DEBUG */
						// TODO: Here we have to notify the user
						g_free(botname);
						g_free(host);
						g_free(port);
						g_free(handle);
						continue;
					}
					(void)update_bot(bot, botname, host, port_num, handle);
					refresh_botlist();

					g_free(port);
					g_free(botname);
					g_free(host);
					g_free(handle);
				}
			}

			gtk_widget_hide(GTK_WIDGET(editor_window));
		}
	}
#ifdef DEBUG
	else
		if (config_data.debug)
			printf("No bot selected, but signal caught??\n");
#endif /* DEBUG */
}
/* }}} */

/* {{{ add_bot_dialog() */
/* This callback is called when the "Add" button is pressed in the Botlist dialog */
static void
add_bot_dialog(GtkWidget *widget, gpointer data)
{
	gint response;
	gboolean can_close = FALSE;

	if (!editor_window)
		create_bot_editor(NULL);
	gtk_window_set_title(GTK_WINDOW(editor_window), _("Add bot"));

	gtk_entry_set_text(GTK_ENTRY(botlist_editor_botname), "");
	gtk_entry_set_text(GTK_ENTRY(botlist_editor_host), "");
	gtk_entry_set_text(GTK_ENTRY(botlist_editor_port), "");
	gtk_entry_set_text(GTK_ENTRY(botlist_editor_handle), "");

	while (!can_close)
	{
		can_close = TRUE;
		response = gtk_dialog_run(editor_window);

		if (response == GTK_RESPONSE_OK)
		{
			gint port_num;
			char *endptr;

			gchar *botname = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_botname)));
			gchar *host = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_host)));
			gchar *port = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_port)));
			gchar *handle = trim((char *)gtk_entry_get_text(GTK_ENTRY(botlist_editor_handle)));
			if ((strcmp(botname, "") == 0) || (strcmp(host, "") == 0) || (strcmp(port, "") == 0))
			{
				can_close = FALSE;
#ifdef DEBUG
				if (config_data.debug)
					fprintf(stderr, "All fields are mandatory except Handle\n");
#endif /* DEBUG */
				// TODO: Here we have to notify the user
				g_free(botname);
				g_free(host);
				g_free(port);
				g_free(handle);
				continue;
			}
			port_num = strtol(port, &endptr, 10);
			if (*endptr)
			{
				can_close = FALSE;
#ifdef DEBUG
				if (config_data.debug)
					fprintf(stderr, "Port must be a number! (%s)\n", endptr);
#endif /* DEBUG */
				// TODO: Here we have to notify the user
				g_free(botname);
				g_free(host);
				g_free(port);
				g_free(handle);
				continue;
			}
			if (add_bot(botname, host, port_num, handle) != BCE_SUCCESS)
			{
#ifdef DEBUG
				if (config_data.debug)
					fprintf(stderr, "Port must be a number! (%s)\n", endptr);
#endif /* DEBUG */
				// TODO: Here we have to notify the user
			}
			else
				refresh_botlist();
			g_free(port);
			g_free(botname);
			g_free(host);
			g_free(handle);
		}
	}

	gtk_widget_hide(GTK_WIDGET(editor_window));
}
/* }}} */

/* {{{ cchange() */
/* This callback is called, when I select a list item */
void
static cchange(GtkWidget *widget, gpointer data)
{
	/* Here we can be sure that we have a selected item */
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_widget_set_sensitive(botlist_connect_button, TRUE);
		gtk_widget_set_sensitive(botlist_edit_button, TRUE);
		gtk_widget_set_sensitive(botlist_remove_button, TRUE);
	}
#ifdef DEBUG
	else
		if (config_data.debug)
			printf("No bot selected, but signal caught??\n");
#endif /* DEBUG */
}
/* }}} */

/* {{{ cactivate() */
/* This callback is called when I double-click a list item, or when I navigate to an item and press Enter */
static void
cactivate(GtkWidget *widget, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
		gtk_button_clicked(GTK_BUTTON(botlist_connect_button));
#ifdef DEBUG
	else
		if (config_data.debug)
			fprintf(stderr, "No bot selected, but signal caught??\n");
#endif /* DEBUG */
}
/* }}} */

/* {{{ toggle_cursor_row() */
/* This callback is called when I select or deselect a list item (but not when I change selection!) */
static gboolean
toggle_cursor_row(GtkTreeView *widget, gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_widget_set_sensitive(botlist_connect_button, FALSE);
		gtk_widget_set_sensitive(botlist_edit_button, FALSE);
		gtk_widget_set_sensitive(botlist_remove_button, FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(botlist_connect_button, TRUE);
		gtk_widget_set_sensitive(botlist_edit_button, TRUE);
		gtk_widget_set_sensitive(botlist_remove_button, TRUE);
	}

	return TRUE;
}
/* }}} */

/* {{{ create_and_fill_model() */
/* This function creates the tree model and fills it with data */
static GtkTreeModel *
create_and_fill_model(void)
{
	GtkListStore *store;

	store = gtk_list_store_new(1, G_TYPE_STRING);

	g_list_foreach(internal_bot_list, add_this_data, (gpointer)store);

	return GTK_TREE_MODEL(store);
}
/* }}} */

/* {{{ display_botlist_window() */
/* This one creates the bot list window if it doesn't exist, and displays it */
void
display_botlist_window(void)
{
	gint response;
	gboolean created_model = FALSE;
	GtkTreeModel *model;

	if (!GTK_IS_WINDOW(botlist_win))
	{
		GtkCellRenderer *renderer;
		GtkWidget *hbox,
				  *vbox,
				  *button,
				  *scrollwin,
				  *image,
				  *bbox,
				  *blabel;

		created_model = TRUE;

		botlist_win = (GtkDialog *)gtk_dialog_new();
		gtk_window_set_title(GTK_WINDOW(botlist_win), _("Bot list"));
		gtk_window_set_transient_for(GTK_WINDOW(botlist_win), GTK_WINDOW(main_window));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(botlist_win), TRUE);
		gtk_window_set_modal(GTK_WINDOW(botlist_win), TRUE);

		image = gtk_image_new_from_stock(GTK_STOCK_CONNECT, GTK_ICON_SIZE_BUTTON);
		blabel = gtk_label_new_with_mnemonic(_("_Connect"));
		bbox = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(bbox), image, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(bbox), blabel, TRUE, TRUE, 0);

		botlist_connect_button = gtk_button_new();
		gtk_widget_set_sensitive(botlist_connect_button, FALSE);
		gtk_container_add(GTK_CONTAINER(botlist_connect_button), bbox);
		gtk_widget_show_all(botlist_connect_button);
		gtk_dialog_add_action_widget(botlist_win, botlist_connect_button, GTK_RESPONSE_APPLY);

		button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
		gtk_widget_show(button);
		gtk_dialog_add_action_widget(botlist_win, button, GTK_RESPONSE_OK);

		button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
		gtk_widget_show(button);
		gtk_dialog_add_action_widget(botlist_win, button, GTK_RESPONSE_CANCEL);

		scrollwin = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_set_size_request(scrollwin, 150, 200);

		renderer = gtk_cell_renderer_text_new();
		bot_list = gtk_tree_view_new();
		gtk_signal_connect(GTK_OBJECT(bot_list), "cursor-changed", GTK_SIGNAL_FUNC(cchange), NULL);
		gtk_signal_connect(GTK_OBJECT(bot_list), "row-activated", GTK_SIGNAL_FUNC(cactivate), NULL);
		gtk_signal_connect(GTK_OBJECT(bot_list), "toggle-cursor-row", GTK_SIGNAL_FUNC(toggle_cursor_row), NULL);
		gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(bot_list), -1, _("Bot name"), renderer, "text", 0, NULL);
		model = create_and_fill_model();
		gtk_tree_view_set_model(GTK_TREE_VIEW(bot_list), model);
		g_object_unref(model);

		gtk_container_add(GTK_CONTAINER(scrollwin), bot_list);

		hbox = gtk_hbox_new(FALSE, 2);

		gtk_box_pack_start(GTK_BOX(hbox), scrollwin, TRUE, TRUE, 0);

		vbox = gtk_vbox_new(FALSE, 2);

		button = gtk_button_new_from_stock(GTK_STOCK_ADD);
		gtk_signal_connect(GTK_OBJECT(button), "clicked", GTK_SIGNAL_FUNC(add_bot_dialog), NULL);
		gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

		botlist_edit_button = gtk_button_new_from_stock(GTK_STOCK_EDIT);
		gtk_signal_connect(GTK_OBJECT(botlist_edit_button), "clicked", GTK_SIGNAL_FUNC(edit_bot_dialog), NULL);
		gtk_widget_set_sensitive(botlist_edit_button, FALSE);
		gtk_box_pack_start(GTK_BOX(vbox), botlist_edit_button, FALSE, FALSE, 0);

		botlist_remove_button = gtk_button_new_from_stock(GTK_STOCK_REMOVE);
		gtk_widget_set_sensitive(botlist_remove_button, FALSE);
		gtk_box_pack_start(GTK_BOX(vbox), botlist_remove_button, FALSE, FALSE, 0);

		gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

		gtk_widget_show_all(hbox);

		gtk_container_add(GTK_CONTAINER(botlist_win->vbox), hbox);
	}

	if (!created_model)
		refresh_botlist();

	response = gtk_dialog_run(botlist_win);
	
	switch (response)
	{
		case GTK_RESPONSE_APPLY:
			{
				GtkTreeSelection *selection;
				GtkTreeModel *model;
				GtkTreeIter iter;
				t_tab_data *tab = get_active_tab();

				if (tab)
					gtk_widget_grab_focus(tab->editbox);

				selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(bot_list));
				if (gtk_tree_selection_get_selected(selection, &model, &iter))
				{
					gchar *name;

					if (!tab)
						add_tab(&tab);

					if (tab->connected)
						add_tab(&tab);

					gtk_tree_model_get(model, &iter, 0, &name, -1);

					assign_bot_to_tab_by_name(tab, name);
					g_free(name);
					connect_tab(tab);
				}
#ifdef DEBUG
				else
					if (config_data.debug)
						printf("No bot selected, but signal caught??\n");
#endif /* DEBUG */
			}
			break;
		case GTK_RESPONSE_OK:
			save_bot_list_to_config();
			break;
	}

	gtk_widget_hide(GTK_WIDGET(botlist_win));
}
/* }}} */

/* {{{ create_bot_editor */
void
create_bot_editor(t_bot_data *bot)
{
	if (!editor_window)
	{
		GtkWidget *button,
				  *label;
		GtkTable *table;

		editor_window = (GtkDialog *)gtk_dialog_new();
		gtk_window_set_transient_for(GTK_WINDOW(editor_window), GTK_WINDOW(botlist_win));
		gtk_window_set_destroy_with_parent(GTK_WINDOW(editor_window), TRUE);
		gtk_window_set_modal(GTK_WINDOW(editor_window), TRUE);

		table = (GtkTable *)gtk_table_new(4, 2, FALSE);

		gtk_table_attach(table, (label = gtk_label_new(_("Bot name"))), 0, 1, 0, 1, GTK_FILL, 0, 2, 2);
		gtk_table_attach(table, (botlist_editor_botname = gtk_entry_new()), 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, 0, 2, 2);
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

		gtk_table_attach(table, (label = gtk_label_new(_("Host"))), 0, 1, 1, 2, GTK_FILL, 0, 2, 2);
		gtk_table_attach(table, (botlist_editor_host = gtk_entry_new()), 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, 0, 2, 2);
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

		gtk_table_attach(table, (label = gtk_label_new(_("Port"))), 0, 1, 2, 3, GTK_FILL, 0, 2, 2);
		gtk_table_attach(table, (botlist_editor_port = gtk_entry_new()), 1, 2, 2, 3, GTK_FILL | GTK_EXPAND, 0, 2, 2);
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

		gtk_table_attach(table, (label = gtk_label_new(_("Handle"))), 0, 1, 3, 4, GTK_FILL, 0, 2, 2);
		gtk_table_attach(table, (botlist_editor_handle = gtk_entry_new()), 1, 2, 3, 4, GTK_FILL | GTK_EXPAND, 0, 2, 2);
		gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);

		gtk_container_add(GTK_CONTAINER(editor_window->vbox), GTK_WIDGET(table));

		button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
		gtk_dialog_add_action_widget(editor_window, button, GTK_RESPONSE_CANCEL);

		button = gtk_button_new_from_stock(GTK_STOCK_SAVE);
		gtk_dialog_add_action_widget(editor_window, button, GTK_RESPONSE_OK);
	}
	gtk_widget_show_all(GTK_WIDGET(editor_window));
}
/* }}} */

