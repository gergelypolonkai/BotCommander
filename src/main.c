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
 *            main.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Very common functions
 }}} */

/* {{{ Includes */
#include <stdio.h>
#include <libintl.h>
#include <locale.h>
#include <glib.h>
#include <string.h>
#include <ctype.h>

#include "config.h"

#include "functions.h"
#include "variables.h"
#include "defines.h"
/* }}} */

/* {{{ change_mode() */
void
change_mode(char newmode, gboolean from_menu)
{
	char *buf;

	t_tab_data *active_tab = get_active_tab();

	buf = g_malloc(strlen(_("Mode: %c")) + 1);
	sprintf(buf, _("Mode: %c"), toupper(newmode));

	active_tab->mode = toupper(newmode);
	gtk_label_set_text(GTK_LABEL(main_statuslabel_mode), buf);
	g_free(buf);

	if (!from_menu)
	{
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(main_menu_mode_b), (toupper(newmode) == 'B') ? TRUE : FALSE);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(main_menu_mode_e), (toupper(newmode) == 'E') ? TRUE : FALSE);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(main_menu_mode_m), (toupper(newmode) == 'M') ? TRUE : FALSE);
	}
}
/* }}} */

/* {{{ find_connected_tab() */
gint
find_connected_tab(gconstpointer a, gconstpointer b)
{
	if (((t_tab_data *)a)->connected)
		return 0;

	return 1;
}
/* }}} */

/* {{{ exit_cleanly() */
void
exit_cleanly(gboolean force)
{
	gboolean ask = FALSE;

	save_bot_list_to_config();

	if (g_list_find_custom(tab_list, NULL, find_connected_tab))
		ask = TRUE;

	if (ask)
	{
		GtkWidget *dialog;
		gint result;

		dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("There are connected tabs. Are you sure, you want to exit?"));
		result = gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
		if (result == GTK_RESPONSE_NO)
			return;
	}

#ifdef DEBUG
	printf("exit_cleanly(%d)\n", force);
#endif /* DEBUG */
	gtk_main_quit();
}
/* }}} */

/* {{{ main() */
int
main(int argc, char **argv)
{
#ifdef DEBUG
	debug_init();
#endif /* DEBUG */

	Context;
	setlocale(LC_ALL, "");
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(GETTEXT_PACKAGE);

	Context;

	gtk_init(&argc, &argv);

	if (!init_config())
		return 1;

	read_config_data();
	read_bot_list_from_config();

	tab_list = NULL;

	g_set_prgname("BotCommander");
	g_set_application_name("BotCommander");

	create_main_window();

	add_tab(NULL);
	
	gtk_main();
	return 0;
}
/* }}} */

