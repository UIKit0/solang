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
#include <functional>
#include <string>
#include <utility>

#include <giomm.h>

#include "application.h"

#ifdef HAVE_BRASERO
#include "brasero-destination.h"
#endif // HAVE_BRASERO

#include "browser-model-column-record.h"
#include "browser-renderer.h"
//#include "camera-source.h"
#include "content-type-repo.h"
//#include "date-manager.h"
#include "directory-destination.h"
//#include "directory-source.h"
#include "editor.h"
#include "enlarged-renderer.h"
#include "exporter.h"
//#include "flickr-source.h"
//#include "i-photo-source.h"
#include "i-plugin.h"
//#include "importer.h"
#include "photo.h"
#include "property-manager.h"
#include "search-manager.h"
#include "slideshow-renderer.h"
#include "tag-manager.h"
#include "thumbnailer.h"

namespace Solang
{

template<typename T>
class Initializer :
    public std::unary_function<T &, void>
{
    public:
        Initializer(Application * const & application) throw();

        Initializer(const Initializer<T> & source) throw();
    
        ~Initializer() throw();

        Initializer<T> &
        operator=(const Initializer<T> & source) throw();

        void
        operator()(T & x) throw();

    protected:
        Application * application_;
};

template<typename T>
Initializer<T>::Initializer(Application * const & application) throw() :
    std::unary_function<T &, void>(),
    application_(application)
{
}

template<typename T>
Initializer<T>::Initializer(const Initializer<T> & source) throw() :
    std::unary_function<T &, void>(source),
    application_(source.application_)
{
}

template<typename T>
Initializer<T>::~Initializer() throw()
{
}

template<typename T>
Initializer<T> &
Initializer<T>::operator=(const Initializer<T> & source) throw()
{
    if (this != &source)
    {
        std::unary_function<T &, void>::operator=(source);
        application_ = source.application_;
    }

    return *this;
}

template<typename T>
void
Initializer<T>::operator()(T & x) throw()
{
    x->init(*application_);
}

template<typename T>
class Finalizer :
    public std::unary_function<T &, void>
{
    public:
        Finalizer(Application * const & application) throw();

        Finalizer(const Finalizer<T> & source) throw();
    
        ~Finalizer() throw();

        Finalizer<T> &
        operator=(const Finalizer<T> & source) throw();

        void
        operator()(T & x) throw();

    protected:
        Application * application_;
};

template<typename T>
Finalizer<T>::Finalizer(Application * const & application) throw() :
    std::unary_function<T &, void>(),
    application_(application)
{
}

template<typename T>
Finalizer<T>::Finalizer(const Finalizer<T> & source) throw() :
    std::unary_function<T &, void>(source),
    application_(source.application_)
{
}

template<typename T>
Finalizer<T>::~Finalizer() throw()
{
}

template<typename T>
Finalizer<T> &
Finalizer<T>::operator=(const Finalizer<T> & source) throw()
{
    if (this != &source)
    {
        std::unary_function<T &, void>::operator=(source);
        application_ = source.application_;
    }

    return *this;
}

template<typename T>
void
Finalizer<T>::operator()(T & x) throw()
{
    x->final(*application_);
}

Application::Application(int & argc, char ** & argv) throw() :
    sigc::trackable(),
    threadPool_(4, false),
    iconFactory_(Gtk::IconFactory::create()),
    engine_(argc, argv),
    mainWindow_(),
    progressDialog_(mainWindow_),
    listStore_(Gtk::ListStore::create(BrowserModelColumnRecord())),
    listStoreIter_(),
    plugins_(),
    rendererRegistry_(),
    initEnd_(),
    listStoreChangeBegin_(),
    listStoreChangeEnd_()
{
    const std::string cache_dir_path = Glib::get_user_cache_dir()
                                       + "/" + Glib::get_prgname();

    if (false == Glib::file_test(cache_dir_path,
                                 Glib::FILE_TEST_EXISTS))
    {
        try
        {
            Gio::File::create_for_path(
                cache_dir_path)->make_directory_with_parents();
        }
        catch (const Gio::Error & e)
        {
            g_warning("%s", e.what().c_str());
        }
    }

    const std::string config_dir_path = Glib::get_user_config_dir()
                                        + "/" + Glib::get_prgname();

    if (false == Glib::file_test(config_dir_path,
                                 Glib::FILE_TEST_EXISTS))
    {
        try
        {
            Gio::File::create_for_path(
                config_dir_path)->make_directory_with_parents();
        }
        catch (const Gio::Error & e)
        {
            g_warning("%s", e.what().c_str());
        }
    }

    const std::string data_dir_path = Glib::get_user_data_dir()
                                        + "/" + Glib::get_prgname();

    if (false == Glib::file_test(data_dir_path,
                                 Glib::FILE_TEST_EXISTS))
    {
        try
        {
            Gio::File::create_for_path(
                data_dir_path)->make_directory_with_parents();
        }
        catch (const Gio::Error & e)
        {
            g_warning("%s", e.what().c_str());
        }
    }

    Gtk::Window::set_default_icon_name(PACKAGE_TARNAME);

    engine_.signal_criteria_changed().connect(
        sigc::mem_fun(*this,
                      &Application::on_criteria_changed));

    Thumbnailer & thumbnailer = Thumbnailer::instance();
    thumbnailer.signal_ready().connect(
        sigc::mem_fun(*this,
                      &Application::on_thumbnailer_ready));
}

Application::~Application() throw()
{
    threadPool_.shutdown(false);

    Thumbnailer & thumbnailer = Thumbnailer::instance();
    thumbnailer.shutdown();
}

void
Application::init() throw()
{
    engine_.init("");
    engine_.search_async(
        IPhotoSearchCriteriaList(),
        sigc::mem_fun(*this,
                      &Application::on_async_search));

    //we use the gconf backend for gsettings
    g_setenv("GSETTINGS_BACKEND", "gconf", FALSE);
    settings_ = g_settings_new("org.gnome.solang");

    // Plugins.

//    IPluginPtr date_manager(new DateManager());
//    plugins_.insert(std::make_pair("date-manager",
//                                   date_manager));

    IPluginPtr editor(new Editor());
    plugins_.push_back(editor);

    IPluginPtr property_manager(new PropertyManager());
    plugins_.push_back(property_manager);

    IPluginPtr search_manager(new SearchManager());
    plugins_.push_back(search_manager);

    IPluginPtr tag_manager(new TagManager( ));
    plugins_.push_back(tag_manager);

    IPhotoDestinationPtr directory_destination(
                             new DirectoryDestination());
    IPluginPtr directory_exporter(new Exporter(directory_destination,
                                               true));
    plugins_.push_back(directory_exporter);

#ifdef HAVE_BRASERO
    IPhotoDestinationPtr brasero_destination(
                             new BraseroDestination());
    IPluginPtr brasero_exporter(new Exporter(brasero_destination,
                                             false));
    plugins_.push_back(brasero_exporter);
#endif // HAVE_BRASERO

//    IPhotoSourcePtr directory_source(new DirectorySource());
//    IPluginPtr directory_importer(new Importer(directory_source, true));
//    plugins_.push_back(directory_importer);

//    IPhotoSourcePtr camera_source(new CameraSource());
//    IPluginPtr camera_importer(new Importer(camera_source, false));
//    plugins_.push_back(camera_importer);

//    IPhotoSourcePtr flickr_source(new FlickrSource());
//    IPluginPtr flickr_importer(new Importer(flickr_source, false));
//    plugins_.push_back(flickr_importer);

    // Renderers.

    IPluginPtr browser_renderer(new BrowserRenderer());
    plugins_.push_back(browser_renderer);

    IPluginPtr enlarged_renderer(new EnlargedRenderer());
    plugins_.push_back(enlarged_renderer);

    IPluginPtr slideshow_renderer(new SlideshowRenderer());
    plugins_.push_back(slideshow_renderer);

    std::for_each(plugins_.begin(), plugins_.end(),
                  Initializer<IPluginPtr>(this));

    const IRendererPtr renderer
        = rendererRegistry_.select<BrowserRenderer>();

    if (0 != renderer)
    {
        rendererRegistry_.set_current(renderer);
    }

    ContentTypeRepo::instance()->init();

    mainWindow_.init(*this);

    initEnd_.emit(*this);
}

void
Application::run() throw()
{
    mainWindow_.show_all();
    Gtk::Main::run(mainWindow_);
}

void
Application::final() throw()
{
    engine_.final();
    mainWindow_.final(*this);

    std::for_each(plugins_.rbegin(), plugins_.rend(),
                  Finalizer<IPluginPtr>(this));
    plugins_.clear();
}

void
Application::add_photo_to_model(const PhotoPtr & photo) throw()
{
    Gtk::TreeModel::iterator model_iter = listStore_->append();
    const Gtk::TreeModel::Path model_path
                                   = listStore_->get_path(model_iter);
    Gtk::TreeModel::Row row = *model_iter;

    BrowserModelColumnRecord model_column_record;

    row[model_column_record.get_column_serial()]
        = static_cast<guint>(model_path.front());
    row[model_column_record.get_column_photo()] = photo;
}

void
Application::add_photos_to_model(const PhotoList & photos) throw()
{
    listStoreChangeBegin_.emit(*this);

    PhotoList::const_iterator list_iter;

    listStore_->clear();

    // Force the iterator to evaluate to false, which is otherwise
    // not happening.
    listStoreIter_ = listStore_->children().end();

    for (list_iter = photos.begin(); list_iter != photos.end();
         list_iter++)
    {
        add_photo_to_model(*list_iter);

        if (false == listStoreIter_)
        {
            listStoreIter_ = listStore_->children().begin();
        }

        while (true == Gtk::Main::events_pending())
        {
            Gtk::Main::iteration();
        }
    }

    listStoreChangeEnd_.emit(*this);
}

sigc::signal<void, Application &> &
Application::init_end() throw()
{
    return initEnd_;
}

sigc::signal<void, Application &> &
Application::list_store_change_begin() throw()
{
    return listStoreChangeBegin_;
}

sigc::signal<void, Application &> &
Application::list_store_change_end() throw()
{
    return listStoreChangeEnd_;
}

void
Application::on_async_search(PhotoList & photos) throw()
{
    mainWindow_.set_busy(true);
    add_photos_to_model(photos);
    mainWindow_.set_busy(false);
}

void
Application::on_criteria_changed(IPhotoSearchCriteriaList & criteria)
                                 throw()
{
    engine_.search_async(
        criteria,
        sigc::mem_fun(*this,
                      &Application::on_async_search));
}

void
Application::on_thumbnailer_ready(PhotoList & photos) const throw()
{
    const Gtk::TreeModel::Children children = listStore_->children();

    // Hoping that keeping the bigger loop within the smaller loop
    // will result in better optimization. In this case,
    // children.size() is likely to be greater than photos.size().

    for (PhotoList::const_iterator photo_iter = photos.begin();
         photos.end() != photo_iter;
         photo_iter++)
    {
        for (Gtk::TreeModel::const_iterator model_iter
                                                = children.begin();
             children.end() != model_iter;
             model_iter++)
        {
            Gtk::TreeModel::Row row = *model_iter;
            BrowserModelColumnRecord model_column_record;
            const PhotoPtr photo
                = row[model_column_record.get_column_photo()];

            if (G_UNLIKELY(photo->get_uri()
                               == (*photo_iter)->get_uri()))
            {
                listStore_->row_changed(listStore_->get_path(
                                            model_iter),
                                        model_iter);
                break;
            }
        }

        while (true == Gtk::Main::events_pending())
        {
            Gtk::Main::iteration();
        }
    }
}

Glib::ThreadPool &
Application::get_thread_pool() throw()
{
    return threadPool_;
}

Engine &
Application::get_engine() throw()
{
    return engine_;
}

MainWindow &
Application::get_main_window() throw()
{
    return mainWindow_;
}

GSettings *
Application::get_settings() throw()
{
    return settings_;
}

ProgressDialog &
Application::get_progress_dialog() throw()
{
    return progressDialog_;
}

const ListStorePtr &
Application::get_list_store() throw()
{
    return listStore_;
}

Gtk::TreeModel::iterator &
Application::get_list_store_iter() throw()
{
    return listStoreIter_;
}

void
Application::set_list_store_iter(
                 const Gtk::TreeModel::iterator & iter) throw()
{
    listStoreIter_ = iter;
}

void
Application::set_list_store_iter(const Gtk::TreeModel::Path & path)
                                 throw()
{
    listStoreIter_ = listStore_->get_iter(path);
}

RendererRegistry &
Application::get_renderer_registry() throw()
{
    return rendererRegistry_;
}

DragDropCriteriaMap &
Application::get_drag_drop_map() throw()
{
    return dragItemMap_;
}

void
Application::set_drag_item(const Glib::ustring & key,
                const IPhotoSearchCriteriaPtr &criteria) throw()
{
    //Existing item will be replaced
    //This handles double dragging of same criteria
    dragItemMap_[ key ] = criteria;
    return;
}

} // namespace Solang
