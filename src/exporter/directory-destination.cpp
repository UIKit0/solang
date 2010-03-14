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

#include <string>

#include <giomm.h>
#include <glib/gstdio.h>
#include <glibmm/i18n.h>

#include "directory-destination.h"
#include "i-progress-observer.h"
#include "photo.h"
#include "photo-destination-enums.h"

namespace Solang
{

DirectoryDestination::DirectoryDestination() throw() :
    PhotoDestination(),
    createArchive_(false),
    filename_(),
    fileChooserButton_(0),
    initEnd_()
{
}

DirectoryDestination::~DirectoryDestination() throw()
{
}

void
DirectoryDestination::init(Application & application) throw()
{
    initEnd_.emit(true);
}

void
DirectoryDestination::final(Application & application) throw()
{
}

void
DirectoryDestination::export_photo_async(
                          const PhotoPtr & photo,
                          const PhotoListPtr & pending,
                          const ProgressObserverPtr & observer)
                          throw()
{
    const FilePtr file = Gio::File::create_for_uri(photo->get_uri());
    const FilePtr dest = Gio::File::create_for_path(
                             filename_ + "/" + file->get_basename());

    file->copy_async(
        dest,
        sigc::slot<void, goffset, goffset>(),
        sigc::bind(
            sigc::mem_fun(*this,
                          &DirectoryDestination::on_async_copy_ready),
            file,
            pending,
            observer),
        observer,
        Gio::FILE_COPY_NONE,
        Glib::PRIORITY_DEFAULT);
}

void
DirectoryDestination::export_photos_async(
                          const PhotoListPtr & photos,
                          const ProgressObserverPtr & observer)
                          throw()
{
    if (0 != observer)
    {
        observer->set_description(_("Exporting photos"));
        observer->set_total(photos->size());
    }

    export_photo_async(photos->back(), photos, observer);

    return;
}

void DirectoryDestination::final() throw()
{
}

sigc::signal<void, bool> &
DirectoryDestination::init_end() throw()
{
    return initEnd_;
}

void
DirectoryDestination::on_async_copy_ready(
                          const AsyncResultPtr & async_result,
                          const FilePtr & file,
                          const PhotoListPtr & pending,
                          const ProgressObserverPtr & observer)
                          throw()
{
    try
    {
        file->copy_finish(async_result);
    }
    catch (const Gio::Error & e)
    {
        switch (e.code())
        {
        case Gio::Error::CANCELLED:
            return;
            break;

        default:
            g_warning("%s", e.what().c_str());
            break;
        }
    }

    if (0 != observer)
    {
        observer->progress();
    }

    pending->pop_back();

    if (false == pending->empty())
    {
        export_photo_async(pending->back(), pending, observer);
    }
}

void
DirectoryDestination::on_child_watch(GPid,
                                     int,
                                     const std::string & filename)
                                     throw()
{
    g_remove(filename.c_str());
}

void
DirectoryDestination::read_selection() throw()
{
    try
    {
        filename_ = Glib::filename_from_utf8(
                        fileChooserButton_->get_filename());
    }
    catch (const Glib::ConvertError & e)
    {
        g_warning("%s", e.what().c_str());
        return;
    }
}

Gtk::Widget &
DirectoryDestination::get_browser() throw()
{
    fileChooserButton_ = new Gtk::FileChooserButton(
                             _("Select Folder"),
                             Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);

    return *Gtk::manage(fileChooserButton_);
}

Glib::ustring
DirectoryDestination::get_label() const throw()
{
    return Glib::ustring(_("_Folder..."));
}

Glib::ustring
DirectoryDestination::get_name() const throw()
{
    return Glib::ustring("Folder");
}

gint
DirectoryDestination::get_options() const throw()
{
    return PHOTO_DESTINATION_CREATE_ARCHIVE_OPTIONAL;
}

Gtk::StockID
DirectoryDestination::get_stock_id() const throw()
{
    return Gtk::Stock::DIRECTORY;
}

void
DirectoryDestination::set_create_archive(bool value) throw()
{
    createArchive_ = value;
}

} // namespace Solang
