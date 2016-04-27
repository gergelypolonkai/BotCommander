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
 *            tabs.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Tab handling functions
 }}} */

/* TODO: Put this into a Makefile */
#define _GNU_SOURCE

/* {{{ Includes */
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <libintl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>

#include "typedefs.h"
#include "variables.h"
#include "functions.h"
#include "defines.h"
/* }}} */

/* {{{ Variables */
GList *tab_list;
/* }}} */

/* {{{ find_tab_by_pagenum() */
gint
find_tab_by_pagenum(gconstpointer a, gconstpointer b)
{
	if (((t_tab_data *)a)->num == (gint)*((gint *)b))
		return 0;

	return 1;
}
/* }}} */

/* {{{ open_channel_properties() */
void
open_channel_properties(GtkWidget *menuitem, gpointer data)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	t_tab_data *tab = (t_tab_data *)data;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tab->channel_list));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(tab->channel_list));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		char *name;

		gtk_tree_model_get(model, &iter, 0, &name, -1);
		dprintf(tab->sock, ".botcomm_chanprop %s\r\n", name);
	}
}
/* }}} */

/* {{{ open_user_properties() */
void
open_user_properties(GtkWidget *menuitem, gpointer data)
{
}
/* }}} */

/* {{{ change_channel_status() */
void
change_channel_status(GtkWidget *menuitem, gpointer data)
{
}
/* }}} */

/* {{{ view_channel_popup() */
void
view_channel_popup(GtkWidget *treeview, GdkEventButton *event, gpointer tab)
{
	GtkWidget *menu = NULL;
	GtkWidget *menuitem;

	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(((t_tab_data *)tab)->channel_list));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(((t_tab_data *)tab)->channel_list));
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		menu = gtk_menu_new();

		// TODO: If the channel is active, the label should say Deactivate
		menuitem = gtk_menu_item_new_with_label(_("Activate"));
		g_signal_connect(menuitem, "activate", G_CALLBACK(change_channel_status), tab);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		menuitem = gtk_menu_item_new_with_label(_("Properties"));
		g_signal_connect(menuitem, "activate", G_CALLBACK(open_channel_properties), tab);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

		gtk_widget_show_all(menu);
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent *)event));
	}
}
/* }}} */

/* {{{ view_user_popup() */
void
view_user_popup(GtkWidget *treeview, GdkEventButton *event, gpointer tab)
{
	GtkWidget *menu;
	GtkWidget *menuitem;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label(_("Properties"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(open_user_properties), tab);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent *)event));
}
/* }}} */

/* {{{ view_terminal_popup() */
void
view_terminal_popup(GtkWidget *terminal, GdkEventButton *event, gpointer tab)
{
	GtkWidget *menu;
	GtkWidget *menuitem;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label(_("Properties"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(open_user_properties), tab);

	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

	gtk_widget_show_all(menu);
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent *)event));
}
/* }}} */

/* {{{ channel_popupmenu_button() */
gboolean
channel_popupmenu_button(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		GtkTreeSelection *selection;
		GtkTreePath *path;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL))
		{
			gtk_tree_selection_unselect_all(selection);
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_path_free(path);
		}
		else
			return FALSE;
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if (gtk_tree_selection_count_selected_rows(selection) > 0)
			view_channel_popup(treeview, event, userdata);

		return TRUE;
	}

	return FALSE;
}
/* }}} */

/* {{{ user_popupmenu_button() */
gboolean
user_popupmenu_button(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		GtkTreeSelection *selection;
		GtkTreePath *path;

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL))
		{
			gtk_tree_selection_unselect_all(selection);
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_path_free(path);
		}
		else
			return FALSE;
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
		if (gtk_tree_selection_count_selected_rows(selection) > 0)
			view_user_popup(treeview, event, userdata);

		return TRUE;
	}

	return FALSE;
}
/* }}} */

/* {{{ terminal_popupmenu_button() */
gboolean
terminal_popupmenu_button(GtkWidget *terminal, GdkEventButton *event, gpointer userdata)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		view_terminal_popup(terminal, event, userdata);

		return TRUE;
	}
	
	return FALSE;
}
/* }}} */

/* {{{ channel_popupmenu_popup() */
gboolean
channel_popupmenu_popup(GtkWidget *treeview, gpointer userdata)
{
	view_channel_popup(treeview, NULL, userdata);

	return TRUE;
}
/* }}} */

