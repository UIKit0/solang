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

#ifndef SOLANG_TYPES_H
#define SOLANG_TYPES_H

#include <map>
#include <vector>
#include <tr1/memory>

#include <gdl/gdl.h>
#include <glibmm.h>

namespace Gdk
{

class DragContext;
class GC;
class Pixbuf;
class PixbufLoader;
class Screen;
class Window;

} // namespace Gdk

namespace Gegl
{

class Buffer;
class Node;
class Processor;

} // namespace Gegl

namespace Gio
{

class AsyncResult;
class DataInputStream;
class File;
class FileInfo;
class FileInputStream;

} // namespace Gio

namespace Gtk
{

class Action;
class ActionGroup;
class IconFactory;
class IconTheme;
class ListStore;
class Menu;
class Settings;
class Style;
class ToggleAction;
class TreeModel;
class TreeModelFilter;
class TreePath;
class TreeSelection;
class TreeStore;
class UIManager;

} // namespace Gtk

namespace Solang
{

typedef std::tr1::shared_ptr<const char> ConstCharPtr;
typedef std::tr1::shared_ptr<char> CharPtr;

typedef std::tr1::shared_ptr<const char *> ConstCharPtrPtr;

typedef std::vector<std::string> StringList;
typedef std::vector<Glib::ustring> UStringList;

class Application;
typedef const Application * ConstApplicationPtr;
typedef Application * ApplicationPtr;

class ArchiveMaker;
typedef std::tr1::shared_ptr<const ArchiveMaker> ConstArchiveMakerPtr;
typedef std::tr1::shared_ptr<ArchiveMaker> ArchiveMakerPtr;

class ContentTypeRepo;
typedef ContentTypeRepo * ContentTypeRepoPtr;

class Cache;
typedef std::tr1::shared_ptr<const Cache> ConstCachePtr;
typedef std::tr1::shared_ptr<Cache> CachePtr;

class Database;
typedef const Database * ConstDatabasePtr;
typedef Database * DatabasePtr;

class DBObject;
typedef const DBObject * ConstDBObjectPtr;
typedef DBObject * DBObjectPtr;
typedef std::vector<DBObjectPtr> DBObjectList;

class DBTable;
typedef std::tr1::shared_ptr<const DBTable> ConstDBTablePtr;
typedef std::tr1::shared_ptr<DBTable> DBTablePtr;

class DeleteAction;
typedef std::tr1::shared_ptr<const DeleteAction> ConstDeleteActionPtr;
typedef std::tr1::shared_ptr<DeleteAction> DeleteActionPtr;
typedef std::vector<DeleteActionPtr> DeleteActionList;

class EditAction;
typedef std::tr1::shared_ptr<const EditAction> ConstEditActionPtr;
typedef std::tr1::shared_ptr<EditAction> EditActionPtr;
typedef std::vector<EditActionPtr> EditActionList;

class EditablePhoto;
typedef std::tr1::shared_ptr<const EditablePhoto>
                                    ConstEditablePhotoPtr;
typedef std::tr1::shared_ptr<EditablePhoto> EditablePhotoPtr;
typedef std::vector<EditablePhotoPtr> EditablePhotoList;
typedef std::map<Glib::ustring, EditablePhotoPtr> EditablePhotoMap;

class ExifDataKey;
typedef std::tr1::shared_ptr<ExifDataKey> ExifDataKeyPtr;
typedef std::vector<ExifDataKeyPtr> ExifDataKeyList;

class FlickrChooserDialog;
typedef std::tr1::shared_ptr<const FlickrChooserDialog>
    ConstFlickrChooserDialogPtr;
typedef std::tr1::shared_ptr<FlickrChooserDialog>
    FlickrChooserDialogPtr;

class FlickrContext;
typedef std::tr1::shared_ptr<const FlickrContext>
    ConstFlickrContextPtr;
typedef std::tr1::shared_ptr<FlickrContext> FlickrContextPtr;

class FlickrInitializer;
typedef std::tr1::shared_ptr<const FlickrInitializer>
    ConstFlickrInitializerPtr;
typedef std::tr1::shared_ptr<FlickrInitializer> FlickrInitializerPtr;

class IPhotoDestination;
typedef std::tr1::shared_ptr<const IPhotoDestination>
    ConstIPhotoDestinationPtr;
typedef std::tr1::shared_ptr<IPhotoDestination> IPhotoDestinationPtr;
typedef std::vector<IPhotoDestinationPtr> IPhotoDestinationList;

class ExifData;
typedef std::tr1::shared_ptr<ExifData> ExifDataPtr;

class ExporterDialog;
typedef std::tr1::shared_ptr<const ExporterDialog>
    ConstExporterDialogPtr;
typedef std::tr1::shared_ptr<ExporterDialog> ExporterDialogPtr;

class Images;
typedef std::tr1::shared_ptr<const Images> ConstImagesPtr;
typedef std::tr1::shared_ptr<Images> ImagesPtr;
typedef std::vector<ImagesPtr> ImagesList;

class ImporterDialog;
typedef std::tr1::shared_ptr<const ImporterDialog>
    ConstImporterDialogPtr;
typedef std::tr1::shared_ptr<ImporterDialog> ImporterDialogPtr;

class IOperation;
typedef std::tr1::shared_ptr<const IOperation> ConstIOperationPtr;
typedef std::tr1::shared_ptr<IOperation> IOperationPtr;
typedef std::vector<IOperationPtr> OperationList;

class IPhotoSource;
typedef std::tr1::shared_ptr<const IPhotoSource> ConstIPhotoSourcePtr;
typedef std::tr1::shared_ptr<IPhotoSource> IPhotoSourcePtr;
typedef std::vector<IPhotoSourcePtr> IPhotoSourceList;

class IPlugin;
typedef std::tr1::shared_ptr<const IPlugin> ConstIPluginPtr;
typedef std::tr1::shared_ptr<IPlugin> IPluginPtr;
typedef std::vector<IPluginPtr> IPluginList;

class IProgressObserver;
typedef Glib::RefPtr<const IProgressObserver>
    ConstProgressObserverPtr;
typedef Glib::RefPtr<IProgressObserver> ProgressObserverPtr;

class IRenderer;
typedef const IRenderer * ConstIRendererPtr;
typedef IRenderer * IRendererPtr;
typedef std::vector<IRendererPtr> IRendererList;

class DatePhotoInfo;
typedef std::vector<DatePhotoInfo> DatePhotoInfoList;

class ModificationDate;
typedef std::tr1::shared_ptr<const ModificationDate> ConstModificationDatePtr;
typedef std::tr1::shared_ptr<ModificationDate> ModificationDatePtr;

class Photo;
typedef std::tr1::shared_ptr<const Photo> ConstPhotoPtr;
typedef std::tr1::shared_ptr<Photo> PhotoPtr;
typedef Photo * UnrefPhotoPtr;
typedef std::vector<PhotoPtr> PhotoList;
typedef std::tr1::shared_ptr<PhotoList> PhotoListPtr;

class IPhotoSearchCriteria;
typedef std::tr1::shared_ptr<const IPhotoSearchCriteria>
    ConstIPhotoSearchCriteriaPtr;
typedef std::tr1::shared_ptr<IPhotoSearchCriteria>
    IPhotoSearchCriteriaPtr;
typedef std::vector<IPhotoSearchCriteriaPtr> IPhotoSearchCriteriaList;
typedef std::map<Glib::ustring, IPhotoSearchCriteriaPtr>
    DragDropCriteriaMap;

class PixbufMaker;
typedef std::tr1::shared_ptr<const PixbufMaker> ConstPixbufMakerPtr;
typedef std::tr1::shared_ptr<PixbufMaker> PixbufMakerPtr;

class ProgressDialog;
typedef Glib::RefPtr<const ProgressDialog> ConstProgressDialogPtr;
typedef Glib::RefPtr<ProgressDialog> ProgressDialogPtr;

typedef Glib::RefPtr<Glib::Dispatcher> DispatcherPtr;

class IStorage;
typedef std::tr1::shared_ptr<const IStorage> ConstIStoragePtr;
typedef std::tr1::shared_ptr<IStorage> IStoragePtr;
typedef std::vector<IStoragePtr> IStorageList;

class ScaleAction;
typedef Glib::RefPtr<const ScaleAction> ConstScaleActionPtr;
typedef Glib::RefPtr<ScaleAction> ScaleActionPtr;

class SlideshowWindow;
typedef std::tr1::shared_ptr<const SlideshowWindow>
    ConstSlideshowWindowPtr;
typedef std::tr1::shared_ptr<SlideshowWindow> SlideshowWindowPtr;

class Tag;
typedef std::tr1::shared_ptr<const Tag> ConstTagPtr;
typedef std::tr1::shared_ptr<Tag> TagPtr;
typedef std::vector<TagPtr> TagList;

class Thumbnail;
typedef std::tr1::shared_ptr<const Thumbnail> ConstThumbnailPtr;
typedef std::tr1::shared_ptr<Thumbnail> ThumbnailPtr;

typedef Glib::RefPtr<const Gdk::DragContext> ConstDragContextPtr;
typedef Glib::RefPtr<Gdk::DragContext> DragContextPtr;

typedef Glib::RefPtr<const Gdk::GC> ConstGCPtr;
typedef Glib::RefPtr<Gdk::GC> GCPtr;

typedef Glib::RefPtr<const Gdk::Pixbuf> ConstPixbufPtr;
typedef Glib::RefPtr<Gdk::Pixbuf> PixbufPtr;
typedef std::vector<PixbufPtr> PixbufList;

typedef Glib::RefPtr<const Gdk::PixbufLoader> ConstPixbufLoaderPtr;
typedef Glib::RefPtr<Gdk::PixbufLoader> PixbufLoaderPtr;

typedef Glib::RefPtr<const Gdk::Screen> ConstScreenPtr;
typedef Glib::RefPtr<Gdk::Screen> ScreenPtr;

typedef Glib::RefPtr<const Gdk::Window> ConstWindowPtr;
typedef Glib::RefPtr<Gdk::Window> WindowPtr;

typedef GdlDockObject * DockObjectPtr;
typedef const GdlDockObject * ConstDockObjectPtr;

typedef Glib::RefPtr<const Gegl::Buffer> ConstBufferPtr;
typedef Glib::RefPtr<Gegl::Buffer> BufferPtr;

typedef Glib::RefPtr<const Gegl::Node> ConstNodePtr;
typedef Glib::RefPtr<Gegl::Node> NodePtr;

typedef Glib::RefPtr<const Gegl::Processor> ConstProcessorPtr;
typedef Glib::RefPtr<Gegl::Processor> ProcessorPtr;

typedef Glib::RefPtr<const Gio::AsyncResult> ConstAsyncResultPtr;
typedef Glib::RefPtr<Gio::AsyncResult> AsyncResultPtr;

typedef Glib::RefPtr<const Gio::DataInputStream>
    ConstDataInputStreamPtr;
typedef Glib::RefPtr<Gio::DataInputStream> DataInputStreamPtr;

typedef Glib::RefPtr<const Gio::File> ConstFilePtr;
typedef Glib::RefPtr<Gio::File> FilePtr;

typedef Glib::RefPtr<const Gio::FileInfo> ConstFileInfoPtr;
typedef Glib::RefPtr<Gio::FileInfo> FileInfoPtr;

typedef Glib::RefPtr<const Gio::FileInputStream>
    ConstFileInputStreamPtr;
typedef Glib::RefPtr<Gio::FileInputStream> FileInputStreamPtr;

typedef Glib::RefPtr<const Gtk::Action> ConstActionPtr;
typedef Glib::RefPtr<Gtk::Action> ActionPtr;
typedef std::vector<ConstActionPtr> ConstActionList;
typedef std::vector<ActionPtr> ActionList;

typedef Glib::RefPtr<const Gtk::ActionGroup> ConstActionGroupPtr;
typedef Glib::RefPtr<Gtk::ActionGroup> ActionGroupPtr;

typedef Glib::RefPtr<const Gtk::IconFactory> ConstIconFactoryPtr;
typedef Glib::RefPtr<Gtk::IconFactory> IconFactoryPtr;

typedef Glib::RefPtr<const Gtk::IconTheme> ConstIconThemePtr;
typedef Glib::RefPtr<Gtk::IconTheme> IconThemePtr;

typedef Glib::RefPtr<const Gtk::ListStore> ConstListStorePtr;
typedef Glib::RefPtr<Gtk::ListStore> ListStorePtr;

typedef Glib::RefPtr<const Gtk::Menu> ConstMenuPtr;
typedef Glib::RefPtr<Gtk::Menu> MenuPtr;

class SearchCriterionSource;
typedef SearchCriterionSource * SearchCriterionSourcePtr;
typedef std::vector<SearchCriterionSourcePtr> SearchCriterionSourceList;

typedef Glib::RefPtr<const Gtk::Settings> ConstSettingsPtr;
typedef Glib::RefPtr<Gtk::Settings> SettingsPtr;

typedef Glib::RefPtr<const Gtk::Style> ConstStylePtr;
typedef Glib::RefPtr<Gtk::Style> StylePtr;

typedef Glib::RefPtr<const Gtk::ToggleAction> ConstToggleActionPtr;
typedef Glib::RefPtr<Gtk::ToggleAction> ToggleActionPtr;

typedef Glib::RefPtr<const Gtk::TreeStore> ConstTreeStorePtr;
typedef Glib::RefPtr<Gtk::TreeStore> TreeStorePtr;

typedef Glib::RefPtr<const Gtk::TreeModel> ConstTreeModelPtr;
typedef Glib::RefPtr<Gtk::TreeModel> TreeModelPtr;

typedef Glib::RefPtr<const Gtk::TreeModelFilter>
    ConstTreeModelFilterPtr;
typedef Glib::RefPtr<Gtk::TreeModelFilter> TreeModelFilterPtr;

typedef std::vector<Gtk::TreePath> TreePathList;

typedef Glib::RefPtr<const Gtk::TreeSelection> ConstTreeSelectionPtr;
typedef Glib::RefPtr<Gtk::TreeSelection> TreeSelectionPtr;

typedef Glib::RefPtr<const Gtk::UIManager> ConstUIManagerPtr;
typedef Glib::RefPtr<Gtk::UIManager> UIManagerPtr;

class EditEngine;
typedef EditEngine *EditEnginePtr;

} // namespace Solang

#endif // SOLANG_TYPES_H
