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
 *            setup.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Preferences dialog
 }}} */

/* {{{ Defines */
#define CB_ALL 0
#define CB_SCROLL 1
#define CB_TRANSBG 2
#define CB_BGSAT 3
#define CB_FONT 4
#define CB_RESET 5
/* }}} */

/* {{{ Includes */
#include <libintl.h>
#include <gtk/gtk.h>
#include <string.h>
#include <vte/vte.h>

#include "variables.h"
#include "functions.h"
/* }}} */

/* {{{ Typedefs */
typedef struct _t_which {
	gint which;
	gboolean bool_value;
	gfloat float_value;
	gchar *string_value;
} t_which;
/* }}} */

/* {{{ Variables */
#ifdef DEBUG
GtkWidget *debug_cb;
#endif /* DEBUG */
GtkWidget *cmode_cb;
GtkWidget *prefsave_cb;
GtkWidget *scroll_cb;
GtkWidget *transbg_cb;
GtkWidget *font_button;
GtkWidget *historylen_spin;
GtkWidget *satur_scale;
GtkWidget *changetab_cb;
t_config_data backup_data;
/* }}} */

/* {{{ debug_cb_toggled() */
#ifdef DEBUG
static void
debug_cb_toggled(GtkWidget *widget, gpointer data)
{
	backup_data.debug = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}
#endif /* DEBUG */
/* }}} */

/* {{{ cmode_cb_toggled() */
static void
cmode_cb_toggled(GtkWidget *widget, gpointer data)
{
	backup_data.change_to_message = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}
/* }}} */

/* {{{ prefsave_cb_toggled() */
static void
prefsave_cb_toggled(GtkWidget *widget, gpointer data)
{
	backup_data.save_prefs = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}
/* }}} */

/* {{{ changetab_cb_toggled() */
static void
changetab_cb_toggled(GtkWidget *widget, gpointer data)
{
	backup_data.change_to_new_tab = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}
/* }}} */

/* {{{ set_tab_vt_prefs() */
static void
set_tab_vt_prefs(gpointer data, gpointer user_data)
{
	t_tab_data *tab = (t_tab_data *)data;
	t_which *which = (t_which *)user_data;

	if ((which->which == CB_SCROLL) || (which->which == CB_ALL))
		vte_terminal_set_scroll_on_output(VTE_TERMINAL(tab->vt), which->bool_value);
	if ((which->which == CB_TRANSBG) || (which->which == CB_ALL))
		vte_terminal_set_background_transparent(VTE_TERMINAL(tab->vt), which->bool_value);
	if ((which->which == CB_BGSAT) || (which->which == CB_ALL))
		vte_terminal_set_background_saturation(VTE_TERMINAL(tab->vt), which->float_value / 100.0);
	if ((which->which == CB_FONT) || (which->which == CB_ALL))
		vte_terminal_set_font_from_string(VTE_TERMINAL(tab->vt), which->string_value);
	if (which->which == CB_RESET)
	{
		vte_terminal_set_scroll_on_output(VTE_TERMINAL(tab->vt), config_data.scroll_on_output);
		vte_terminal_set_background_transparent(VTE_TERMINAL(tab->vt), config_data.transparent_background);
		vte_terminal_set_background_saturation(VTE_TERMINAL(tab->vt), config_data.background_saturation);
		vte_terminal_set_font_from_string(VTE_TERMINAL(tab->vt), config_data.vte_font);
	}
}
/* }}} */

/* {{{ scroll_cb_toggled() */
static void
scroll_cb_toggled(GtkWidget *widget, gpointer data)
{
	t_which which;

	backup_data.scroll_on_output = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

	which.which = CB_SCROLL;
	which.bool_value = backup_data.scroll_on_output;

	g_list_foreach(tab_list, set_tab_vt_prefs, &which);
}
/* }}} */

/* {{{ transbg_cb_toggled() */
static void
transbg_cb_toggled(GtkWidget *widget, gpointer data)
{
	t_which which;

	backup_data.transparent_background = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

	which.which = CB_TRANSBG;
	which.bool_value = backup_data.transparent_background;

	g_list_foreach(tab_list, set_tab_vt_prefs, &which);
}
/* }}} */

/* {{{ satur_bar_changed() */
static void
satur_bar_changed(GtkWidget *widget, gpointer data)
{
	t_which which;

	backup_data.background_saturation = gtk_range_get_value(GTK_RANGE(widget));

	which.which = CB_BGSAT;
	which.float_value = backup_data.background_saturation;

	g_list_foreach(tab_list, set_tab_vt_prefs, &which);
}
/* }}} */

