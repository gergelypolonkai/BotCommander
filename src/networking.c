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
 *            networking.c
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Network-handling functions
 }}} */

/* {{{ Includes */
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <libintl.h>
#include <errno.h>
#include <fcntl.h>

#include "config.h"

#include "defines.h"
#include "typedefs.h"
#include "functions.h"
#include "variables.h"
/* }}} */

/* {{{ connect_to_host() */
gint
connect_to_host(gchar *host, guint port, int *fd, gint *tag, t_tab_data *tab)
{
	int desc;
	static struct protoent *p = NULL;
	struct hostent *h = NULL;
	struct sockaddr_in dest;

	if (tab)
		feed_info_to_terminal(tab, TRUE, _("Connecting to %s:%d"), host, port);

	if (!p)
	{
		if ((p = getprotobyname("IP")) == NULL)
			return BCE_NOPROTO;
	}
	desc = socket(PF_INET, SOCK_STREAM, p->p_proto);
	fcntl(desc, F_SETFL, O_NONBLOCK);

	if ((h = gethostbyname(host)) == NULL)
		return BCE_CANTRESOLV;

	memset(&dest, 0, sizeof(struct sockaddr_in));
	dest.sin_family = AF_INET;
	dest.sin_addr = *((struct in_addr *)h->h_addr);
	dest.sin_port = htons(port);

	if (connect(desc, (struct sockaddr *)&dest, sizeof(struct sockaddr)) < 0)
	{
		if (errno != EINPROGRESS)
		{
			if (tab)
			{
				char *buf = NULL;
				char *orig = strerror(errno);

				buf = g_locale_to_utf8(orig, -1, NULL, NULL, NULL);
				feed_error_to_terminal(tab, TRUE, "%s", buf);
				g_free(buf);
			}
#ifdef DEBUG
			perror("BotCommander: connect");
#endif /* DEBUG */
			return BCE_CANTCONNECT;
		}
	}

	if (fd)
		*fd = desc;
	if (tag)
		*tag = gdk_input_add(desc, GDK_INPUT_READ | GDK_INPUT_EXCEPTION, socket_event, tab);

	return BCE_SUCCESS;
}
/* }}} */

/* {{{ process_incoming_data() */
void
process_incoming_data(t_tab_data *tab, char *string)
{
	char *obuf;

	obuf = g_convert(string, -1, "UTF-8", BOT_CHARSET, NULL, NULL, NULL);
	if (strncmp(obuf, "BC", 2) == 0)
		process_botcommander_script_commands(tab, obuf);
	else
		feed_message_to_terminal(tab, TRUE, obuf);
}
/* }}} */

