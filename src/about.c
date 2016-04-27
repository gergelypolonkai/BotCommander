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
 *            about.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * The About dialog
 }}} */

/* {{{ Includes */
#include <gtk/gtk.h>
#include <libintl.h>

#include "config.h"
#include "defines.h"
#include "functions.h"
/* }}} */

/* {{{ authors[] */
const gchar *authors[] = {
	"Gergely POLONKAI <polesz@w00d5t0ck.info>",
	"KrzychuG from #eggdrop@IRCNET as Beta Tester",
	"Peter \"FaZy\" Erdosi as Beta Tester",
	NULL,
};
/* }}} */

/* {{{ documenters[] */
const gchar *documenters[] = {
	"Gergely POLONKAI <polesz@w00d5t0ck.info>",
	NULL,
};
/* }}} */

/* {{{ display_about_box() */
void
display_about_box(void)
{
	static GtkWidget *about_box = NULL;
	GdkPixbuf *logo;
	gchar *translator_credits = _("translator_credits");
	gchar *licence = _("licence");

	if (!GTK_IS_WINDOW(about_box))
	{
		char *buf = NULL;

		buf = g_locale_to_utf8(licence, -1, NULL, NULL, NULL);
		logo = gdk_pixbuf_new_from_file(PACKAGE_PIXMAPS_DIR "/bc48_trans.png", NULL);

		about_box = gtk_about_dialog_new();
		gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about_box), "BotCommander");
		gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_box), "1.0");
		gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_box), "\xc2\xa9 2005, Gergely POLONKAI");
		gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_box), "Developer's Version");
		gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about_box), buf);
		g_free(buf);
		gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_box), "http://botcommander.hu/");
		gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_box), authors);
		gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(about_box), documenters);
		gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about_box), translator_credits);
		gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about_box), logo);

		if (logo)
			g_object_unref(logo);

		g_signal_connect(G_OBJECT(about_box), "destroy", G_CALLBACK(gtk_widget_destroyed), &about_box);
		gtk_widget_show(about_box);
	}
	else
		gtk_window_present(GTK_WINDOW(about_box));
}
/* }}} */