/* {{{ historylen_spin_changed() */
static void
historylen_spin_changed(GtkWidget *widget, gpointer data)
{
	backup_data.history_len = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}
/* }}} */

/* {{{ font_changed() */
static void
font_changed(GtkWidget *widget, gpointer data)
{
	t_which which;

	if (backup_data.vte_font)
		g_free(backup_data.vte_font);
	backup_data.vte_font = g_strdup(gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget)));

	which.which = CB_FONT;
	which.string_value = backup_data.vte_font;
	
	g_list_foreach(tab_list, set_tab_vt_prefs, &which);
}
/* }}} */

/* {{{ setup_response() */
static void
setup_response(GtkDialog *dialog, int id, gpointer data)
{
	switch (id)
	{
		case GTK_RESPONSE_OK:
			g_free(config_data.vte_font);
			memcpy(&config_data, &backup_data, sizeof(t_config_data));
			config_data.vte_font = g_strdup(backup_data.vte_font);
			/* TODO: Change the command line history buffer's size!! */
			gtk_widget_hide(GTK_WIDGET(dialog));
			break;
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_DELETE_EVENT:
			{
				t_which which;

				which.which = CB_RESET;
				g_list_foreach(tab_list, set_tab_vt_prefs, &which);
				gtk_widget_hide(GTK_WIDGET(dialog));
			}
			break;
		case GTK_RESPONSE_YES:
			/* Itt minden pref beallitast el kell menteni az aktualis backendbe */
			break;
		default:
#ifdef DEBUG
			if (config_data.debug)
				fprintf(stderr, "Unknown response!\n");
#endif /* DEBUG */
			break;
	}
}
/* }}} */

