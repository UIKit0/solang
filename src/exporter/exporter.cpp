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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>

#include <sigc++/sigc++.h>

#include "application.h"
#include "archive-maker.h"
//#include "directory-storage.h"
#include "engine.h"
#include "exporter-dialog.h"
#include "exporter.h"
#include "i-photo-destination.h"
#include "photo-factory.h"
#include "progress-observer.h"

namespace Solang
{

Exporter::Exporter(const IPhotoDestinationPtr & photo_destination,
                   bool standard) throw() :
    Plugin(),
    application_(NULL),
    photoDestination_(photo_destination),
    standard_(standard),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2,%3",
                         __FILE__,
                         __LINE__,
                         photo_destination->get_name()))),
    standardUIID_(0),
    uiID_(0)
{
    // if (true == standard_)
    // {
    //     actionGroup_->add(
    //         Gtk::Action::create("ActionPhotoExportStandard",
    //                             "Export"),
    //         sigc::mem_fun(*this, &Exporter::on_action_photo_export));
    // }

    actionGroup_->add(
        Gtk::Action::create("ActionPhotoMenuExportMenu",
                            "Export To"));

    const Glib::ustring action_name
        = "ActionPhotoExport" + photoDestination_->get_name();

    actionGroup_->add(
        Gtk::Action::create(action_name,
                            photoDestination_->get_stock_id(),
                            photoDestination_->get_label()),
        sigc::mem_fun(*this, &Exporter::on_action_photo_export));

    photoDestination_->init_end().connect(
        sigc::mem_fun(*this,
                      &Exporter::on_photo_destination_init_end));
}

Exporter::~Exporter() throw()
{
}

void
Exporter::init(Application & application) throw()
{
    application_ = &application;

    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    // if (true == standard_)
    // {
    //     Glib::ustring standard_ui_info
    //         = "<ui>"
    //           "  <toolbar name='ToolBar'>"
    //           "    <placeholder name='PlaceholderExportToolBar'>"
    //           "    <toolitem action='ActionPhotoExportStandard'/>"
    //           "    </placeholder>"
    //           "  </toolbar>"
    //           "</ui>";

    //     standardUIID_
    //         = ui_manager->add_ui_from_string(standard_ui_info);
    //     if (0 == standardUIID_)
    //     {
    //         // FIXME: error condition.
    //     }
    // }

    Glib::ustring ui_info
        = "<ui>"
          "  <menubar name='MenuBar'>"
          "    <menu action='ActionPhotoMenu'>"
          "      <placeholder name='PlaceholderExportMenus'>"
          "      <menu action='ActionPhotoMenuExportMenu'>"
          "        <placeholder name='PlaceholderExportSubMenus'>"
          "        <menuitem action='ActionPhotoExport"
                       + photoDestination_->get_name()
                       + "'/>"
          "        </placeholder>"
          "      </menu>"
          "      </placeholder>"
          "    </menu>"
          "  </menubar>"
          "</ui>";

    uiID_ = ui_manager->add_ui_from_string(ui_info);
    if (0 == uiID_)
    {
        // FIXME: error condition.
    }

    ui_manager->insert_action_group(actionGroup_);

    initialized_.emit(*this);
}

void
Exporter::final(Application & application) throw()
{
    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    ui_manager->remove_action_group(actionGroup_);
    ui_manager->remove_ui(uiID_);
    // if (true == standard_)
    // {
    //     ui_manager->remove_ui(standardUIID_);
    // }

    finalized_.emit(*this);
}

void
Exporter::visit_renderer(BrowserRenderer & browser_renderer) throw()
{
    ui_show();
}

void
Exporter::visit_renderer(EnlargedRenderer & enlarged_renderer) throw()
{
    ui_show();
}

void
Exporter::visit_renderer(SlideshowRenderer & slideshow_renderer)
                         throw()
{
}

void
Exporter::export_photos_async(bool create_archive) throw()
{
    Engine & engine = application_->get_engine();
    const PhotoSet & export_queue = engine.get_export_queue();

    const PhotoListPtr photos(new PhotoList());
    std::copy(export_queue.begin(), export_queue.end(),
              std::inserter(*photos, photos->begin()));

    const ProgressObserverPtr observer
        = ProgressObserver<sigc::signal<void> >::create(
              application_->get_progress_dialog());

    if (true == create_archive)
    {
        observer->set_description(_("Creating archive"));
        observer->set_total(photos->size());

        GTimeVal time;
        g_get_current_time(&time);

        const std::string & username = Glib::get_user_name();
        std::ostringstream sout;
        sout << Glib::get_tmp_dir() << "/"
             << "Photos-" << username << "-" << time.tv_sec << ".zip";

        // FIXME: We need to ensure that this is unique.
        const std::string & archive_path = sout.str();

        const ArchiveMakerPtr archive_maker = ArchiveMaker::create();
        archive_maker->make_async(
            archive_path,
            photos,
            sigc::bind(
                sigc::mem_fun(*this,
                              &Exporter::on_archive_maker_ready),
                archive_maker,
                archive_path,
                photos),
            observer);
    }
    else
    {
        photoDestination_->export_photos_async(photos, observer);
    }
}

void
Exporter::on_action_photo_export() throw()
{
    photoDestination_->init(*application_);
}

void
Exporter::on_archive_maker_ready(const ArchiveMakerPtr &,
                                 const std::string & archive_path,
                                 const PhotoListPtr & photos) throw()
{
    photos->clear();

    PhotoFactory & photo_factory = PhotoFactory::instance();

    // This does not represent a photo, but we can get away with it.
    const PhotoPtr photo  = photo_factory.create_photo(
                                Glib::filename_to_uri(archive_path),
                                "application/zip");
    photos->push_back(photo);

    const ProgressObserverPtr observer
        = ProgressObserver<sigc::signal<void> >::create(
              application_->get_progress_dialog());

    photoDestination_->export_photos_async(photos, observer);
}

void
Exporter::on_photo_destination_init_end(bool status) throw()
{
    if (false == status)
    {
        return;
    }

    ExporterDialogPtr exporter_dialog(
        new ExporterDialog(photoDestination_->get_browser(),
        photoDestination_->get_options()));

    exporter_dialog->set_transient_for(
                         application_->get_main_window());

    exporter_dialog->signal_response().connect(
        sigc::bind(sigc::mem_fun(
                       *this,
                       &Exporter::on_exporter_dialog_response),
                   exporter_dialog));

    exporter_dialog->show_all();
}

void
Exporter::on_exporter_dialog_response(
              gint response_id,
              ExporterDialogPtr & exporter_dialog) throw()
{
    switch (response_id)
    {
        case Gtk::RESPONSE_APPLY:
        {
            photoDestination_->read_selection();

            const bool create_archive
                           = exporter_dialog->get_create_archive();

            photoDestination_->set_create_archive(create_archive);
            export_photos_async(create_archive);
            break;
        }

        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT:
        {
            break;
        }

        default:
        {
            break;
        }
    }

    photoDestination_->final(*application_);
    exporter_dialog.reset();
}

void
Exporter::on_renderer_changed(RendererRegistry & renderer_registry)
                              throw()
{
    const IRendererPtr & renderer = renderer_registry.get_current();
    renderer->receive_plugin(*this);
}

void
Exporter::ui_hide() throw()
{
    actionGroup_->set_visible(false);
}

void
Exporter::ui_show() throw()
{
    actionGroup_->set_visible(true);
}

} // namespace Solang
