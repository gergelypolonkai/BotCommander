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
 *            commands.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * BotCommander internal command definitions
 }}} */

/* TODO: Put this into a Makefile */
#define _GNU_SOURCE
/* {{{ Includes */
#include <libintl.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "typedefs.h"
#include "functions.h"
#include "variables.h"
/* }}} */

/* {{{ Prototypes */
BC_COMMAND(command_exit);
BC_COMMAND(command_close);
BC_COMMAND(command_connect);
BC_COMMAND(command_assign);
BC_COMMAND(command_mode);
BC_COMMAND(command_histlist);
BC_COMMAND(command_botlist);
BC_COMMAND(command_preferences);
BC_COMMAND(command_disconnect);
/* }}} */

/* {{{ internal_command[] */
t_command internal_command[] = {
/*	verb,           CT_INTERNAL, paramnum, can_abbrev, function */
	{"QUIT",        CT_INTERNAL, 0,        FALSE,      command_exit},
	{"CLOSE",       CT_INTERNAL, 0,        TRUE,       command_close},
	{"CONNECT",     CT_INTERNAL, 0,        TRUE,       command_connect},
	{"ASSIGN",      CT_INTERNAL, 1,        TRUE,       command_assign},
	{"MODE",        CT_INTERNAL, 1,        TRUE,       command_mode},
	{"HISTLIST",    CT_INTERNAL, 0,        TRUE,       command_histlist},
	{"BOTLIST",     CT_INTERNAL, 0,        TRUE,       command_botlist},
	{"PREFERENCES", CT_INTERNAL, 0,        TRUE,       command_preferences},
	{"DISCONNECT",  CT_INTERNAL, 0,        TRUE,       command_disconnect},
	{NULL,          CT_INTERNAL, 0,        FALSE,      NULL},
};
/* }}} */

#define num_commands (sizeof(command) / sizeof(t_command))

/* {{{ command_exit */
BC_COMMAND(command_exit)
{
	exit_cleanly(FALSE);
	return FALSE;
}
/* }}} */

/* {{{ command_close */
BC_COMMAND(command_close)
{
	Context;
	close_tab(tab);
	Context;
	return FALSE;
}
/* }}} */

/* {{{ command_connect */
BC_COMMAND(command_connect)
{
	if (tab->connected)
	{
		feed_error_to_terminal(tab, TRUE, _("This tab is already connected. Exit the bot with .quit, or use /DISCONNECT!"));
		return TRUE;
	}
	if ((paramnum == 0) && (tab->bot == NULL))
	{
		feed_error_to_terminal(tab, TRUE, _("No bot is assigned to this tab. Use ASSIGN to do that."));
		return TRUE;
	}
	if (paramnum == 0)
	{
#ifdef DEBUG /* {{{ */
		if (config_data.debug)
			feed_message_to_terminal(tab, TRUE, _("Connecting to %s"), tab->bot->botname);
#endif /* DEBUG }}} */
	}
	if (paramnum == 1)
	{
		t_bot_data *bot;

		if ((bot = get_bot_by_botname(param[1])) == NULL)
		{
			feed_error_to_terminal(tab, TRUE, _("No bot exists by that name"));
			return TRUE;
		}
		assign_bot_to_tab(tab, bot);
#ifdef DEBUG /* {{{ */
		if (config_data.debug)
			feed_message_to_terminal(tab, TRUE, _("Connecting to %s"), bot->botname);
#endif /* DEBUG }}} */
	}
	if (paramnum > 1)
	{
		gchar *host = param[1];
		gint port;
		t_bot_data *bot;

		if (!is_numeric(param[2], &port))
		{
			feed_error_to_terminal(tab, TRUE, _("Second parameter must be a port number!"));
			return TRUE;
		}
		if (create_bot_record(host, port, &bot) != BCE_SUCCESS)
		{
			feed_error_to_terminal(tab, TRUE, _("Not enough memory!"));
			return TRUE;
		}
		tab->bot = bot;
		set_title(tab, "%s:%d", host, port);
#ifdef DEBUG /* {{{ */
		if (config_data.debug)
			feed_message_to_terminal(tab, TRUE, _("Connecting to %s:%d"), host, port);
#endif /* DEBUG }}} */
	}

	connect_tab(tab);
	return TRUE;
}
/* }}} */

