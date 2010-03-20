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
#include <cstdlib>
#include <fstream>
#include <iterator>

#include <glibmm/i18n.h>
#include <libsoup/soup.h>
#include <libsoup/soup-method.h>

#include "application.h"
#include "flickr-auth-dialog.h"
#include "flickr-chooser-button.h"
#include "flickr-context.h"
#include "flickr-source.h"
#include "i-storage.h"
#include "photo.h"
#include "photo-source-enums.h"
#include "photo-tag.h"
#include "progress-observer.h"
#include "spinner-dialog.h"

namespace Solang
{

static const std::string apiKey = "1878ca674d609279800163d92bd56472";
static const std::string sharedSecret = "130b7362a2181162";

static const Glib::ustring authUri
    = "http://www.flickr.com/auth-72157619720626366";

static const std::string tokenFile = "flickr-token";

class FlickrInitializer
{
    public:
        FlickrInitializer(Application * const & application,
                          const FlickrContextPtr & flickr_context,
                          const Glib::ustring & name,
                          sigc::signal<void, bool> & init_end)
                          throw();

        ~FlickrInitializer() throw();

        void
        operator()() throw();

    protected:
        void
        attempt_authorization() throw();

        void
        on_get_token_end() throw();

        void
        on_test_login_end() throw();

        void
        spawn_auth_get_token() throw();

        void
        spawn_test_login() throw();

        void
        auth_get_token() throw();

        void
        test_login() throw();

        Application * application_;

        IPhotoSource * photoSource_;

        bool loginStatus_;

        bool tokenStatus_;

        FlickrContextPtr flickrContext_;

        const Glib::ustring name_;

        SpinnerDialog spinnerDialog_;

        std::string frob_;

        const std::string tokenFilePath_;

        Glib::Dispatcher getTokenEnd_;

        Glib::Dispatcher testLoginEnd_;

        sigc::signal<void, bool> & initEnd_;

