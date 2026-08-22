/** \file       CategoryTree.h
 *  \brief      Contains declaration of the .dat structure tree control.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2014-2019 Khralkatorrix <https://github.com/kytulendu>
 * Copyright (C) 2013 Till034 <https://github.com/Till034>
 * Copyright (C) 2012 Rhoot <https://github.com/rhoot>
 *
 * This file is part of Gw2Browser.
 *
 * Gw2Browser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef CATEGORYTREE_H_INCLUDED
#define CATEGORYTREE_H_INCLUDED

#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <set>
#include <unordered_map>

#include "DatIndex.h"

namespace gw2b {
    class CategoryTree;

    /** Represents the data associated with an item in the tree control. */
    class CategoryTreeItem : public wxTreeItemData {
    public:
        /** Type of data this entry represents. */
        enum DataType {
            DT_Category,    /**< The entry is an index category. */
            DT_Entry,       /**< The entry is an index entry. */
        };
    private:
        const void* m_data;
        DataType    m_dataType;
        bool        m_isDirty;
    public:
        /** Constructor. Sets the properties of this object to the given values.
        *  \param[in]  p_type   type of entry.
        *  \param[in]  p_data   pointer to the data represented by this entry. */
        CategoryTreeItem( DataType p_type, const void* p_data ) : m_data( p_data ), m_dataType( p_type ), m_isDirty( false ) {
        }
        /** Destructor. */
        virtual ~CategoryTreeItem( ) {
        }
        /** Gets the type of data represented by this entry.
        *  \return DataType    type of data represented by the entry. */
        DataType dataType( ) const {
            return m_dataType;
        }
        /** Gets the data represented by this entry.
        *  \return void*   pointer to the represented data. */
        const void* data( ) const {
            return m_data;
        }
        /** Marks this object as dirty. Only used for categories that are collapsed
        *  but have had new entries added to it. Entries that are yet to be created.
        *  \param[in]  pDirty  new dirty flag. */
        void setDirty( bool p_dirty ) {
            m_isDirty = p_dirty;
        }
        /** Determines whether this object is dirty or not.
        *  \return bool    true if object is dirty, false if not. */
        bool isDirty( ) const {
            return m_isDirty;
        }
    }; // class CategoryTreeItem

    /** Image list used to show icons in the category tree. */
    class CategoryTreeImageList : public wxImageList {
    public:
        /** The various types of icons contained within this image list. */
        enum IconType {
            IT_OpenFolder,      /**< Open folder icon. */
            IT_UnknownFile,     /**< Unknown file icon. */
            IT_ClosedFolder,    /**< Closed folder icon. */
            IT_Executable,      /**< Executable file icon. */
            IT_Dll,             /**< DLL file icon. */
            IT_Image,           /**< Image file icon. */
            IT_Text,            /**< Text file icon. */
            IT_Sound,           /**< Sound file icon. */
            IT_Font,            /**< Font file icon. */
            IT_BitmapFont,      /**< Bitmap font file icon. */
            IT_Video,           /**< Video file icon. */
            IT_Model,           /**< Model file icon. */
        };
    public:
        /** Constructor. Loads the images needed by this image list. */
        CategoryTreeImageList( );
        /** Destructor. */
        virtual ~CategoryTreeImageList( );
    }; // class CategoryTreeImageList

    /** \interface  ICategoryTreeListener
    *  Receives events from the category tree. */
    class ICategoryTreeListener {
    public:
        /** Raised when the user wants to extract files.
        *  \param[in]  p_tree   category tree invoking the callback.
        *  \param[in]  p_mode   if false extract raw file, if true extract converted file. */
        virtual void onTreeExtractFile( CategoryTree& p_tree, bool p_mode ) {
        }
        /** Raised whenever a non-category entry is clicked in the category tree.
        *  \param[in]  p_tree   category tree invoking the callback.
        *  \param[in]  p_entry  reference to the clicked entry. */
        virtual void onTreeEntryClicked( CategoryTree& p_tree, const DatIndexEntry& p_entry ) {
        }
        /** Raised whenever a category entry is clicked in the category tree.
        *  \param[in]  p_tree       category tree invoking the callback.
        *  \param[in]  p_category   reference to the clicked category. */
        virtual void onTreeCategoryClicked( CategoryTree& p_tree, const DatIndexCategory& p_category ) {
        }
        /** Raised when the category tree is being cleared.
        *  \param[in]  p_tree   category tree invoking the callback. */
        virtual void onTreeCleared( CategoryTree& p_tree ) {
        }
        /** Raised when the category tree is being destroyed.
        *  \param[in]  p_tree   category tree invoking the callback. */
        virtual void onTreeDestruction( CategoryTree& p_tree ) {
        }
    };

    /** Tree control containing categories and entries of a DatIndex. */
    class CategoryTree : public wxTreeCtrl, public IDatIndexListener {
        wxDECLARE_DYNAMIC_CLASS( CategoryTree );
        typedef std::set<ICategoryTreeListener*>    ListenerSet;
        typedef std::unordered_map<const DatIndexCategory*, wxTreeItemId>   CategoryNodeMap;
    private:
        std::shared_ptr<DatIndex>   m_index;
        ListenerSet                 m_listeners;
        /** Maps each index category to its node in the tree. Invalidated by
        *  clearEntries( ) and forgetSubCategories( ). */
        CategoryNodeMap             m_categoryNodes;
    public:
        /** Constructor. Creates the tree control with the given parent.
        *  \param[in]  p_parent     Parent of the control.
        *  \param[in]  p_location   Optional location of the control.
        *  \param[in]  p_size       Optional size of the control. */
        CategoryTree( wxWindow* p_parent, const wxPoint& p_location = wxDefaultPosition, const wxSize& p_size = wxDefaultSize );
        /** Constructor */
        CategoryTree();
        /** Destructor. */
        virtual ~CategoryTree( );

        /** Adds an index entry to this tree.
        *  \param[in]  p_entry  Entry to add. */
        void addEntry( const DatIndexEntry& p_entry );
        /** Ensures that the given category is part of the tree. Note that the
        *  category is \e not added if a parent category is collapsed. If it
        *  already exists, its id is returned and no category is added.
        *  \param[in]  p_category   Category to add to the tree.
        *  \param[in]  p_force      Forcefully add the category, even if the parent
        *              is collapsed. */
        wxTreeItemId ensureHasCategory( const DatIndexCategory& p_category, bool p_force = false );
        /** Clears all entries from the tree. */
        void clearEntries( );
        /** Gets the currently selected objects.
        *  \return Array<DatIndexEntry*>  array of entries. */
        Array<const DatIndexEntry*> getSelectedEntries( ) const;
        /** Find entry id of given entry name.
        *  \param[in]  p_root       Root entry.
        *  \param[in]  p_string     Entry name to search. */
        wxTreeItemId findEntry( wxTreeItemId p_root, const wxString& p_string );

        /** Gets the .dat file index represented by this tree. */
        std::shared_ptr<DatIndex> datIndex( ) const;
        /** Sets the .dat file index represented by this tree.
        *  \param[in]  p_index  Index this tree should represent. */
        void setDatIndex( const std::shared_ptr<DatIndex>& p_index );

        /** Adds an event listener to this tree.
        *  \param  pListener   Pointer to the listener to add. */
        void addListener( ICategoryTreeListener* p_listener );
        /** Removes an event listener from this tree.
        *  \param  p_listener   Pointer to the listener to remove. */
        void removeListener( ICategoryTreeListener* p_listener );

        /** Called by the .dat index when an entry is added.
        *  \param[in]  p_index  Reference to the index that had a file added to it.
        *  \param[in]  p_entry  Reference to the newly added entry. */
        virtual void onIndexFileAdded( DatIndex& p_index, const DatIndexEntry& p_entry ) override;
        /** Called by the .dat index when it is cleared.
        *  \param[in]  p_index  Reference to the index being cleared. */
        virtual void onIndexCleared( DatIndex& p_index ) override;
        /** Called by the .dat index when it is destroyed.
        *  \param[in]  p_index  Reference to the index being destroyed. */
        virtual void onIndexDestruction( DatIndex& p_index ) override;
    protected:
        /** Compare 2 items and return -1, 0 or +1 if the first item is less than,
        equal to or greater than the second one.
        *  \param[in]  item1, item2 Items to compare. */
        virtual int OnCompareItems( const wxTreeItemId& p_item1, const wxTreeItemId& p_item2 );
    private:
        void addCategoryEntriesToArray( Array<const DatIndexEntry*>& p_array, uint& p_index, const DatIndexCategory& p_category ) const;

        /** Drops the cached tree nodes of every category below the given one. Must be
        *  called before deleting a node's children.
        *  \param[in]  p_category   Category whose descendants should be forgotten. */
        void forgetSubCategories( const DatIndexCategory& p_category );

        /** Helper method to add an entry to the tree at the right spot, for sorting.
        *  \param[in]  p_parent     Category to add the entry to.
        *  \param[in]  p_entry      Entry to add. */
        wxTreeItemId addEntry( const wxTreeItemId& p_parent, const DatIndexEntry& p_entry );
        /** Helper method to add an entry to the tree at the right spot, for sorting.
        *  \param[in]  p_parent     Category to add the entry to.
        *  \param[in]  p_entry      Entry to add.
        *  \param[in]  p_name       Name of the entry, as an integer. */
        wxTreeItemId addNumberEntry( const wxTreeItemId& p_parent, const DatIndexEntry& p_entry, uint p_name );
        /** Helper method to add an entry to the tree at the right spot, for sorting.
        *  \param[in]  p_parent     Category to add the entry to.
        *  \param[in]  p_entry      Entry to add. */
        wxTreeItemId addTextEntry( const wxTreeItemId& p_parent, const DatIndexEntry& p_entry );
        /** Helper method to add a category to the tree at the right spot, for sorting.
        *  \param[in]  p_parent         Parent category to add the category to.
        *  \param[in]  p_displayName    Name of the category to add. */
        wxTreeItemId addCategoryEntry( const wxTreeItemId& p_parent, const wxString& p_displayName );

        /** Gets the index for the image that should represent the given entry.
        *  \param[in]  p_entry  Entry in need of an icon. */
        int getImageForEntry( const DatIndexEntry& p_entry );
        /** Event raised when an item is being expanded.
        *  \param[in]  p_event  Event object handed to us by wxWidgets. */
        void onItemExpanding( wxTreeEvent& p_event );
        /** Event raised when an item is being collapsed.
        *  \param[in]  p_event  Event object handed to us by wxWidgets. */
        void onItemCollapsing( wxTreeEvent& p_event );
        /** Event raised when an item is being selected.
        *  \param[in]  p_event  Event object handed to us by wxWidgets. */
        void onSelChanged( wxTreeEvent& p_event );
        /** Event raised when an item has been right clicked on.
        *  \param[in]  p_event  Event object handed to us by wxWidgets. */
        void onContextMenu( wxTreeEvent& p_event );
        /** Event raised when the user wants to extract raw files.
        *  \param[in]  p_event  Event object handed to us by wxWidgets. */
        void onExtractRawFiles( wxCommandEvent& p_event );
        /** Event raised when the user wants to extract converted files.
        *  \param[in]  p_event  Event object handed to us by wxWidgets. */
        void onExtractConvertedFiles( wxCommandEvent& p_event );
    }; // class CategoryTree

}; // namespace gw2b

#endif // CATEGORYTREE_H_INCLUDED
