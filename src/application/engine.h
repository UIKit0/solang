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

#ifndef SOLANG_ENGINE_H
#define SOLANG_ENGINE_H

#include <map>

#include <glibmm.h>

#include "database.h"
#include "deletion-queue.h"
#include "i-photo-search-criteria.h"
#include "i-storage.h"
#include "non-copyable.h"
#include "photo.h"
#include "search-criterion-repo.h"
#include "types.h"

namespace Solang
{

class Engine :
    public NonCopyable
{
    public:
        typedef std::map<Glib::ustring, IStoragePtr> StorageMap;

    public:
        Engine(int & argc, char ** & argv) throw();

        ~Engine() throw();

        void
        init(Glib::ustring);

        void
        final();

        void
        criteria_changed() throw();

        void
        search_async(const IPhotoSearchCriteriaList & criteria,
                     const Database::SlotAsyncPhotos & slot) const
                     throw();

        void
        erase(const PhotoList & photos);

        void
        get_tags_async(const Database::SlotAsyncTags & slot) const
                       throw();

        //Group by year
        DatePhotoInfoList
        get_dates_with_picture_count();

        //Group by year, month
        DatePhotoInfoList
        get_dates_with_picture_count( gint year );

        //Group by year, month, day
        DatePhotoInfoList
        get_dates_with_picture_count( gint year, gint month );
    
        Glib::Dispatcher &
        photo_export_begin() throw();

        Glib::Dispatcher &
        photo_export_end() throw();

        Glib::Dispatcher &
        photo_import_begin() throw();

        Glib::Dispatcher &
        photo_import_end() throw();

        sigc::signal<void, IPhotoSearchCriteriaList &> &
        signal_criteria_changed() throw();

        Glib::Dispatcher &
        tag_add_begin() throw();

        Glib::Dispatcher &
        tag_add_end() throw();

        sigc::signal<void> &
        selection_changed() throw();

        inline DatabasePtr
        get_db()
        {
            return DatabasePtr(&database_);
        }

        PhotoList &
        get_export_queue() throw();

        PhotoList
        get_photos() throw();

        inline SearchCriterionRepo &
        get_criterion_repo();

        inline DeletionQueue &
        get_delete_actions();

    private:
        Glib::Dispatcher photoExportBegin_;

        Glib::Dispatcher photoExportEnd_;

        Glib::Dispatcher photoImportBegin_;

        Glib::Dispatcher photoImportEnd_;

        Glib::Dispatcher tagAddBegin_;

        Glib::Dispatcher tagAddEnd_;

        sigc::signal<void, IPhotoSearchCriteriaList &>
            criteriaChanged_;

        sigc::signal<void> selectionChanged_;

        Glib::Mutex mutex_;

        PhotoList exportQueue_;

        PhotoList photos_;

        //The following are populated 
        //from gconf

        StorageMap currentStorageSystems_;

        Database database_;

        SearchCriterionRepo criterionRepo_;

        DeletionQueue deleteActions_;
};

inline SearchCriterionRepo &
Engine::get_criterion_repo()
{
    return criterionRepo_;
}

inline DeletionQueue &
Engine::get_delete_actions()
{
    return deleteActions_;
}

} // namespace Solang

#endif // SOLANG_ENGINE_H