/* {{{ command_assign */
BC_COMMAND(command_assign)
{
	if (!assign_bot_to_tab_by_name(tab, param[1]))
	{
		feed_error_to_terminal(tab, TRUE, _("No bot exists by this name"));
		return TRUE;
	}
	return TRUE;
}
/* }}} */

/* {{{ command_mode */
BC_COMMAND(command_mode)
{
	char newmode;
	if (paramnum < 1)
	{
		feed_error_to_terminal(tab, TRUE, _("Not enough parameters!"));
		return TRUE;
	}
	if (paramnum > 1)
	{
		feed_error_to_terminal(tab, TRUE, _("Only one parameter required!"));
		return TRUE;
	}
	if (strlen(param[1]) > 1)
	{
		feed_error_to_terminal(tab, TRUE, _("The parameter must be one character!"));
		return TRUE;
	}
	newmode = *param[1];
	if (!strchr("bem", tolower(newmode)))
	{
		feed_error_to_terminal(tab, TRUE, _("Mode char can be one of B, E or M!"));
		return TRUE;
	}

	change_mode(newmode, FALSE);
	return TRUE;
}
/* }}} */

/* {{{ command_histlist */
BC_COMMAND(command_histlist)
{
	t_history_data *temp = tab->history;

	while (temp)
	{
		feed_info_to_terminal(tab, TRUE, temp->line);
		temp = temp->next;
	}

	return TRUE;
}
/* }}} */

/* {{{ command_botlist */
BC_COMMAND(command_botlist)
{
#ifdef DEBUG
	list_bots();
#endif

	display_botlist_window();

	return TRUE;
}
/* }}} */

/* {{{ command_preferences */
BC_COMMAND(command_preferences)
{
	display_setup_window();

	return TRUE;
}
/* }}} */

/* {{{ command_disconnect */
BC_COMMAND(command_disconnect)
{
	disconnect_tab(tab);

	return TRUE;
}
/* }}} */

/* {{{ get_command_from_abbrev() */
/* Returning rules:
 *   If the given abbreviation is a whole command, that command will be returned
 *   If there are more commands matching the pattern, and only one of them can be abbreviated, that command will be returned
 *   If there are multiple commands matching the pattern, and more of them can be abbreviated, nothing is returned
 */
t_command *
get_command_from_abbrev(gchar *abbrev)
{
	t_command *temp, *found_command = NULL;
	gchar *a;

	for (temp = internal_command; temp->command_verb; temp++)
	{
		gboolean found = TRUE;

		if (streq(temp->command_verb, abbrev, FALSE))
			return temp;
		if (strlen(abbrev) > strlen(temp->command_verb))
			continue;
		for (a = abbrev; *a; a++)
		{
			if (tolower(*a) != tolower(*(temp->command_verb + (a - abbrev))))
			{
				found = FALSE;
				break;
			}
		}
		if ((found) && (temp->can_abbrev == TRUE))
		{
			if (found_command)
				return NULL;
			found_command = temp;
		}
	}

	return found_command;
}
/* }}} */

/* {{{ process_commandline() */
gboolean
process_commandline(gchar *cmdline, t_tab_data *tab)
{
	gchar **wl;
	gint wnum;
	t_command *this_command;
	gboolean want_change = TRUE;

	Context;
	
	if (wordwrap(cmdline, " \t", &wl, &wnum) != BCE_SUCCESS)
	{
		feed_error_to_terminal(tab, TRUE, _("Not enough memory!"));
	}

	Context;
	
	if (!wnum)
		return TRUE;

	this_command = get_command_from_abbrev(wl[0]);
	if (this_command)
	{
		if (!this_command->can_abbrev && (strlen(wl[0]) != strlen(this_command->command_verb)))
			feed_error_to_terminal(tab, TRUE, _("This command cannot be abbreviated!"));
		else
		{
			if (this_command->min_paramnum > wnum - 1)
				feed_error_to_terminal(tab, TRUE, _("Not enough parameters!"));
			else
				want_change = this_command->func(tab, wnum - 1, wl);
		}
	}
	else
		feed_error_to_terminal(tab, TRUE, _("Unknown command \"%s\""), wl[0]);

	Context;

	wl_free(&wl, wnum);

	Context;

	return want_change;
}
/* }}} */