    private:
};

FlickrInitializer::FlickrInitializer(
                       Application * const & application,
                       const FlickrContextPtr & flickr_context,
                       const Glib::ustring & name,
                       sigc::signal<void, bool> & init_end) throw() :
    application_(application),
    loginStatus_(false),
    tokenStatus_(false),
    flickrContext_(flickr_context),
    name_(name),
    spinnerDialog_(),
    frob_(),
    tokenFilePath_(Glib::get_user_data_dir()
                   + "/" + Glib::get_prgname()
                   + "/" + tokenFile),
    testLoginEnd_(),
    initEnd_(init_end)
{
    spinnerDialog_.set_transient_for(application_->get_main_window());

    getTokenEnd_.connect(
        sigc::mem_fun(*this,
                      &FlickrInitializer::on_get_token_end));

    testLoginEnd_.connect(
        sigc::mem_fun(*this,
                      &FlickrInitializer::on_test_login_end));
}

FlickrInitializer::~FlickrInitializer() throw()
{
}

void
FlickrInitializer::operator()() throw()
{
    if (true == Glib::file_test(tokenFilePath_,
                                Glib::FILE_TEST_EXISTS))
    {
        spawn_test_login();
    }
    else
    {
        attempt_authorization();
    }
}

void
FlickrInitializer::attempt_authorization() throw()
{
    Gtk::MessageDialog message_dialog(
        application_->get_main_window(),
        Glib::ustring::compose(
            _("Authorization needed to access %1"),
            name_),
        false, Gtk::MESSAGE_QUESTION,
        Gtk::BUTTONS_OK_CANCEL, false);

    message_dialog.set_default_response(Gtk::RESPONSE_OK);

    message_dialog.set_secondary_text(
        _("During authorization, you will be taken to your account "
          "and if you agree to it, you will be given a nine digit "
          "code. Please enter this nine digit code into the denoted "
          "input area."));

    message_dialog.set_title(_("Authorization needed"));

    gint response = message_dialog.run();
    message_dialog.hide();

    switch (response)
    {
        case Gtk::RESPONSE_CANCEL:
        {
            initEnd_.emit(false);
            return;
        }

        case Gtk::RESPONSE_OK:
        {
            break;
        }

        default:
        {
            initEnd_.emit(false);
            return;
        }
    }

    FlickrAuthDialog auth_dialog(authUri);
    auth_dialog.set_transient_for(application_->get_main_window());
    response = auth_dialog.run();

    switch (response)
    {
        case Gtk::RESPONSE_CANCEL:
        {
            initEnd_.emit(false);
            return;
        }

        case Gtk::RESPONSE_OK:
        {
            frob_ = auth_dialog.get_frob();
            spawn_auth_get_token();
            break;
        }

        default:
        {
            initEnd_.emit(false);
            return;
        }
    }
}

void
FlickrInitializer::on_get_token_end() throw()
{
    spinnerDialog_.set_busy(false);
    spinnerDialog_.hide();

    if (true == tokenStatus_)
    {
        spawn_test_login();
    }
    else
    {
        attempt_authorization();
    }
}

void
FlickrInitializer::on_test_login_end() throw()
{
    spinnerDialog_.set_busy(false);
    spinnerDialog_.hide();

    if (true == loginStatus_)
    {
        initEnd_.emit(loginStatus_);
    }
    else
    {
        attempt_authorization();
    }
}

void
FlickrInitializer::spawn_auth_get_token() throw()
{
    spinnerDialog_.set_title(_("Fetching token..."));
    spinnerDialog_.set_text(Glib::ustring::compose(
        _("<big><b>Fetching token for %1...</b></big>"), frob_));
    spinnerDialog_.show_all();
    spinnerDialog_.set_busy(true);

    Glib::ThreadPool & thread_pool = application_->get_thread_pool();

    thread_pool.push(
        sigc::mem_fun(*this,
                      &FlickrInitializer::auth_get_token));
}

void
FlickrInitializer::spawn_test_login() throw()
{
    spinnerDialog_.set_title(_("Logging in..."));
    spinnerDialog_.set_text(Glib::ustring::compose(
        _("<big><b>Logging into %1...</b></big>"),
        name_));
    spinnerDialog_.show_all();
    spinnerDialog_.set_busy(true);

    Glib::ThreadPool & thread_pool = application_->get_thread_pool();

    thread_pool.push(
        sigc::mem_fun(*this,
                      &FlickrInitializer::test_login));
}

void
FlickrInitializer::auth_get_token() throw()
{
    flickcurl * const fc = flickrContext_->get_flickr_session();

    char * status = flickcurl_auth_getFullToken(fc, frob_.c_str());

    if (true == (tokenStatus_ = (0 != status)))
    {
        const std::string token = status;
        free(status);
        status = 0;

        std::ofstream fout(tokenFilePath_.c_str());
        std::copy(token.begin(), token.end(),
                  std::ostreambuf_iterator<char>(fout));
        fout.close();
    }

    getTokenEnd_.emit();
}

void
FlickrInitializer::test_login() throw()
{
    std::ifstream fin(tokenFilePath_.c_str());
    const std::string token((std::istreambuf_iterator<char>(fin)),
                            std::istreambuf_iterator<char>());
    fin.close();

    flickcurl * const fc = flickrContext_->get_flickr_session();

    flickcurl_set_auth_token(fc, token.c_str());
    char * user_name = flickcurl_test_login(fc);

    if (true == (loginStatus_ = (0 != user_name)))
    {
        flickrContext_->set_user_name(user_name);
        free(user_name);
        user_name = 0;
    }
    else
    {
        flickcurl_set_auth_token(fc, "");
    }

    testLoginEnd_.emit();
}

FlickrSource::FlickrSource() throw() :
    PhotoSource(),
    iconFactory_(Gtk::IconFactory::create()),
    flickrChooserButton_(0),
    flickrContext_(),
    uris_(),
    initEnd_(),
    signalInitEnd_()
{
    Gtk::IconSource icon_source;
    Gtk::IconSet icon_set_flickr;

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "flickr.png");
    icon_source.set_size(Gtk::IconSize(24));
    icon_set_flickr.add_source(icon_source);

    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-flickr"),
                      icon_set_flickr);
    iconFactory_->add_default();
}

FlickrSource::~FlickrSource() throw()
{
    iconFactory_->remove_default();
}

void
FlickrSource::init(Application & application) throw()
{
    flickrContext_ = FlickrContextPtr(new FlickrContext());
    flickcurl * const fc = flickrContext_->get_flickr_session();

    flickcurl_set_api_key(fc, apiKey.c_str());
    flickcurl_set_shared_secret(fc, sharedSecret.c_str());

    FlickrInitializerPtr flickr_initializer(
        new FlickrInitializer(&application, flickrContext_,
                              get_name(), initEnd_));

    signalInitEnd_
        = initEnd_.connect(
              sigc::bind(
                  sigc::mem_fun(*this,
                                &FlickrSource::on_init_end),
                  flickr_initializer));

    (*flickr_initializer)();
}

void
FlickrSource::final(Application & application) throw()
{
    flickrContext_.reset();
}

