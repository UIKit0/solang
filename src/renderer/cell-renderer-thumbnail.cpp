/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) Debarshi Ray 2009, 2010 <rishi@gnu.org>
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

#include <iostream>
#include <cmath>

#include <cairomm/cairomm.h>

#include "cell-renderer-thumbnail.h"
#include "photo.h"
#include "thumbbuf-maker.h"
#include "thumbnailer.h"

namespace Solang
{
CellRendererThumbnail::CellRendererThumbnail() throw() :
    Gtk::CellRendererPixbuf(),
    exportIcon_(0),
    photo_(),
    imageLoading_(0)
{
    property_xalign().set_value(0.5);
    property_yalign().set_value(0.5);

    const IconThemePtr icon_theme = Gtk::IconTheme::get_default();
    icon_theme->signal_changed().connect(
        sigc::mem_fun(*this,
                      &CellRendererThumbnail::on_icon_theme_changed));

    load_icons();
}

CellRendererThumbnail::~CellRendererThumbnail() throw()
{
}

void
CellRendererThumbnail::create_thumbnail(gint thumbnail_height,
                                        gint thumbnail_width) throw()
{
    if (thumbnail_height < -1 || thumbnail_width < -1)
    {
        return;
    }

    if (Photo::THUMBNAIL_STATE_LOADING
            == photo_->get_thumbnail_state())
    {
        property_pixbuf().set_value(imageLoading_);
        return;
    }

    ThumbbufMaker thumbbuf_maker(thumbnail_width,
                                 thumbnail_height,
                                 false);
    PixbufPtr pixbuf;
    try
    {
        pixbuf = thumbbuf_maker(photo_);
        photo_->set_thumbnail_buffer(pixbuf);
        photo_->set_thumbnail_state(Photo::THUMBNAIL_STATE_READY);
    }
    catch (const Gdk::PixbufError & e)
    {
        Thumbnailer & thumbnailer = Thumbnailer::instance();
        thumbnailer.push(photo_);

        pixbuf = imageLoading_;
        photo_->set_thumbnail_state(Photo::THUMBNAIL_STATE_LOADING);
    }
    catch (const Glib::FileError & e)
    {
        Thumbnailer & thumbnailer = Thumbnailer::instance();
        thumbnailer.push(photo_);

        pixbuf = imageLoading_;
        photo_->set_thumbnail_state(Photo::THUMBNAIL_STATE_LOADING);
    }

    property_pixbuf().set_value(pixbuf);
}

void
CellRendererThumbnail::load_export_icon() throw()
{
    const IconThemePtr icon_theme = Gtk::IconTheme::get_default();
    const Gtk::IconInfo icon_info
        = icon_theme->lookup_icon(
                          "conduit-source",
                          22,
                          Gtk::ICON_LOOKUP_FORCE_SIZE);

    if (true == icon_info)
    {
        const Glib::ustring & filename = icon_info.get_filename();
        exportIcon_ = Cairo::ImageSurface::create_from_png(filename);
    }
    else
    {
        exportIcon_.clear();
    }
}

void
CellRendererThumbnail::load_icons() throw()
{
    const IconThemePtr icon_theme = Gtk::IconTheme::get_default();
    try
    {
        imageLoading_ = icon_theme->load_icon(
                            "image-loading",
                            96,
                            static_cast<Gtk::IconLookupFlags>(0));
    }
    catch (const Glib::Error & e)
    {
        std::cerr << G_STRLOC << ", " << G_STRFUNC << ": "
                  << e.what() << std::endl;
    }
}

void
CellRendererThumbnail::on_icon_theme_changed() throw()
{
    load_icons();
}

void
CellRendererThumbnail::render_vfunc(
    const Glib::RefPtr<Gdk::Drawable> & window,
    Gtk::Widget & widget,
    const Gdk::Rectangle & background_area,
    const Gdk::Rectangle & cell_area,
    const Gdk::Rectangle & expose_area,
    Gtk::CellRendererState flags)
{
    if (0 == property_pixbuf().get_value())
    {
        create_thumbnail(cell_area.get_height() - 6,
                         cell_area.get_width() - 6);
    }

    Gtk::CellRendererPixbuf::render_vfunc(window, widget, background_area,
                                          cell_area, expose_area, flags);

    const Cairo::RefPtr<Cairo::Context> context
        = window->create_cairo_context();

    if (true == photo_->get_state_export_queue())
    {
        if (0 == exportIcon_)
        {
            load_export_icon();
        }
        if (0 != exportIcon_)
        {
            const gint x = background_area.get_x();
            const gint y = background_area.get_y();

            context->set_source(exportIcon_, x, y);
            context->paint();
        }
    }
}

void
CellRendererThumbnail::set_photo(const PhotoPtr & photo) throw()
{
    photo_ = photo;
}

} // namespace Solang
