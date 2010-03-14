/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_EXPORTER_H
#define SOLANG_EXPORTER_H

#include <tr1/memory>

#include <glibmm.h>
#include <gtkmm.h>

#include "plugin.h"
#include "types.h"

namespace Solang
{

class Application;
class IPhotoDestination;
class RendererRegistry;

class Exporter :
    public Plugin
{
    public:
        Exporter(const IPhotoDestinationPtr & photo_destination,
                 bool standard = false) throw();

        virtual
        ~Exporter() throw();

        virtual void
        init(Application & application) throw();

        virtual void
        final(Application & application) throw();

        virtual void
        visit_renderer(BrowserRenderer & browser_renderer) throw();

        virtual void
        visit_renderer(EnlargedRenderer & enlarged_renderer) throw();

        virtual void
        visit_renderer(SlideshowRenderer & slideshow_renderer) throw();

    protected:
        void
        export_photos_async(bool create_archive) throw();

        void
        on_action_photo_export() throw();

        void
        on_archive_maker_ready(const ArchiveMakerPtr &,
                               const std::string & archive_path,
                               const PhotoListPtr & photos) throw();

        void
        on_photo_destination_init_end(bool status) throw();

        void
        on_exporter_dialog_response(
            gint response_id,
            ExporterDialogPtr & exporter_dialog) throw();

        void
        on_renderer_changed(RendererRegistry & renderer_registry)
                            throw();

        void
        ui_hide() throw();

        void
        ui_show() throw();

        ApplicationPtr application_;

        IPhotoDestinationPtr photoDestination_;

        bool standard_;

        ActionGroupPtr actionGroup_;

        Gtk::UIManager::ui_merge_id standardUIID_;

        Gtk::UIManager::ui_merge_id uiID_;

    private:
};

} // namespace Solang

#endif // SOLANG_EXPORTER_H