/* {{{ display_setup_window() */
void
display_setup_window(void)
{
	static GtkWidget *setup_window = NULL;

	memcpy(&backup_data, &config_data, sizeof(t_config_data));
	backup_data.vte_font = g_strdup(config_data.vte_font);

	if (!GTK_IS_WINDOW(setup_window))
	{
		GtkWidget *frame;
		GtkWidget *content;
		GtkWidget *satur_label;
		GtkWidget *font_label;
		GtkWidget *historylen_label;
		gint row;
#ifdef DEBUG
# define CONTENT_ROW_NUMBER 5
#else
# define CONTENT_ROW_NUMBER 4
#endif

		setup_window = gtk_dialog_new_with_buttons((const gchar *)_("Setup"), GTK_WINDOW(main_window), GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR, GTK_STOCK_OK, GTK_RESPONSE_OK, GTK_STOCK_SAVE, GTK_RESPONSE_YES, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
		gtk_dialog_set_response_sensitive(GTK_DIALOG(setup_window), GTK_RESPONSE_OK, TRUE);

		gtk_signal_connect(GTK_OBJECT(setup_window), "response", GTK_SIGNAL_FUNC(setup_response), NULL);

		frame = gtk_frame_new("BotCommander");
		gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
		
		content = gtk_table_new(CONTENT_ROW_NUMBER, 2, TRUE);

		row = 0;
#ifdef DEBUG
		debug_cb = gtk_check_button_new_with_label(_("Debug mode"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(debug_cb), config_data.debug);
		gtk_table_attach_defaults(GTK_TABLE(content), debug_cb, 0, 2, row, row + 1);
		gtk_signal_connect(GTK_OBJECT(debug_cb), "toggled", GTK_SIGNAL_FUNC(debug_cb_toggled), NULL);
		row++;
#endif

		cmode_cb = gtk_check_button_new_with_label(_("Change to message mode after connect"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cmode_cb), config_data.change_to_message);
		gtk_table_attach_defaults(GTK_TABLE(content), cmode_cb, 0, 2, row, row + 1);
		gtk_signal_connect(GTK_OBJECT(cmode_cb), "toggled", GTK_SIGNAL_FUNC(cmode_cb_toggled), NULL);
		row++;

		changetab_cb = gtk_check_button_new_with_label(_("Change to newly created tab"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(changetab_cb), config_data.change_to_new_tab);
		gtk_table_attach_defaults(GTK_TABLE(content), changetab_cb, 0, 2, row, row + 1);
		gtk_signal_connect(GTK_OBJECT(changetab_cb), "toggled", GTK_SIGNAL_FUNC(changetab_cb_toggled), NULL);
		row++;

		prefsave_cb = gtk_check_button_new_with_label(_("Save preferences on exit"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefsave_cb), config_data.save_prefs);
		gtk_table_attach_defaults(GTK_TABLE(content), prefsave_cb, 0, 2, row, row + 1);
		gtk_signal_connect(GTK_OBJECT(prefsave_cb), "toggled", GTK_SIGNAL_FUNC(prefsave_cb_toggled), NULL);
		row++;

		historylen_label = gtk_label_new(_("Command line history length"));
		gtk_table_attach(GTK_TABLE(content), historylen_label, 0, 1, row, row + 1, GTK_FILL, 0, 4, 0);

		historylen_spin = gtk_spin_button_new_with_range(0, 5000, 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(historylen_spin), config_data.history_len);
		gtk_table_attach_defaults(GTK_TABLE(content), historylen_spin, 1, 2, row, row + 1);
		gtk_signal_connect(GTK_OBJECT(historylen_spin), "value-changed", GTK_SIGNAL_FUNC(historylen_spin_changed), NULL);
		row++;

		gtk_container_add(GTK_CONTAINER(frame), content);

		gtk_widget_show_all(frame);

		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(setup_window)->vbox), frame, TRUE, TRUE, 0);

		frame = gtk_frame_new(_("Terminal"));
		gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

		content = gtk_table_new(5, 3, TRUE);

		scroll_cb = gtk_check_button_new_with_label(_("Scroll on output"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(scroll_cb), config_data.scroll_on_output);
		gtk_table_attach_defaults(GTK_TABLE(content), scroll_cb, 0, 3, 0, 1);
		gtk_signal_connect(GTK_OBJECT(scroll_cb), "toggled", GTK_SIGNAL_FUNC(scroll_cb_toggled), NULL);

		transbg_cb = gtk_check_button_new_with_label(_("Transparent background"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(transbg_cb), config_data.transparent_background);
		gtk_table_attach_defaults(GTK_TABLE(content), transbg_cb, 0, 3, 1, 2);
		gtk_signal_connect(GTK_OBJECT(transbg_cb), "toggled", GTK_SIGNAL_FUNC(transbg_cb_toggled), NULL);

		satur_label = gtk_label_new(_("Saturation"));
		gtk_table_attach(GTK_TABLE(content), satur_label, 0, 1, 2, 3, GTK_FILL, 0, 0, 0);

		satur_scale = gtk_hscale_new_with_range(0, 100, 1);
		gtk_scale_set_digits(GTK_SCALE(satur_scale), 0);
		gtk_scale_set_draw_value(GTK_SCALE(satur_scale), TRUE);
		gtk_scale_set_value_pos(GTK_SCALE(satur_scale), GTK_POS_RIGHT);
		gtk_range_set_value(GTK_RANGE(satur_scale), config_data.background_saturation * 100);
		gtk_signal_connect(GTK_OBJECT(satur_scale), "value-changed", GTK_SIGNAL_FUNC(satur_bar_changed), NULL);
		gtk_table_attach_defaults(GTK_TABLE(content), satur_scale, 0, 3, 3, 4);

		font_label = gtk_label_new(_("Font"));
		gtk_table_attach(GTK_TABLE(content), font_label, 0, 1, 4, 5, GTK_FILL, 0, 0, 0);

		font_button = gtk_font_button_new_with_font(config_data.vte_font);
		gtk_table_attach_defaults(GTK_TABLE(content), font_button, 1, 3, 4, 5);
		gtk_signal_connect(GTK_OBJECT(font_button), "font-set", GTK_SIGNAL_FUNC(font_changed), NULL);

		gtk_container_add(GTK_CONTAINER(frame), content);

		gtk_widget_show_all(frame);
		
		gtk_box_pack_start(GTK_BOX(GTK_DIALOG(setup_window)->vbox), frame, TRUE, TRUE, 0);
	}
	else
	{
#ifdef DEBUG
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(debug_cb), config_data.debug);
#endif /* DEBUG */
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cmode_cb), config_data.change_to_message);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefsave_cb), config_data.save_prefs);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(scroll_cb), config_data.scroll_on_output);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(transbg_cb), config_data.transparent_background);
		gtk_font_button_set_font_name(GTK_FONT_BUTTON(font_button), config_data.vte_font);
		gtk_range_set_value(GTK_RANGE(satur_scale), config_data.background_saturation * 100);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(historylen_spin), config_data.history_len);
	}

	gtk_dialog_run(GTK_DIALOG(setup_window));
}
/* }}} */

