/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
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

#include <glibmm/i18n.h>

#include "date-photo-info.h"
#include "engine.h"
#include "i-photo-destination.h"
#include "i-photo-source.h"
#include "photo-tag.h"
#include "progress-observer.h"
#include "tag.h"

namespace Solang
{

Engine::Engine(int & argc, char ** & argv,
               const ProgressObserverPtr & observer) throw() :
    observer_(observer),
    photoExportBegin_(),
    photoExportEnd_(),
    photoImportBegin_(),
    photoImportEnd_(),
    tagAddBegin_(),
    tagAddEnd_(),
    criteriaChanged_(),
    selectionChanged_(),
    mutex_(),
    exportQueue_(),
    photos_(),
    currentStorageSystems_(),
    database_(),
    criterionRepo_(),
    deleteActions_( database_ )
{
}

Engine::~Engine() throw()
{
}

void
Engine::init(Glib::ustring str)
{
}

void
Engine::final()
{
    deleteActions_.execute_actions( observer_ );
}

void
Engine::criteria_changed() throw()
{
    criterionRepo_.update();
    PhotoSearchCriteriaList criteria = criterionRepo_.get_criterion();
    criteriaChanged_.emit(criteria);
}

void
Engine::import(const PhotoPtr & photo,
               const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags) throw()
{
    import(photo, source, selected_storage, tags, observer_);
    return;
}

void
Engine::import(const PhotoPtr & photo,
               const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags,
               const ProgressObserverPtr & observer) throw()
{
    photoImportBegin_.emit();
#if 0
    for (TagList::const_iterator it = tags.begin();
         it != tags.end(); it++)
    {
        (*it)->save(database_);
    }
#endif
    PhotoPtr imp_photo = source->import(photo, selected_storage,
                                    tags, database_, observer);
    photoImportEnd_.emit();

    PhotoList imp_photos;
    imp_photos.push_back(imp_photo);
    {
        Glib::Mutex::Lock lock(mutex_);
        photos_ = imp_photos;
    }

    return;
}

void
Engine::import(const PhotoList & photos,
               const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags) throw()
{
    import(photos, source, selected_storage, tags, observer_);
    return;
}

void
Engine::import(const PhotoList & photos,
               const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags,
               const ProgressObserverPtr & observer) throw()
{
    photoImportBegin_.emit();
#if 0
    for (TagList::const_iterator it = tags.begin();
         it != tags.end(); it++)
    {
        (*it)->save(database_);
    }
#endif
    PhotoList imp_photos = source->import(photos, selected_storage,
                                          tags, database_, observer);
    photoImportEnd_.emit();

    {
        Glib::Mutex::Lock lock(mutex_);
        photos_ = imp_photos;
    }

    return;
}

void
Engine::import(const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags) throw()
{
    import(source, selected_storage, tags, observer_);
    return;
}

void
Engine::import(const IPhotoSourcePtr & source,
               const IStoragePtr & selected_storage,
               const TagList & tags,
               const ProgressObserverPtr & observer) throw()
{
    photoImportBegin_.emit();
#if 0
    for (TagList::const_iterator it = tags.begin();
         it != tags.end(); it++)
    {
        (*it)->save(database_);
    }
#endif
    PhotoList imp_photos = source->import(selected_storage, tags,
                                          database_, observer);
    photoImportEnd_.emit();

    {
        Glib::Mutex::Lock lock(mutex_);
        photos_ = imp_photos;
    }

    return;
}

void
Engine::search_async(const PhotoSearchCriteriaList & criteria,
                     const Database::SlotAsyncPhotos & slot) const
                     throw()
{
    database_.search_async(criteria, slot);
}

void
Engine::export_photos(const IPhotoDestinationPtr & destination)
                      throw()
{
    export_photos(destination, observer_);
    return;
}

void
Engine::export_photos(const IPhotoDestinationPtr & destination,
                      const ProgressObserverPtr & observer) throw()
{
    photoExportBegin_.emit();
    destination->export_photos(exportQueue_, observer);
    photoExportEnd_.emit();

    return;
}


void
Engine::erase(const PhotoList & photos,
              const ProgressObserverPtr & observer)
{
    ProgressObserverPtr obs = (observer) ? observer : observer_;

    if (obs)
    {
        obs->set_num_events(photos.size());
        obs->set_event_description("Removing Selected Photos");
    }

    return;
}

void
Engine::get_tags_async(const Database::SlotAsyncTags & slot) const
                       throw()
{
    database_.get_tags_async(slot);
}

DatePhotoInfoList
Engine::get_dates_with_picture_count()
{
    return database_.get_dates_with_picture_count( observer_ );
}

DatePhotoInfoList
Engine::get_dates_with_picture_count( gint year )
{
    return database_.get_dates_with_picture_count(year, observer_ );
}

DatePhotoInfoList
Engine::get_dates_with_picture_count( gint year, gint month )
{
    return database_.get_dates_with_picture_count(year, month,
                                                  observer_ );
}

Glib::Dispatcher &
Engine::photo_export_begin() throw()
{
    return photoExportBegin_;
}

Glib::Dispatcher &
Engine::photo_export_end() throw()
{
    return photoExportEnd_;
}

Glib::Dispatcher &
Engine::photo_import_begin() throw()
{
    return photoImportBegin_;
}

Glib::Dispatcher &
Engine::photo_import_end() throw()
{
    return photoImportEnd_;
}

Glib::Dispatcher &
Engine::tag_add_begin() throw()
{
    return tagAddBegin_;
}

Glib::Dispatcher &
Engine::tag_add_end() throw()
{
    return tagAddEnd_;
}

sigc::signal<void> &
Engine::selection_changed() throw()
{
    return selectionChanged_;
}

sigc::signal<void, PhotoSearchCriteriaList &> &
Engine::signal_criteria_changed() throw()
{
    return criteriaChanged_;
}

PhotoList &
Engine::get_export_queue() throw()
{
    return exportQueue_;
}

PhotoList
Engine::get_photos() throw()
{
    Glib::Mutex::Lock lock(mutex_);
    return photos_;
}

} //namespace Solang
