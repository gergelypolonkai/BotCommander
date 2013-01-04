#include <stdio.h>
#include <libintl.h>
#include <gtk/gtk.h>

#include "config.h"

static void
quit(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	GApplication *app = user_data;

	g_application_quit(app);
}

static void
startup(GtkApplication *app, gpointer user_data)
{
	static const GActionEntry actions[] = {
		{ "quit", quit }
	};

	GMenu *menu;

	g_action_map_add_action_entries(G_ACTION_MAP(app), actions, G_N_ELEMENTS(actions), app);

	menu = g_menu_new();
	g_menu_append(menu, "Quit", "app.quit");
	gtk_application_set_app_menu(app, G_MENU_MODEL(menu));
	g_object_unref(menu);
}

static void
activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *botlist;

	window = gtk_application_window_new(app);
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
	gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "BotCommander");

	botlist = gtk_notebook_new();

	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(botlist));

	gtk_widget_show_all(window);
}

int
main(int argc, char **argv)
{
	GtkApplication *app;
	int status;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	app = gtk_application_new("org.botcommander.gtk", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "startup", G_CALLBACK(startup), NULL);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

