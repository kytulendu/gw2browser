/** \file       PreviewGLCanvas.cpp
 *  \brief      Contains declaration of the preview GLCanvas control.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2015-2018 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef PREVIEWGLCANVAS_H_INCLUDED
#define PREVIEWGLCANVAS_H_INCLUDED

#include "Viewers/ModelViewer/Renderer.h"

#include "Readers/MapReader.h"
#include "Readers/ModelReader.h"

#include "FileReader.h"

namespace gw2b {
    class DatFile;
    class DatIndexEntry;

    class PreviewGLCanvas;
    class RenderTimer : public wxTimer {
        PreviewGLCanvas* canvas;
    public:
        RenderTimer( PreviewGLCanvas* canvas );
        void Notify( );
        void start( );
    }; // class RenderTimer

    /** Panel control used to preview files from the .dat. */
    class PreviewGLCanvas : public wxGLCanvas {
        // Internal status
        bool                        m_isViewingMap = false;             // Is we are viewing map?

        wxWindow*                   m_parent;

        FileReader*                 m_reader;
        wxGLContext*                m_glContext;
        Renderer*                   m_glRenderer;
        RenderTimer*                m_renderTimer;
        wxTimer*                    m_movementKeyTimer;

        int                         m_winHeight;
        wxPoint                     m_lastMousePos;

    public:
        /** Constructor. Creates the preview GLCanvas with the given parent.
        *  \param[in]  p_parent     Parent of the control.
        *  \param[in]  p_attrib     The wxGLAttributes used for setting display attributes
        *                           (not for rendering context attributes). */
        PreviewGLCanvas( wxWindow* p_parent, const wxGLAttributes& p_attrib );
        /** Destructor. */
        ~PreviewGLCanvas( );
        /** Tells this GLCanvas to preview a file.
        *  \param[in]  p_datFile    .dat file containing the file to preview.
        *  \param[in]  p_entry      Entry to preview.
        *  \return bool    true if successful, false if not. */
        bool previewFile( DatFile& p_datFile, const DatIndexEntry& p_entry );
        /** Clear the viewer. */
        void clear( );
        /** Initialize the GLCanvas. */
        bool initGL( );
        /** Used just to know if we must end the program now because OpenGL 3.3 is not available. */
        bool glCtxAvailable( ) { return m_glContext != NULL; }

    private:
        /** Gets the reader containing the data displayed by this viewer.
        *  \return FileReader*     Reader containing the data. */
        FileReader* reader( ) {
            return m_reader;
        }
        /** Gets the reader containing the data displayed by this viewer.
        *  \return FileReader*     Reader containing the data. */
        const FileReader* reader( ) const {
            return m_reader;
        }

        /** Gets the model reader containing the data displayed by this viewer.
        *  \return ModelReader*    Reader containing the data. */
        ModelReader* modelReader( ) {
            return reinterpret_cast<ModelReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
          /** Gets the model reader containing the data displayed by this viewer.
          *  \return ModelReader*    Reader containing the data. */
        const ModelReader* modelReader( ) const {
            return reinterpret_cast<const ModelReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
          /** Gets the map reader containing the data displayed by this viewer.
          *  \return MapReader*    Reader containing the data. */
        MapReader* mapReader( ) {
            return reinterpret_cast<MapReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast
          /** Gets the map reader containing the data displayed by this viewer.
          *  \return MapReader*    Reader containing the data. */
        const MapReader* mapReader( ) const {
            return reinterpret_cast<const MapReader*>( this->reader( ) );
        } // already asserted with a dynamic_cast

        void onPaintEvt( wxPaintEvent& p_event );
        void onMotionEvt( wxMouseEvent& p_event );
        void onMouseWheelEvt( wxMouseEvent& p_event );
        void onKeyDownEvt( wxKeyEvent& p_event );
        void onMovementKeyTimerEvt( wxTimerEvent& p_event );
        void onClose( wxCloseEvent& p_event );
        void onResize( wxSizeEvent& p_event );
        void onEraseBackground(wxEraseEvent & p_event);

    }; // class PreviewGLCanvas

}; // namespace gw2b

#endif // PREVIEWGLCANVAS_H_INCLUDED
