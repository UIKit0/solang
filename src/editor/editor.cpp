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

#include <geglmm.h>
#include <glibmm/i18n.h>

#include "application.h"
#include "editable-photo.h"
#include "editor.h"
#include "flip-horiz-operation.h"
#include "flip-vert-operation.h"
#include "i-renderer.h"
#include "non-copyable.h"
#include "progress-observer.h"
#include "rotate-clock-operation.h"
#include "rotate-counter-operation.h"

namespace Solang
{

class IOperationFactory :
    public NonCopyable
{
    public:
        virtual
        ~IOperationFactory() throw() = 0;

        virtual IOperation *
        create() throw() = 0;

    protected:
        IOperationFactory() throw();
};

IOperationFactory::IOperationFactory() throw() :
    NonCopyable()
{
}

IOperationFactory::~IOperationFactory() throw()
{
}

template <typename T>
class OperationFactory :
    public IOperationFactory
{
    public:
        OperationFactory() throw();

        virtual
        ~OperationFactory() throw();

        virtual T *
        create() throw();
};

template <typename T>
OperationFactory<T>::OperationFactory() throw() :
    IOperationFactory()
{
}

template <typename T>
OperationFactory<T>::~OperationFactory() throw()
{
}

template <typename T>
T *
OperationFactory<T>::create() throw()
{
    return new T();
}

const std::string Editor::uiFile_
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/ui/"
          PACKAGE_TARNAME"-editor.ui";

Editor::Editor() throw() :
    Plugin(),
    application_(),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiID_(0),
    editablePhotos_(),
    currentPhoto_(),
    signalRendererChanged_()
{
    actionGroup_->add(
        Gtk::Action::create(
            "ActionToolsMenu", _("T_ools")));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionToolsMenuTransformMenu", _("_Transform")));

    actionGroup_->add(
        Gtk::Action::create_with_icon_name(
            "ActionToolsFlipHorizontal",
            "object-flip-horizontal",
            _("Flip _Horizontal"),
            _("Mirror the photo horizontally")),
        Gtk::AccelKey(""),
        sigc::bind(
            sigc::mem_fun(*this, &Editor::on_action_edit_photo),
            IOperationFactoryPtr(
                new OperationFactory<FlipHorizOperation>)));

    actionGroup_->add(
        Gtk::Action::create_with_icon_name(
            "ActionToolsFlipVertical",
            "object-flip-vertical",
            _("Flip _Vertical"),
            _("Mirror the photo vertically")),
        Gtk::AccelKey(""),
        sigc::bind(
            sigc::mem_fun(*this, &Editor::on_action_edit_photo),
            IOperationFactoryPtr(
                new OperationFactory<FlipVertOperation>)));

    actionGroup_->add(
        Gtk::Action::create_with_icon_name(
            "ActionToolsRotateClockwise",
            "object-rotate-right",
            _("_Rotate Clockwise"),
            _("Rotate the photo 90 degrees to the right")),
        Gtk::AccelKey("<control>R"),
        sigc::bind(
            sigc::mem_fun(*this, &Editor::on_action_edit_photo),
            IOperationFactoryPtr(
                new OperationFactory<RotateClockOperation>)));

    actionGroup_->add(
        Gtk::Action::create_with_icon_name(
            "ActionToolsRotateCounterclockwise",
            "object-rotate-left",
            _("Rotate Counterc_lockwise"),
            _("Rotate the photo 90 degrees to the left")),
        Gtk::AccelKey("<control><shift>R"),
        sigc::bind(
            sigc::mem_fun(*this, &Editor::on_action_edit_photo),
            IOperationFactoryPtr(
                new OperationFactory<RotateCounterOperation>)));
}

Editor::~Editor() throw()
{
    //g_object_unref(dockItem_);
}

void
Editor::init(Application & application) throw()
{
    Gegl::init(0, 0);
    application_ = &application;

    Engine & engine = application.get_engine();
    signalSelectionChanged_
        = engine.selection_changed().connect(
              sigc::mem_fun(*this,
                            &Editor::on_selection_changed));

    RendererRegistry & renderer_registry
                           = application.get_renderer_registry();
    signalRendererChanged_
        = renderer_registry.changed().connect(
              sigc::mem_fun(*this,
                            &Editor::on_renderer_changed));

    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    uiID_ = ui_manager->add_ui_from_file(uiFile_);
    if (0 == uiID_)
    {
        // FIXME: error condition.
    }

    ui_manager->insert_action_group(actionGroup_);

    initialized_.emit(*this);
}

void
Editor::final(Application & application) throw()
{
    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    ui_manager->remove_action_group(actionGroup_);
    ui_manager->remove_ui(uiID_);

    signalRendererChanged_.disconnect();
    editablePhotos_.clear();
    Gegl::exit();

    finalized_.emit(*this);
}

void
Editor::visit_renderer(BrowserRenderer & browser_renderer) throw()
{
    ui_hide();
}

void
Editor::visit_renderer(EnlargedRenderer & editor_renderer) throw()
{
    ui_show();
}

void
Editor::visit_renderer(SlideshowRenderer & editor_renderer) throw()
{
    ui_hide();
}

void
Editor::apply_async(const IOperationPtr & operation) throw()
{
    const Glib::ustring current_uri = currentPhoto_->get_uri();
    EditablePhotoMap::const_iterator iter = editablePhotos_.find(
                                                current_uri);

    EditablePhotoPtr editable_photo;

    if (editablePhotos_.end() == iter)
    {
        editable_photo = EditablePhotoPtr(new EditablePhoto(
                                              currentPhoto_));
        editablePhotos_.insert(std::make_pair(current_uri,
                                              editable_photo));
    }
    else
    {
        editable_photo = iter->second;
    }

    const ProgressObserverPtr observer
        = ProgressObserver<Glib::Dispatcher>::create(
              application_->get_progress_dialog());

    editable_photo->apply_async(
        operation,
        sigc::bind(sigc::mem_fun(*this,
                                 &Editor::on_action_edit_photo_end),
                   editable_photo),
        observer);
}

void
Editor::on_action_edit_photo_end(
            const EditablePhotoPtr & editable_photo) throw()
{
    const PhotoPtr & photo = editable_photo->get_photo();
    if (currentPhoto_->get_uri() != photo->get_uri())
    {
        return;
    }

    RendererRegistry & renderer_registry
                           = application_->get_renderer_registry();
    const IRendererPtr & current_renderer
                             = renderer_registry.get_current();
    const IRendererPtr enlarged_renderer
        = renderer_registry.select<EnlargedRenderer>();

    if (current_renderer != enlarged_renderer
        || 0 == enlarged_renderer)
    {
        return;
    }

    enlarged_renderer->render(photo);
}

void
Editor::on_action_edit_photo(const IOperationFactoryPtr & factory)
                             throw()
{
    const IOperationPtr operation(factory->create());
    Gtk::Widget * const widget = operation->get_widget();

    if (0 != widget)
    {
        widget->show_all();
    }

    operation->signal_ready().connect(
        sigc::bind(
            sigc::mem_fun(*this,
                          &Editor::apply_async),
            operation));
}

void
Editor::on_renderer_changed(RendererRegistry & renderer_registry)
                            throw()
{
    const IRendererPtr & renderer = renderer_registry.get_current();
    renderer->receive_plugin(*this);
}

void
Editor::on_selection_changed() throw()
{
    RendererRegistry & renderer_registry
                           = application_->get_renderer_registry();
    const IRendererPtr renderer = renderer_registry.get_current();

    if (0 == renderer)
    {
        return;
    }

    PhotoList photos = renderer->get_current_selection();

    if (true == photos.empty())
    {
        return;
    }

    currentPhoto_ = *photos.begin();
    return;
}

void
Editor::ui_hide() throw()
{
    actionGroup_->set_visible(false);
}

void
Editor::ui_show() throw()
{
    actionGroup_->set_visible(true);
}

} // namespace Solang
