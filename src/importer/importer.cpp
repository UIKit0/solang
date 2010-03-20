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

#include <sigc++/sigc++.h>

#include "application.h"
#include "browser-renderer.h"
#include "directory-storage.h"
#include "engine.h"
#include "enlarged-renderer.h"
#include "i-photo-source.h"
#include "importer-dialog.h"
#include "importer.h"
#include "progress-observer.h"
#include "tag.h"

namespace Solang
{

static const std::string uiFile
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-importer.ui";
    
Importer::Importer(const IPhotoSourcePtr & photo_source, bool standard)
    throw() :
    Plugin(),
    application_(NULL),
    photoSource_(photo_source),
    standard_(standard),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2,%3",
                         __FILE__,
                         __LINE__,
                         photo_source->get_name()))),
    standardUIID_(0),
    uiID_(0),
    signalPhotoImportBegin_(),
    signalPhotoImportEnd_()
{
    if (true == standard_)
    {
        actionGroup_->add(
            Gtk::Action::create("ActionPhotoImportStandard",
                                Gtk::Stock::ADD, "Import"),
            sigc::mem_fun(*this, &Importer::on_action_photo_import));
    }

    actionGroup_->add(
        Gtk::Action::create("ActionPhotoMenuImportMenu",
                            Gtk::Stock::ADD, "Import From"));

    const Glib::ustring action_name
        = "ActionPhotoImport" + photoSource_->get_name();

    actionGroup_->add(
        Gtk::Action::create(action_name, photoSource_->get_stock_id(),
                            photoSource_->get_label()),
        sigc::mem_fun(*this, &Importer::on_action_photo_import));

    photoSource_->init_end().connect(
        sigc::mem_fun(*this,
                      &Importer::on_photo_source_init_end));
}

Importer::~Importer() throw()
{
}

void
Importer::init(Application & application) throw()
{
    application_ = &application;

    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    if (true == standard_)
    {
        Glib::ustring standard_ui_info
            = "<ui>"
              "  <toolbar name='ToolBar'>"
              "    <placeholder name='PlaceholderImportToolBar'>"
              "    <toolitem action='ActionPhotoImportStandard'/>"
              "    </placeholder>"
              "  </toolbar>"
              "</ui>";

        standardUIID_
            = ui_manager->add_ui_from_string(standard_ui_info);
        if (0 == standardUIID_)
        {
            // FIXME: error condition.
        }
    }

    Glib::ustring ui_info
        = "<ui>"
          "  <menubar name='MenuBar'>"
          "    <menu action='ActionPhotoMenu'>"
          "      <placeholder name='PlaceholderImportMenus'>"
          "      <menu action='ActionPhotoMenuImportMenu'>"
          "        <placeholder name='PlaceholderImportSubMenus'>"
          "        <menuitem action='ActionPhotoImport"
                       + photoSource_->get_name()
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

    Engine & engine = application.get_engine();

    signalPhotoImportBegin_
        = engine.photo_import_begin().connect(
              sigc::mem_fun(*this,
                            &Importer::on_photo_import_begin));

    signalPhotoImportEnd_
        = engine.photo_import_end().connect(
              sigc::mem_fun(*this,
                            &Importer::on_photo_import_end));

    initialized_.emit(*this);
}

void
Importer::final(Application & application) throw()
{
    signalPhotoImportEnd_.disconnect();
    signalPhotoImportBegin_.disconnect();

    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    ui_manager->remove_action_group(actionGroup_);
    ui_manager->remove_ui(uiID_);
    if (true == standard_)
    {
        ui_manager->remove_ui(standardUIID_);
    }

    finalized_.emit(*this);
}

void
Importer::visit_renderer(BrowserRenderer & browser_renderer) throw()
{
    ui_show();
}

void
Importer::visit_renderer(EnlargedRenderer & enlarged_renderer) throw()
{
    ui_show();
}

void
Importer::visit_renderer(SlideshowRenderer & slideshow_renderer)
                         throw()
{
}

void
Importer::on_action_photo_import() throw()
{
    photoSource_->init(*application_);
}

void
Importer::on_photo_import_begin() throw()
{
    actionGroup_->set_sensitive(false);
}

void
Importer::on_photo_import_end() throw()
{
    actionGroup_->set_sensitive(true);
}

void
Importer::on_photo_source_init_end(bool status) throw()
{
    if (false == status)
    {
        return;
    }

    Engine & engine = application_->get_engine();
    ImporterDialogPtr importer_dialog(
        new ImporterDialog(photoSource_->get_browser(),
        engine.get_tags(),
        photoSource_->get_options()));

    importer_dialog->set_transient_for(
                         application_->get_main_window());

    importer_dialog->signal_response().connect(
        sigc::bind(sigc::mem_fun(
                       *this,
                       &Importer::on_importer_dialog_response),
                   importer_dialog));

    importer_dialog->show_all();
}

void
Importer::on_importer_dialog_response(
              gint response_id,
              ImporterDialogPtr & importer_dialog) throw()
{
    switch (response_id)
    {
        case Gtk::RESPONSE_APPLY:
        {
            photoSource_->read_selection();

            TagList tags;
            importer_dialog->get_tag_view().get_selected_tags( tags );
            
            Engine & engine = application_->get_engine();
            const IStoragePtr & storage
                = engine.get_current_storage_system("file");

            bool doNotCopy = !importer_dialog->get_copy_photos();
            const std::tr1::shared_ptr<DirectoryStorage> dirStorage
                = std::tr1::dynamic_pointer_cast<DirectoryStorage>(
                                                            storage );
            if( dirStorage )
            {
                dirStorage->set_do_not_copy( doNotCopy );
            }


            Glib::ThreadPool & thread_pool
                = application_->get_thread_pool();

            thread_pool.push(
                sigc::bind(sigc::mem_fun(engine, &Engine::import),
                           photoSource_, storage, tags));
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

    photoSource_->final(*application_);
    importer_dialog.reset();
}

void
Importer::on_renderer_changed(RendererRegistry & renderer_registry)
                              throw()
{
    const IRendererPtr & renderer = renderer_registry.get_current();
    renderer->receive_plugin(*this);
}

void
Importer::ui_hide() throw()
{
    actionGroup_->set_visible(false);
}

void
Importer::ui_show() throw()
{
    actionGroup_->set_visible(true);
}

} // namespace Solang
