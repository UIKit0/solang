/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010 Debarshi Ray <rishi@gnu.org>
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

#include <libgen.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <glibmm/i18n.h>
#include <gphoto2.h>

#include "camera-source.h"
#include "progress-observer.h"
#include "i-storage.h"
#include "photo-source-enums.h"
#include "photo-tag.h"
#include "progress-observer.h"
#include "tag.h"

namespace Solang
{

CameraSource::CameraSource() throw()
    : PhotoSource(),
    gpContext_(),
    iconFactory_(Gtk::IconFactory::create()),
    select_( gpContext_ ),
    initEnd_()
{
    Gtk::IconSource icon_source;
    Gtk::IconSet icon_set_camera_photo;

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "camera-photo-16.png");
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_camera_photo.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "camera-photo-22.png");
    icon_source.set_size(Gtk::IconSize(22));
    icon_set_camera_photo.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "camera-photo-24.png");
    icon_source.set_size(Gtk::IconSize(24));
    icon_set_camera_photo.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "camera-photo-32.png");
    icon_source.set_size(Gtk::IconSize(32));
    icon_set_camera_photo.add_source(icon_source);

    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-camera-photo"),
                      icon_set_camera_photo);
    iconFactory_->add_default();

    select_.signal_show().connect(
            sigc::mem_fun( &select_, &CameraImportWidget::populate ));
}

CameraSource::~CameraSource() throw()
{
    iconFactory_->remove_default();
}

void
CameraSource::init(Application & application) throw()
{
    initEnd_.emit(true);
}

void
CameraSource::final(Application & application) throw()
{
}

PhotoPtr
CameraSource::import(const PhotoPtr & photo,
                     const IStoragePtr & storage,
                     const TagList &tags, Database & db,
                     const ProgressObserverPtr & observer) throw()
{
    if (photo)
    {
        Glib::ustring path = photo->get_disk_file_path();
        download_file_from_camera( photo );
        storage->save(photo, true);
        for( TagList::const_iterator it = tags.begin();
                            it != tags.end(); it++ )
        {
            PhotoTag pt(
                    photo->get_photo_id(), (*it)->get_tag_id());
            pt.save(db);
        }
    }

    return PhotoPtr(photo);
}

PhotoList
CameraSource::import(const PhotoList & photos,
                     const IStoragePtr & storage,
                     const TagList &tags, Database & db,
                     const ProgressObserverPtr & observer) throw()
{
    if (0 != observer)
    {
        observer->set_event_description(_("Importing photos"));
        observer->set_num_events(photos.size());
        observer->set_current_events(0);
    }

    PhotoList imported_photos;
    for (PhotoList::const_iterator it = photos.begin();
                                   it != photos.end(); it++ )
    {
        if( observer )
        {
            if( observer->get_stop() )
            {
                return imported_photos;
            }
        }
        PhotoPtr photo = (*it);
        imported_photos.push_back(import(photo, storage, tags, db,
                                         observer));

        if (0 != observer)
        {
            observer->receive_event_notifiation();
        }
    }

    if (0 != observer)
    {
        observer->reset();
    }

    return imported_photos;
}

PhotoList
CameraSource::import(const IStoragePtr & storage, const TagList &tags,
                     Database & db,
                     const ProgressObserverPtr & observer) throw()
{
    PhotoList files;
    create_photo_list( files );
    PhotoList tmpList = import(files, storage, tags, db, observer);
    gpContext_.cleanup(); //So that this can be recreated
    return tmpList;
}

void
CameraSource::create_photo_list( PhotoList &files )
{
    const Gtk::TreeView & cameraPhotos = select_.get_camera_photos();
    const CameraImportWidget::PictureSelectColumns &cols
                                = select_.get_camera_photo_columns();

    Gtk::TreeModel::Children children
                        = cameraPhotos.get_model()->children();
    for( Gtk::TreeModel::iterator it = children.begin();
                it != children.end(); it ++ )
    {
        Gtk::TreeModel::Row row = (*it);
        if( true == row[ cols.selected_ ] )
        {
            std::ostringstream sout;
            sout<<row[ cols.picturePath_ ]<<"/"
                << row[ cols.pictureName_];
            PhotoPtr photo( new Photo() );
            photo->set_disk_file_path( sout.str() );
            files.push_back( photo );
        }
    }
}

void
CameraSource::download_file_from_camera( const PhotoPtr &photo )
{
    Glib::ustring path = photo->get_disk_file_path();
    CameraFile *cImg;
    Glib::ustring cImgPath = "/tmp/" + path;
    Glib::ustring folder;
    Glib::ustring file;

    {
        char tmpPath[ PATH_MAX ];
        strcpy( tmpPath, path.c_str() );
        folder = dirname( tmpPath );
    }

    {
        char tmpPath[ PATH_MAX ];
        strcpy( tmpPath, path.c_str() );
        file = basename( tmpPath );
    }
    try
    {
        Glib::RefPtr<Gio::File> tmp
                = Gio::File::create_for_path("/tmp/"+folder);
        if( !tmp->query_exists() )
        {
            tmp->make_directory_with_parents();
        }
    }
    catch( Glib::Error &e)
    {
        std::cout<<"Error::"<<e.what()<<std::endl;
    }

    int fd = open( cImgPath.c_str(), O_CREAT | O_WRONLY, 0644 );

    if( gp_file_new_from_fd( &cImg, fd  ) )
    {
        return;
    }
    if( GP_OK > gp_camera_file_get(
                    gpContext_.camera_, folder.c_str(),
                    file.c_str(), GP_FILE_TYPE_NORMAL,
                    cImg, gpContext_.context_ ) )
    {
        gp_file_unref( cImg );
        return;
    }
    gp_file_unref( cImg );

    photo->set_disk_file_path( cImgPath );
}

sigc::signal<void, bool> &
CameraSource::init_end() throw()
{
    return initEnd_;
}

void
CameraSource::read_selection() throw()
{
}

Gtk::Widget &
CameraSource::get_browser() throw()
{
    return select_;
}

Glib::ustring
CameraSource::get_label() const throw()
{
    return Glib::ustring("_Camera...");
}

Glib::ustring
CameraSource::get_name() const throw()
{
    return Glib::ustring("Camera");
}

gint
CameraSource::get_options() const throw()
{
    return PHOTO_SOURCE_COPY_PHOTOS_TRUE
           | PHOTO_SOURCE_INCLUDE_SUBFOLDERS_FALSE;
}

Gtk::StockID
CameraSource::get_stock_id() const throw()
{
    return Gtk::StockID(PACKAGE_TARNAME"-camera-photo");
}

} // namespace Solang