PhotoPtr
FlickrSource::import(const PhotoPtr & photo,
                     const IStoragePtr & storage,
                     const TagList &tags, Database & db,
                     const ProgressObserverPtr & observer) throw()
{
    if (0 == photo)
    {
        return PhotoPtr();
    }

    storage->save(photo);

    TagList::const_iterator iter;

    for (iter = tags.begin(); tags.end() != iter; iter++)
    {
        PhotoTag pt(photo->get_photo_id(), (*iter)->get_tag_id());
        pt.save(db);
    }

    return PhotoPtr(photo);
}

PhotoList
FlickrSource::import(const PhotoList & photos,
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
    PhotoList::const_iterator iter;

    for (iter = photos.begin(); photos.end() != iter; iter++)
    {
        if (0 != observer)
        {
            if (true == observer->get_stop())
            {
                break;
            }
        }

        const PhotoPtr photo = import(*iter, storage, tags, db,
                                      observer);

        imported_photos.push_back(photo);

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
FlickrSource::import(const IStoragePtr & storage, const TagList &tags,
                     Database & db,
                     const ProgressObserverPtr & observer) throw()
{
    PhotoList files;
    download_photos(files, observer);
    return import(files, storage, tags, db, observer);
}

sigc::signal<void, bool> &
FlickrSource::init_end() throw()
{
    return initEnd_;
}

void
FlickrSource::download_photos(PhotoList & files,
                              const ProgressObserverPtr & observer)
                              throw()
{
    if (0 != observer)
    {
        observer->set_event_description(_("Downloading photos"));
        observer->set_num_events(uris_.size());
        observer->set_current_events(0);
    }

    const std::string cache_dir_path = Glib::get_user_cache_dir()
                                       + "/" + Glib::get_prgname();

    SoupSession * const soup_session
        = soup_session_sync_new_with_options(
              "user-agent",
              Glib::get_application_name().c_str(),
              NULL);

    std::list<std::string>::iterator iter;

    for (iter = uris_.begin(); uris_.end() != iter; iter++)
    {
        if (0 != observer)
        {
            if (true == observer->get_stop())
            {
                files.clear();
                break;
            }
        }

        // FIXME: Ubuntu has a thing against SOUP_METHOD_GET.
        SoupMessage * soup_message = soup_message_new(
                                         "GET",
                                         iter->c_str());

        const guint status = soup_session_send_message(soup_session,
                                                       soup_message);

        if (false == SOUP_STATUS_IS_SUCCESSFUL(status))
        {
            continue;
        }

        const PixbufLoaderPtr pixbuf_loader
                                  = Gdk::PixbufLoader::create();

        try
        {
            pixbuf_loader->write(
                reinterpret_cast<const guint8 *>(
                    soup_message->response_body->data),
                soup_message->response_body->length);
        }
        catch (const Glib::FileError & e)
        {
            g_warning("%s", e.what().c_str());
        }
        catch (const Gdk::PixbufError & e)
        {
            g_warning("%s", e.what().c_str());
        }

        try
        {
            pixbuf_loader->close();
        }
        catch (const Glib::FileError & e)
        {
            g_warning("%s", e.what().c_str());
        }
        catch (const Gdk::PixbufError & e)
        {
            g_warning("%s", e.what().c_str());
        }

        PixbufPtr pixbuf = pixbuf_loader->get_pixbuf();
        const std::string file_path
            = cache_dir_path + "/"
              + Gio::File::create_for_uri(*iter)->get_basename();

        pixbuf->save(file_path, "jpeg");

        PhotoPtr photo(new Photo());
        photo->set_disk_file_path(file_path);

        files.push_back(photo);
        observer->receive_event_notifiation();
    }

    if (0 != observer)
    {
        observer->reset();
    }

    soup_session_abort(soup_session);
}

void
FlickrSource::read_selection() throw()
{
    uris_ = flickrChooserButton_->get_uris();
}

Gtk::Widget &
FlickrSource::get_browser() throw()
{
    flickrChooserButton_ = new FlickrChooserButton(flickrContext_);
    return *Gtk::manage(flickrChooserButton_);
}

Glib::ustring
FlickrSource::get_label() const throw()
{
    return Glib::ustring("F_lickr...");
}

Glib::ustring
FlickrSource::get_name() const throw()
{
    return Glib::ustring("Flickr");
}

gint
FlickrSource::get_options() const throw()
{
    return PHOTO_SOURCE_COPY_PHOTOS_TRUE
           | PHOTO_SOURCE_INCLUDE_SUBFOLDERS_FALSE;
}

Gtk::StockID
FlickrSource::get_stock_id() const throw()
{
    return Gtk::StockID(PACKAGE_TARNAME"-flickr");
}

void
FlickrSource::on_init_end(bool status,
                          FlickrInitializerPtr & flickr_initializer)
                          throw()
{
    signalInitEnd_.disconnect();
    flickr_initializer.reset();
}

} // namespace Solang
