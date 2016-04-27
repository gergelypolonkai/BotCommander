/* vim: set foldmethod=marker : */
/* {{{
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
 *            variables.h
 *
 *  Wed Nov 09 22:16:59 2005
 *  Copyright (C) 2005 Gergely POLONKAI
 *  polesz@techinfo.hu
 *
 * Variable definitions
 }}} */

/* {{{ _BC_VARIABLES_H */
#ifndef _BC_VARIABLES_H
# define _BC_VARIABLES_H

/* {{{ Includes */
# include <gtk/gtk.h>
# include <gconf/gconf-client.h>

#include "typedefs.h"
/* }}} */

/* {{{ widgets.c */
extern GtkWidget *main_window;
extern GtkWidget *main_book;
extern GtkWidget *main_statuslabel_mode;
extern GtkWidget *main_menu_mode_b;
extern GtkWidget *main_menu_mode_e;
extern GtkWidget *main_menu_mode_m;
extern GtkWidget *main_menu_show_sidebar;
/* }}} */

/* {{{ tabs.c */
extern GList *tab_list;
/* }}} */

/* {{{ bots.c */
extern GList *internal_bot_list;
/* }}} */

/* {{{ config.c */
extern t_config_data config_data;
extern GConfClient *gconf_client;
/* }}} */

#endif
/* }}} */

