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

#include <glibmm/i18n.h>

#include "date-photo-info.h"
#include "engine.h"
#include "i-photo-destination.h"
#include "i-photo-source.h"
#include "photo-tag.h"
#include "tag.h"

namespace Solang
{

Engine::Engine(int & argc, char ** & argv) throw() :
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
    deleteActions_.execute_actions();
}

void
Engine::criteria_changed() throw()
{
    criterionRepo_.update();
    IPhotoSearchCriteriaList criteria
                                 = criterionRepo_.get_criterion();
    criteriaChanged_.emit(criteria);
}

void
Engine::search_async(const IPhotoSearchCriteriaList & criteria,
                     const Database::SlotAsyncPhotos & slot) const
                     throw()
{
    database_.search_async(criteria, slot);
}

void
Engine::erase(const PhotoList & photos)
{
    return;
}

void
Engine::get_tags_async(bool all, const Database::SlotAsyncTags & slot)
                       const throw()
{
    database_.get_tags_async(all, slot);
}

DatePhotoInfoList
Engine::get_dates_with_picture_count()
{
    return database_.get_dates_with_picture_count( );
}

DatePhotoInfoList
Engine::get_dates_with_picture_count( gint year )
{
    return database_.get_dates_with_picture_count(year);
}

DatePhotoInfoList
Engine::get_dates_with_picture_count( gint year, gint month )
{
    return database_.get_dates_with_picture_count(year, month);
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

sigc::signal<void, IPhotoSearchCriteriaList &> &
Engine::signal_criteria_changed() throw()
{
    return criteriaChanged_;
}

PhotoSet &
Engine::get_export_queue() throw()
{
    return exportQueue_;
}

} //namespace Solang
