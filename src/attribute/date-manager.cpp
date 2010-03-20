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

#include <glibmm/i18n.h>

#include "application.h"
#include "browser-renderer.h"
#include "date-manager.h"
#include "engine.h"
#include "enlarged-renderer.h"
#include "i-renderer.h"
#include "main-window.h"

namespace Solang
{

static const std::string uiFile
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-property-manager.ui";

DateManager::DateManager() throw() :
    Plugin(),
    application_(),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiID_( 1 ),
    dockItemName_("date-dock-item"),
    dockItemTitle_(_("Picture Taken Date")),
    dockItemBehaviour_(GDL_DOCK_ITEM_BEH_NORMAL),
    dockItem_(NULL),
    vBox_(false, 6),
    scrolledWindow_(),
    dateView_(),
    signalRendererChanged_()
{
    dockItem_ = gdl_dock_item_new_with_stock(dockItemName_.c_str(),
                                             dockItemTitle_.c_str(),
                                             "gtk-properties",
                                             dockItemBehaviour_);
    gtk_container_add(GTK_CONTAINER(dockItem_),
                      GTK_WIDGET(vBox_.gobj()));

    scrolledWindow_.set_policy(Gtk::POLICY_AUTOMATIC,
                               Gtk::POLICY_AUTOMATIC);
    scrolledWindow_.set_shadow_type(Gtk::SHADOW_IN);

    scrolledWindow_.add(dateView_);

    vBox_.pack_start( scrolledWindow_, Gtk::PACK_EXPAND_WIDGET,0 );

}

DateManager::~DateManager() throw()
{
    //g_object_unref(dockItem_);
}

void
DateManager::init(Application & application) throw()
{
    application_ = &application;
    dateView_.set_application( &application );

    Engine & engine = application.get_engine();
    engine.photo_import_end().connect(
            sigc::mem_fun(*this,
                    &DateManager::on_photos_changed) );

    RendererRegistry & renderer_registry
                           = application.get_renderer_registry();

    signalRendererChanged_
        = renderer_registry.changed().connect(
              sigc::mem_fun(*this,
                            &DateManager::on_renderer_changed));

    MainWindow & main_window = application.get_main_window();
    main_window.add_dock_object_left_top(GDL_DOCK_OBJECT(dockItem_));
    dateView_.populate();

    initialized_.emit(*this);
}

void
DateManager::final(Application & application) throw()
{
    signalRendererChanged_.disconnect();

    finalized_.emit(*this);
}

void
DateManager::visit_renderer(BrowserRenderer & browser_renderer)
                            throw()
{
    ui_show();
}

void
DateManager::visit_renderer(EnlargedRenderer & enlarged_renderer)
                            throw()
{
    ui_show();
}

void
DateManager::visit_renderer(SlideshowRenderer & slideshow_renderer)
                            throw()
{
}

void
DateManager::on_photos_changed() throw()
{
    dateView_.populate( );

    return;
}

void
DateManager::on_renderer_changed(RendererRegistry & renderer_registry)
                                 throw()
{
    if (false == gdl_dock_object_is_bound(GDL_DOCK_OBJECT(dockItem_)))
    {
        return;
    }

    const IRendererPtr & renderer = renderer_registry.get_current();
    renderer->receive_plugin(*this);
}

void
DateManager::ui_hide() throw()
{
    GdlDockItem * const dock_item = GDL_DOCK_ITEM(dockItem_);

    if (true == GDL_DOCK_OBJECT_ATTACHED(dock_item))
    {
        gdl_dock_item_hide_item(GDL_DOCK_ITEM(dockItem_));
    }
}

void
DateManager::ui_show() throw()
{
    GdlDockItem * const dock_item = GDL_DOCK_ITEM(dockItem_);

    if (false == GDL_DOCK_OBJECT_ATTACHED(dock_item))
    {
        gdl_dock_item_show_item(GDL_DOCK_ITEM(dockItem_));
    }
}

} // namespace Solang