/* {{{ user_popupmenu_popup() */
gboolean
user_popupmenu_popup(GtkWidget *treeview, gpointer userdata)
{
	view_user_popup(treeview, NULL, userdata);

	return TRUE;
}
/* }}} */

/* {{{ terminal_popupmenu_popup() */
gboolean
terminal_popupmenu_popup(GtkWidget *terminal, gpointer userdata)
{
	view_terminal_popup(terminal, NULL, userdata);

	return TRUE;
}
/* }}} */

/* {{{ add_tab() */
int
add_tab(t_tab_data **newtab)
{
	t_tab_data *temp;
	GtkWidget *img;
	gint w, h;
	gboolean change_title = config_data.change_to_new_tab,
		 first_tab = FALSE;
	char *buf;
	GtkWidget *paned;
	GtkWidget *scroll;
	GtkWidget *box;
	t_tab_data *active;
	GtkWidget *hbox;

	GtkCellRenderer *renderer;

	if (newtab)
		*newtab = NULL;

	if (tab_list == NULL)
	{
		change_title = TRUE;
		first_tab = TRUE;
	}

	temp = (t_tab_data *)malloc(sizeof(t_tab_data));

	temp->mode = config_data.default_mode;
	temp->connected = FALSE;
	temp->history = NULL;
	temp->history_position = -1;
	temp->chanlist_process = FALSE;
	temp->userlist_process = FALSE;
	temp->chanprops_process = NULL;
	temp->actual_channel_list = NULL;
	temp->actual_user_list = NULL;
	temp->data_arrived = FALSE;

	paned = gtk_hpaned_new();

	hbox = gtk_hbox_new(FALSE, 0);
	temp->vt = vte_terminal_new();
	vte_terminal_set_encoding(VTE_TERMINAL(temp->vt), "UTF-8");
	vte_terminal_set_allow_bold(VTE_TERMINAL(temp->vt), TRUE);
	vte_terminal_set_audible_bell(VTE_TERMINAL(temp->vt), TRUE);
	vte_terminal_set_visible_bell(VTE_TERMINAL(temp->vt), FALSE);
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(temp->vt), config_data.scroll_on_output);
	vte_terminal_set_background_transparent(VTE_TERMINAL(temp->vt), config_data.transparent_background);
	vte_terminal_set_background_saturation(VTE_TERMINAL(temp->vt), (double)config_data.background_saturation);
	vte_terminal_set_font_from_string(VTE_TERMINAL(temp->vt), (const char *)config_data.vte_font);
	GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(temp->vt), GTK_CAN_FOCUS);
	g_signal_connect(temp->vt, "button-press-event", G_CALLBACK(terminal_popupmenu_button), temp);
	g_signal_connect(temp->vt, "popup-menu", G_CALLBACK(terminal_popupmenu_popup), temp);
	gtk_box_pack_start(GTK_BOX(hbox), temp->vt, TRUE, TRUE, 0);

	scroll = gtk_vscrollbar_new(vte_terminal_get_adjustment(VTE_TERMINAL(temp->vt)));
	gtk_box_pack_start(GTK_BOX(hbox), scroll, FALSE, TRUE, 0);

	box = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(box), hbox, TRUE, TRUE, 0);

	temp->editbox = gtk_entry_new();
	gtk_signal_connect(GTK_OBJECT(temp->editbox), "activate", GTK_SIGNAL_FUNC(entry_activate), temp);
	gtk_signal_connect(GTK_OBJECT(temp->editbox), "key_press_event", GTK_SIGNAL_FUNC(entry_keypress), temp);
	gtk_box_pack_start(GTK_BOX(box), temp->editbox, FALSE, TRUE, 0);

	gtk_paned_add1(GTK_PANED(paned), box);

	box = gtk_vbox_new(TRUE, 2);

	Context;
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scroll, 150, -1);
	gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

	temp->channel_store = gtk_list_store_new(1, G_TYPE_STRING);
	renderer = gtk_cell_renderer_text_new();
	temp->channel_list = gtk_tree_view_new();
	g_signal_connect(temp->channel_list, "button-press-event", G_CALLBACK(channel_popupmenu_button), temp);
	g_signal_connect(temp->channel_list, "popup-menu", G_CALLBACK(channel_popupmenu_popup), temp);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(temp->channel_list), -1, _("Channels"), renderer, "text", 0, NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(temp->channel_list), GTK_TREE_MODEL(temp->channel_store));
	gtk_container_add(GTK_CONTAINER(scroll), temp->channel_list);

	Context;
	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scroll, 150, -1);
	gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);
	
	renderer = gtk_cell_renderer_text_new();
	temp->user_store = gtk_list_store_new(1, G_TYPE_STRING);
	temp->user_list = gtk_tree_view_new();
	g_signal_connect(temp->user_list, "button-press-event", G_CALLBACK(user_popupmenu_button), temp);
	g_signal_connect(temp->user_list, "popup-menu", G_CALLBACK(user_popupmenu_popup), temp);
	gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(temp->user_list), -1, _("Handles"), renderer, "text", 0, NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(temp->user_list), GTK_TREE_MODEL(temp->user_store));
	gtk_container_add(GTK_CONTAINER(scroll), temp->user_list);
	Context;

	gtk_paned_add2(GTK_PANED(paned), box);

	img = gtk_image_new_from_stock(GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
	gtk_icon_size_lookup(GTK_ICON_SIZE_MENU, &w, &h);

	temp->labelbox = gtk_hbox_new(FALSE, 0);

	temp->label = gtk_label_new(_("Not connected"));
	temp->closebutton = gtk_button_new();
	gtk_signal_connect(GTK_OBJECT(temp->closebutton), "clicked", GTK_SIGNAL_FUNC(close_tab_cb), temp);
	gtk_container_add(GTK_CONTAINER(temp->closebutton), img);
	gtk_button_set_relief(GTK_BUTTON(temp->closebutton), GTK_RELIEF_NONE);
	gtk_widget_set_size_request(temp->closebutton, w, h);
	Context;

	gtk_box_pack_start(GTK_BOX(temp->labelbox), temp->label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(temp->labelbox), temp->closebutton, FALSE, TRUE, 0);
	gtk_widget_show_all(temp->labelbox);

	Context;

	gtk_widget_show_all(paned);
	Context;

	temp->num = gtk_notebook_append_page(GTK_NOTEBOOK(main_book), paned, temp->labelbox);

	gtk_widget_set_sensitive(main_menu_show_sidebar, TRUE);

#ifdef DEBUG
	feed_info_to_terminal(temp, TRUE, "Opened as %d\n", temp->num);
#endif

	if (change_title)
	{
		set_title(NULL, "BotCommander - %s", _("Not connected"));
		gtk_notebook_set_current_page(GTK_NOTEBOOK(main_book), temp->num);
		buf = g_malloc(strlen(_("Mode: %c")) + 1);
		sprintf(buf, _("Mode: %c"), toupper(temp->mode));
		gtk_label_set_text(GTK_LABEL(main_statuslabel_mode), buf);
		g_free(buf);
	}

	tab_list = g_list_append(tab_list, temp);

	Context;

	active = get_active_tab();
	if (active)
		gtk_widget_grab_focus((get_active_tab())->editbox);

	Context;

	temp->sock = 0;
	temp->socktag = 0;
	temp->bot = NULL;
	
	if (newtab)
		*newtab = temp;

	Context;

	return temp->num;
}
/* }}} */