/* {{{ process_botcommander_script_commands() */
/* This function processes the messages sent by the botcommander.tcl script. */
void
process_botcommander_script_commands(t_tab_data *tab, char *string)
{
	/* Here the script command is already in UTF-8 */

	Context;

	switch (*(string + 2))
	{
		case '+':
			if (strlen(string) > 4)
			{
				char *command = (string + 4);

				Context;
				if (strcmp(command, "ABILITY") == 0)
				{
					dprintf(tab->sock, ".botcomm_chanlist\n");
					dprintf(tab->sock, ".botcomm_userlist\n");
				}
				else if (strcmp(command, "CHANLIST_START") == 0)
				{
					tab->chanlist_process = TRUE;
					if (tab->actual_channel_list != NULL)
						g_list_free(tab->actual_channel_list);
					tab->actual_channel_list = NULL;
				}
				else if (strcmp(command, "CHANLIST_END") == 0)
				{
					tab->actual_channel_list = g_list_reverse(tab->actual_channel_list);
					tab->chanlist_process = FALSE;
					update_channel_list(tab);
				}
				else if ((strncmp(command, "CHAN ", 5) == 0) && (tab->chanlist_process))
				{
					t_channel *chan = g_malloc(sizeof(t_channel));

					chan->active = (*(command + 5) == '1') ? TRUE : FALSE;
					chan->name = g_strdup((command + 7));
					chan->properties = NULL;
					tab->actual_channel_list = g_list_prepend(tab->actual_channel_list, chan);
				}
				else if (strcmp(command, "USERLIST_START") == 0)
				{
					tab->userlist_process = TRUE;
					if (tab->actual_user_list != NULL)
						g_list_free(tab->actual_user_list);
					tab->actual_user_list = NULL;
				}
				else if (strcmp(command, "USERLIST_END") == 0)
				{
					tab->actual_user_list = g_list_reverse(tab->actual_user_list);
					tab->userlist_process = FALSE;
					update_user_list(tab);
				}
				else if ((strncmp(command, "USER ", 5) == 0) && (tab->userlist_process))
				{
					t_user *user = g_malloc(sizeof(t_user));

					user->name = g_strdup((command + 5));
					tab->actual_user_list = g_list_prepend(tab->actual_user_list, user);
				}
				else if (strncmp(command, "CHANPROPS_START ", 16) == 0)
				{
					tab->chanprops_process = g_strdup(command + 16);
					tab->chanprops_list_num = 0;
				}
				else if (strncmp(command, "CHANPROPS_END ", 14) == 0)
				{
					if (tab->chanprops_process != NULL)
					{
						g_free(tab->chanprops_process);
						tab->chanprops_process = NULL;
					}
					tab->chanprops_list_num = 0;
				}
				else if (strncmp(command, "CHANPROP ", 9) == 0)
				{
					gchar *chan = command + 9;
					gchar *property;

					for (property = chan; *property; property++)
						if (isspace(*property))
							break;
					if (*property)
					{
						*property = 0;
						if (streq(chan, tab->chanprops_process, FALSE))
						{
							property++;
							for (; *property; property++)
								if (!isspace(*property))
									break;
							tab->chanprops_list_num++;
							update_channel_property(tab, chan, property);
						}
					}
				}
#ifdef DEBUG
				else if (config_data.debug)
					printf("Unknown script command: %s\n", command);
#endif /* DEBUG */
				Context;
			}
			break;
		case '-':
			feed_error_to_terminal(tab, FALSE, "%s", string);
			break;
		default:
			return;
	}

	Context;

#ifdef DEBUG
	if (config_data.debug)
	{
		feed_message_to_terminal(tab, TRUE, string);
		printf("%s\n", string);
	}
#endif /* DEBUG */
}
/* }}} */

