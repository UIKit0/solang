/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2009 Santanu Sinha <santanu.sinha@gmail.com>
 *
 * Solang is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solang is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cstdlib>

#include <giomm.h>
#include <glibmm.h>
#include <glibmm/i18n.h>
#include <gtkmm.h>

#ifdef ENABLE_NLS
#include <libintl.h>
#endif // ENABLE_NLS

#include "application.h"
#include "types.h"

int
main(int argc, char *argv[])
{
    bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    Glib::thread_init();
    Gio::init();

    Glib::set_prgname(PACKAGE_TARNAME);
    Glib::set_application_name(_(PACKAGE_NAME));

    Gtk::Main kit(argc, argv, true);

    const Solang::IconThemePtr icon_theme = Gtk::IconTheme::get_default();
    icon_theme->append_search_path(DATA_DIR G_DIR_SEPARATOR_S
                                   "icons");

    Solang::Application application(argc, argv);

    application.init();
    application.run();
    application.final();

    return EXIT_SUCCESS;
}