/* {{{ get_active_tab() */
t_tab_data *
get_active_tab(void)
{
	GList *temp;
	gint current = gtk_notebook_get_current_page(GTK_NOTEBOOK(main_book));

	Context;

	temp = g_list_find_custom(tab_list, &current, find_tab_by_pagenum);

	if (temp)
		return (t_tab_data *)(temp->data);

	Context;

	return NULL;
}
/* }}} */

/* {{{ set_tab_numbers() */
void
set_tab_numbers(gpointer data, gpointer user_data)
{
	t_tab_data *tab = (t_tab_data *)data;
	gint num = *((gint *)user_data);

	if (tab->num > num)
		tab->num--;
}
/* }}} */

/* {{{ close_tab() */
gboolean
close_tab(t_tab_data *to_close)
{
	Context;

	if (!to_close)
		return TRUE;

	if (to_close->connected)
	{
		GtkWidget *dialog;
		gint result;

		dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("This tab is connected. Are you sure you want to close it?"));
		result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		switch (result)
		{
			case GTK_RESPONSE_YES:
				gdk_input_remove(to_close->socktag);
				close(to_close->sock);
				break;
			default:
				return TRUE;
		}
	}

	Context;

	g_list_foreach(tab_list, set_tab_numbers, &(to_close->num));

	Context;

	gtk_notebook_remove_page(GTK_NOTEBOOK(main_book), to_close->num);
	tab_list = g_list_remove(tab_list, to_close);

	Context;

	if (!tab_list)
	{
		char *buf;

		buf = g_malloc(strlen(_("Mode: %c")) + 1);
		sprintf(buf, _("Mode: %c"), 0);
		gtk_label_set_text(GTK_LABEL(main_statuslabel_mode), buf);
		g_free(buf);

		gtk_widget_set_sensitive(main_menu_show_sidebar, FALSE);
	}

	Context;

	return TRUE;
}
/* }}} */

