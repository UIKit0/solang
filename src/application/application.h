/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2010 Florent Thévenet <feuloren@free.fr>
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

#ifndef SOLANG_APPLICATION_H
#define SOLANG_APPLICATION_H

#include <map>

#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "engine.h"
#include "main-window.h"
#include "progress-dialog.h"
#include "renderer-registry.h"
#include "types.h"

namespace Solang
{

class IPlugin;

class Application :
    public sigc::trackable
{
    public:
        Application(int & argc, char ** & argv) throw();

        ~Application() throw();

        void
        init() throw();

        void
        run() throw();

        void
        final() throw();

        sigc::signal<void, Application &> &
        init_end() throw();

        sigc::signal<void, Application &> &
        list_store_change_begin() throw();

        sigc::signal<void, Application &> &
        list_store_change_end() throw();

        Glib::ThreadPool &
        get_thread_pool() throw();

        Engine &
        get_engine() throw();

        MainWindow &
        get_main_window() throw();
        
        GSettings *
        get_settings() throw();

        ProgressDialog &
        get_progress_dialog() throw();

        const ListStorePtr &
        get_list_store() throw();

        Gtk::TreeModel::iterator &
        get_list_store_iter() throw();

        void
        set_list_store_iter(const Gtk::TreeModel::iterator & iter)
                            throw();

        void
        set_list_store_iter(const Gtk::TreeModel::Path & path) throw();

        RendererRegistry &
        get_renderer_registry() throw();

        DragDropCriteriaMap &
        get_drag_drop_map() throw();

        void
        set_drag_item(const Glib::ustring &, //key
                      const IPhotoSearchCriteriaPtr &) throw();

    protected:

    private:
        void
        add_photo_to_model(const PhotoPtr & photo) throw();

        void
        add_photos_to_model(const PhotoList & photos) throw();

        void
        on_async_search(PhotoList & photos) throw();

        void
        on_criteria_changed(IPhotoSearchCriteriaList & criteria)
                            throw();

        void
        on_thumbnailer_ready(PhotoList & photos) const throw();

        Glib::ThreadPool threadPool_;

        IconFactoryPtr iconFactory_;

        Engine engine_;

        MainWindow mainWindow_;

        ProgressDialog progressDialog_;

        ListStorePtr listStore_;

        Gtk::TreeModel::iterator listStoreIter_;

        IPluginList plugins_;

        RendererRegistry rendererRegistry_;

        DragDropCriteriaMap dragItemMap_;
        
        GSettings * settings_;

        sigc::signal<void, Application &> initEnd_;

        sigc::signal<void, Application &> listStoreChangeBegin_;

        sigc::signal<void, Application &> listStoreChangeEnd_;
};

} // namespace Solang

#endif // SOLANG_APPLICATION_H
