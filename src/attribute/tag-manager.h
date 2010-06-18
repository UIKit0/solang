/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_TAG_MANAGER_H
#define SOLANG_TAG_MANAGER_H

#include <string>
#include <stdexcept>

#include <gdl/gdl.h>
#include <glibmm.h>
#include <gtkmm.h>

#include "plugin.h"
#include "tag-view.h"
#include "tag-key-manager.h"

namespace Solang
{

class Application;
class RendererRegistry;

class TagManager :
    public Plugin
{
    public:
        TagManager() throw();

        virtual
        ~TagManager() throw();

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

        virtual TagPtr
        get_tag_for_urn(std::string) throw(std::runtime_error);

        virtual inline TagKeyManager*
        get_key_manager();

        virtual inline Application*
        get_application();

    protected:
        void
        on_action_tag_new() throw();

        void
        on_action_tag_edit() throw();

        void
        on_action_tag_delete() throw();

        void
        on_action_apply_tag() throw();

        void
        apply_tag(TagPtr tag, PhotoList *photos);

        void
        on_action_remove_tag() throw();

        void
        on_action_show_all_tags() throw();

        void
        on_get_tags(TagList & tags) throw();

        void
        on_renderer_changed(RendererRegistry & renderer_registry)
                            throw();

        void
        on_updated_tag() throw();

        void
        populate_view() throw();

        void
        ui_hide() throw();

        void
        ui_show() throw();

        void
        on_enlarged_renderer_key_press(PhotoList *photos, Glib::ustring key);

        ApplicationPtr application_;

        IconFactoryPtr iconFactory_;

        ActionGroupPtr actionGroup_;

        Gtk::UIManager::ui_merge_id uiID_;

        const std::string dockItemName_;

        const Glib::ustring dockItemTitle_;

        GdlDockItemBehavior dockItemBehaviour_;

        GtkWidget * dockItem_;

        Gtk::VBox vBox_;

        Gtk::ScrolledWindow scrolledWindow_;

        TagView tagView_;

        bool showAll_;

        TagKeyManager keyManager_;

        sigc::connection signalRendererChanged_;

    private:
};

inline TagKeyManager*
TagManager::get_key_manager()
{
    return &keyManager_;
}

inline Application*
TagManager::get_application()
{
    return application_;
}

} // namespace Solang

#endif // SOLANG_TAG_MANAGER_H