/* {{{ assign_bot_to_tab_by_name() */
gboolean
assign_bot_to_tab_by_name(t_tab_data *tab, gchar *name)
{
	t_bot_data *temp = get_bot_by_botname(name);

	if (temp)
	{
		tab->bot = get_bot_by_botname(name);
		set_title(tab, name);
		return TRUE;
	}
	
	return FALSE;
}
/* }}} */

/* {{{ assign_bot_to_tab() */
void
assign_bot_to_tab(t_tab_data *tab, t_bot_data *bot)
{
	tab->bot = bot;
	set_title(tab, bot->botname);
}
/* }}} */

/* {{{ connect_tab() */
gint
connect_tab(t_tab_data *tab)
{
	gint t = BCE_NOBOT;

	if (tab->bot)
	{
		t = connect_to_host(tab->bot->host, tab->bot->port, &(tab->sock), &(tab->socktag), tab);
		switch (t)
		{
			case BCE_NOPROTO:
				feed_error_to_terminal(tab, TRUE, _("Can not get IP protocol"));
				break;
			case BCE_CANTRESOLV:
				feed_error_to_terminal(tab, TRUE, _("Can not resolv hostname %s"), tab->bot->host);
				break;
			case BCE_CANTCONNECT:
				feed_error_to_terminal(tab, TRUE, _("Can not connect to bot"));
				break;
			case BCE_SUCCESS:
				tab->connected = TRUE;
				break;
			default:
				feed_error_to_terminal(tab, TRUE, _("Unknown error!"));
				break;
		}
	}

	return t;
}
/* }}} */

/* {{{ channel_store_updater() */
void
channel_store_updater(gpointer item, gpointer store)
{
	GtkListStore *model = (GtkListStore *)store;
	GtkTreeIter iter;

	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter, 0, ((t_channel *)item)->name, -1);
}
/* }}} */

/* {{{ update_channel_list() */
void
update_channel_list(t_tab_data *tab)
{
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(tab->channel_list));
	g_object_ref(model);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tab->channel_list), NULL);

	gtk_list_store_clear(GTK_LIST_STORE(model));

	g_list_foreach(tab->actual_channel_list, channel_store_updater, model);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tab->channel_list), model);
	g_object_unref(model);
}
/* }}} */

/* {{{ user_store_updater() */
void
user_store_updater(gpointer item, gpointer store)
{
	GtkListStore *model = (GtkListStore *)store;
	GtkTreeIter iter;

	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter, 0, ((t_user *)item)->name, -1);
}
/* }}} */

/* {{{ update_user_list() */
void
update_user_list(t_tab_data *tab)
{
	GtkTreeModel *model;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(tab->user_list));
	g_object_ref(model);
	gtk_tree_view_set_model(GTK_TREE_VIEW(tab->user_list), NULL);

	gtk_list_store_clear(GTK_LIST_STORE(model));
	
	g_list_foreach(tab->actual_user_list, user_store_updater, model);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tab->user_list), model);
	g_object_unref(model);
}
/* }}} */

/* {{{ disconnect_tab() */
void
disconnect_tab(t_tab_data *tab)
{
	if (tab->connected == FALSE)
	{
		feed_error_to_terminal(tab, TRUE, _("Tab is not connected"));
		return;
	}
	shutdown(tab->sock, SHUT_RDWR);
	close(tab->sock);
	gdk_input_remove(tab->socktag);
	tab->connected = FALSE;
	tab->socktag = 0;
	tab->sock = 0;
	if (tab->data_arrived)
		feed_info_to_terminal(tab, TRUE, _("Connection closed"));
	set_title(tab, "Not connected");
	change_mode('B', FALSE);
	tab->data_arrived = FALSE;
	/* If the connection is closed while we enter the password (e.g
	 * you mistype the password), the entry would be masked
	 * forever, so I unmask it now */
	gtk_entry_set_visibility(GTK_ENTRY(tab->editbox), TRUE);
}
/* }}} */

/* {{{ change_active_tab() */
void
change_active_tab(gint num)
{
	gtk_notebook_set_current_page(GTK_NOTEBOOK(main_book), num);
}
/* }}} */
