/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010 Debarshi Ray <rishi@gnu.org>
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

/*
 * Based on code by: Alexander Larsson
 *
 * This widget was originally written in C as a part of Nautilus:
 * libnautilus-private/nautilus-progress-info.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "progress-widget.h"

namespace Solang
{

ProgressWidget::ProgressWidget() throw() :
    Gtk::VBox(false, 5),
    button_(),
    hBox_(false, 10),
    image_(Gtk::Stock::CANCEL, Gtk::ICON_SIZE_BUTTON),
    detailsLabel_("details", false),
    statusLabel_("status", false),
    progressBar_(),
    vBox_(false, 0)
{
    statusLabel_.set_alignment(0.0, 0.5);
    statusLabel_.set_line_wrap(true);
    statusLabel_.set_line_wrap_mode(Pango::WRAP_WORD_CHAR);
    statusLabel_.set_size_request(500, -1);
    pack_start(statusLabel_, true, false, 0);

    progressBar_.set_pulse_step(0.05);
    vBox_.pack_start(progressBar_, true, false, 0);
    hBox_.pack_start(vBox_, true, true, 0);

    button_.add(image_);
    hBox_.pack_start(button_, false, false, 0);

    pack_start(hBox_, false, false, 0);

    detailsLabel_.set_alignment(0.0, 0.5);
    detailsLabel_.set_line_wrap(true);
    detailsLabel_.set_use_markup(true);
    pack_start(detailsLabel_, true, false, 0);

    show_all_children();
}

ProgressWidget::~ProgressWidget() throw()
{
}

void
ProgressWidget::set_details(const Glib::ustring & details) throw()
{
    const Glib::ustring markup
        = Glib::ustring::compose("<span size='small'>%1</span>",
                                 Glib::Markup::escape_text(details));
    detailsLabel_.set_markup(markup);
}

void
ProgressWidget::set_fraction(gdouble fraction) throw()
{
    progressBar_.set_fraction(fraction);
}

void
ProgressWidget::set_status(const Glib::ustring & status) throw()
{
    statusLabel_.set_text(status);
}

Glib::SignalProxy0<void>
ProgressWidget::signal_cancelled() throw()
{
    return button_.signal_clicked();
}

} // namespace Solang
