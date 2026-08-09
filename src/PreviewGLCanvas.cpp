/** \file       PreviewGLCanvas.cpp
 *  \brief      Contains definition of the preview GLCanvas control.
 *  \author     Khralkatorrix
 */

/**
 * Copyright (C) 2015-2019 Khralkatorrix <https://github.com/kytulendu>
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

#include "stdafx.h"

#include "DatFile.h"
#include "DatIndex.h"
#include "Exception.h"

#include "PreviewGLCanvas.h"

namespace gw2b {

    //----------------------------------------------------------------------------
    //      RenderTimer
    //----------------------------------------------------------------------------

    RenderTimer::RenderTimer( PreviewGLCanvas* canvas ) : wxTimer( ) {
        RenderTimer::canvas = canvas;
    }

    void RenderTimer::Notify( ) {
        canvas->Refresh( );
    }

    void RenderTimer::start( ) {
        wxTimer::Start( 6.94 );
    }

    //----------------------------------------------------------------------------
    //      PreviewGLCanvas
    //
    // We create a wxGLContext in this constructor and do OpenGL initialization
    // at onResize().
    //----------------------------------------------------------------------------

    PreviewGLCanvas::PreviewGLCanvas( wxWindow* p_parent, const wxGLAttributes& p_attrib )
        : wxGLCanvas( p_parent, p_attrib )
        , m_reader( nullptr )
        , m_lastMousePos( std::numeric_limits<int>::min( ), std::numeric_limits<int>::min( ) ) {

        m_parent = p_parent;

        m_glRenderer = nullptr;
        m_winHeight = 0;                // We have not been sized yet

        // Create OpenGL context

        wxGLContextAttrs ctxAttrs;
        ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
        m_glContext = new wxGLContext(this, nullptr, &ctxAttrs);
        if ( !m_glContext->IsOK() )
        {
            wxMessageBox("Gw2Browser needs OpenGL 3.3 support.",
                         "OpenGL version error", wxOK | wxICON_INFORMATION, this);
            delete m_glContext;
            m_glContext = nullptr;
        }
        else
        {
            wxLogMessage("OpenGL Core Profile 3.3 successfully set.");
        }

        m_renderTimer = new RenderTimer( this );
        m_movementKeyTimer = new wxTimer( this );

        // Hook up events
        this->Bind( wxEVT_PAINT, &PreviewGLCanvas::onPaintEvt, this );
        this->Bind( wxEVT_MOTION, &PreviewGLCanvas::onMotionEvt, this );
        this->Bind( wxEVT_MOUSEWHEEL, &PreviewGLCanvas::onMouseWheelEvt, this );
        this->Bind( wxEVT_KEY_DOWN, &PreviewGLCanvas::onKeyDownEvt, this );
        this->Bind( wxEVT_CLOSE_WINDOW, &PreviewGLCanvas::onClose, this );
        this->Bind( wxEVT_SIZE, &PreviewGLCanvas::onResize, this );
        this->Bind( wxEVT_ERASE_BACKGROUND, &PreviewGLCanvas::onEraseBackground, this );

        // Start render timer
        m_renderTimer->start( );
    }

    PreviewGLCanvas::~PreviewGLCanvas( ) {
        m_renderTimer->Stop( );

        if ( m_glRenderer )
        {
            delete m_glRenderer;
            m_glRenderer = nullptr;
        }

        if ( m_glContext )
        {
            delete m_glContext;
            m_glContext = nullptr;
        }

        this->clear( );

        delete m_renderTimer;
        delete m_movementKeyTimer;
    }

    bool PreviewGLCanvas::previewFile( DatFile& p_datFile, const DatIndexEntry& p_entry ) {
        this->clear( );

        auto entryData = p_datFile.readFile( p_entry.mftEntry( ) );
        if ( !entryData.GetSize( ) ) {
            return false;
        }

        // Create file reader
        m_reader = FileReader::readerForData( entryData, p_datFile, p_entry.fileType( ) );
        if ( m_reader ) {
            switch ( m_reader->dataType( ) ) {
            //case FileReader::DT_Map:
            case FileReader::DT_Model:
                break;
            default:
                return false;
            }

            if ( isOfType<MapReader>( m_reader ) ) {
                // we are viewing a map file
                m_isViewingMap = true;

                //auto reader = this->mapReader( );
                //auto test = reader->getMapData( );


            } if ( isOfType<ModelReader>( m_reader ) ) {
                // Load model
                auto reader = this->modelReader( );
                auto model = reader->getModel( );

                m_glRenderer->loadModel( p_datFile, model );
            }

            // Re-focus and re-render
            m_glRenderer->focus( );
            m_glRenderer->render( );

            return true;
        }

        return false;
    }

    void PreviewGLCanvas::clear( ) {
        if ( m_glRenderer ) {
            m_glRenderer->clear();
        }

        if ( m_reader ) {
            deletePointer( m_reader );
        }
    }

    bool PreviewGLCanvas::initGL( ) {
        if ( !m_glContext )
            return false;

        wxLogMessage( wxT( "Initializing OpenGL..." ) );

        m_glContext->SetCurrent(*this);

        // Initialize GLEW to setup the OpenGL Function pointers
        glewExperimental = true;
        GLenum glewerr = glewInit( );
        if ( GLEW_OK != glewerr )
        {
            wxLogMessage( wxT( "GLEW: Could not initialize GLEW library.\nError : %s" ), wxString( glewGetErrorString( glewerr ) ) );
            //return false;     // FIXME: Comment this out due to glewInit() give error 0x4 on some Linux system
        }

        wxLogMessage( wxT( "GLEW version %s" ), wxString( glewGetString( GLEW_VERSION ) ) );

        // Create OpenGL renderer, pass our OpenGL error handler
        //m_glRenderer = new Renderer(&fOGLErrHandler);
        m_glRenderer = new Renderer();

        // Get the GL version for the current OGL context
        wxString sglVer = "\nUsing OpenGL version: ";
        sglVer += wxString::FromUTF8( reinterpret_cast<const char*>(m_glRenderer->getGLVersion()) );
        // Also Vendor and Renderer
        sglVer += "\nVendor: ";
        sglVer += wxString::FromUTF8( reinterpret_cast<const char*>(m_glRenderer->getGLVendor()) );
        sglVer += "\nRenderer: ";
        sglVer += wxString::FromUTF8( reinterpret_cast<const char*>(m_glRenderer->getGLRenderer()) );

        wxLogMessage( sglVer );

        // For the menu "About" info
        //m_parent->setOGLString(sglVer);

        // Setup the renderer
        try {
            m_glRenderer->setup();
        } catch ( const exception::Exception& err ) {
            wxLogMessage( wxT( "%s" ), wxString( err.what( ) ) );
            return false;
        }

        return true;
    }

    void PreviewGLCanvas::onPaintEvt( wxPaintEvent& p_event ) {
        // This is a dummy, to avoid an endless succession of paint messages.
        // OnPaint handlers must always create a wxPaintDC.
        wxPaintDC dc( this );

        // Avoid painting when we have not yet a size
        if ( m_winHeight < 1 || !m_glRenderer )
            return;

        // This should not be needed, while we have only one canvas
        m_glContext->SetCurrent(*this);

        // Do the magic
        m_glRenderer->render();

        this->SwapBuffers();
    }

    void PreviewGLCanvas::onMotionEvt( wxMouseEvent& p_event ) {
        if ( m_lastMousePos.x == std::numeric_limits<int>::min( ) &&
            m_lastMousePos.y == std::numeric_limits<int>::min( ) ) {
            m_lastMousePos = p_event.GetPosition( );
        }

        glm::vec2 mousePos( ( p_event.GetX( ) - m_lastMousePos.x ), ( p_event.GetY( ) - m_lastMousePos.y ) );

        // Yaw/Pitch
        if ( p_event.LeftIsDown( ) ) {
            if ( m_glRenderer->getCameraMode() ) {
                // FPS Mode
                m_glRenderer->processCameraMouseMovement( mousePos.x, -mousePos.y );
            } else {
                // ORBITAL Mode
                m_glRenderer->processCameraMouseMovement( -mousePos.x, mousePos.y );
            }
            m_glRenderer->render( );
        }

        // Pan
        if ( p_event.RightIsDown( ) && !m_glRenderer->getCameraMode() ) {
            m_glRenderer->panCamera( mousePos.x, mousePos.y ); // Y mouse position is reversed since y-coordinates go from bottom to left
            m_glRenderer->render( );
        }

        m_lastMousePos = p_event.GetPosition( );

    }

    void PreviewGLCanvas::onMouseWheelEvt( wxMouseEvent& p_event ) {
        float zoomSteps = static_cast<float>( p_event.GetWheelRotation( ) ) / static_cast<float>( p_event.GetWheelDelta( ) );
        m_glRenderer->processCameraMouseScroll( -zoomSteps );
        m_glRenderer->render( );
    }

    void PreviewGLCanvas::onKeyDownEvt( wxKeyEvent& p_event ) {
        // Rendering control
        if ( p_event.GetKeyCode( ) == 'F' ) {
            m_glRenderer->focus( );
        } else if ( p_event.GetKeyCode( ) == '1' ) {
            m_glRenderer->toggleStatusText();
        } else if ( p_event.GetKeyCode( ) == '2' ) {
            m_glRenderer->toggleStatusWireframe();
        } else if ( p_event.GetKeyCode( ) == '3' ) {
            m_glRenderer->toggleStatusCullFace();
        } else if ( p_event.GetKeyCode( ) == '4' ) {
            m_glRenderer->toggleStatusTextured();
        } else if ( p_event.GetKeyCode( ) == '5' ) {
            m_glRenderer->toggleStatusLighting();
        } else if ( p_event.GetKeyCode( ) == '6' ) {
            m_glRenderer->toggleStatusNormalMapping();
        } else if ( p_event.GetKeyCode( ) == '7' ) {
            m_glRenderer->toggleStatusAntiAlising();
            // Force re-create framebuffer
            m_glRenderer->createFrameBuffer( );
        }

        // Rendering debugging/visualization control
        else if ( p_event.GetKeyCode( ) == 'N' ) {
            m_glRenderer->toggleStatusVisualizeNormal();
        } else if ( p_event.GetKeyCode( ) == 'M' ) {
            m_glRenderer->toggleStatusVisualizeZbuffer();
        } else if ( p_event.GetKeyCode( ) == 'L' ) {
            m_glRenderer->toggleStatusRenderLightSource();
        }

        // Camera control
        else if ( p_event.GetKeyCode( ) == 'C' ) {
            Camera::CameraMode mode;
            float sensivity = 0.25f;

            m_glRenderer->setCameraMode(!m_glRenderer->getCameraMode());

            if ( m_glRenderer->getCameraMode() ) {
                mode = Camera::CameraMode::FPSCAM;
                sensivity = 0.25f;

                // Scan for input every 3ms
                m_movementKeyTimer->Start( 3 );
                this->Bind( wxEVT_TIMER, &PreviewGLCanvas::onMovementKeyTimerEvt, this );
            } else {
                mode = Camera::CameraMode::ORBITALCAM;
                sensivity = 0.5f * ( glm::pi<float>( ) / 180.0f );  // 0.5 degrees per pixel

                m_movementKeyTimer->Stop( );
                this->Unbind( wxEVT_TIMER, &PreviewGLCanvas::onMovementKeyTimerEvt, this );
            }
            m_glRenderer->setCameraMode(mode);
            m_glRenderer->setCameraMouseSensitivity(sensivity);
        }

        // Reload shaders
        else if ( p_event.GetKeyCode( ) == '=' ) {
            wxLogMessage( wxT( "Reloading shader..." ) );
            m_glRenderer->reloadShader( );
            wxLogMessage( wxT( "Done." ) );
        }
    }

    void PreviewGLCanvas::onMovementKeyTimerEvt( wxTimerEvent&p_event ) {
        m_movementKeyTimer->Stop( );
        // Prevent modify camera value if the camera is in orbital mode
        if ( m_glRenderer->getCameraMode() ) {
            // First person camera control
            if ( wxGetKeyState( wxKeyCode( 'W' ) ) ) {
                m_glRenderer->processCameraKeyboard( Camera::CameraMovement::FORWARD );
            }
            if ( wxGetKeyState( wxKeyCode( 'S' ) ) ) {
                m_glRenderer->processCameraKeyboard( Camera::CameraMovement::BACKWARD );
            }
            if ( wxGetKeyState( wxKeyCode( 'A' ) ) ) {
                m_glRenderer->processCameraKeyboard( Camera::CameraMovement::LEFT );
            }
            if ( wxGetKeyState( wxKeyCode( 'D' ) ) ) {
                m_glRenderer->processCameraKeyboard( Camera::CameraMovement::RIGHT );
            }
        }
        m_movementKeyTimer->Start( );
    }

    void PreviewGLCanvas::onClose( wxCloseEvent& evt ) {
        m_renderTimer->Stop( );
        evt.Skip( );
    }

    //Note:
    // You may wonder why OpenGL initialization was not done at wxGLCanvas ctor.
    // The reason is due to GTK+/X11 working asynchronously, we can't call
    // SetCurrent() before the window is shown on screen (GTK+ doc's say that the
    // window must be realized first).
    // In wxGTK, window creation and sizing requires several size-events. At least
    // one of them happens after GTK+ has notified the realization. We use this
    // circumstance and do initialization then.

    void PreviewGLCanvas::onResize( wxSizeEvent& p_event ) {
        p_event.Skip( );

        // If this window is not fully initialized, dismiss this event
        if ( !this->IsShownOnScreen() )
            return;

        if ( !m_glRenderer ) {
            // Now we have a context, retrieve pointers to OGL functions
            if ( !this->initGL() )
                return;
            // Some GPUs need an additional forced paint event
            PostSizeEvent();
        }

        // This is normally only necessary if there is more than one wxGLCanvas
        // or more than one wxGLContext in the application.
        m_glContext->SetCurrent(*this);

        // It's up to the application code to update the OpenGL viewport settings.
        auto clientSize = p_event.GetSize();
        m_winHeight = clientSize.y;
        m_glRenderer->setViewport(0, 0, clientSize);

        // Generate paint event without erasing the background.
        this->Refresh(false);
    }

    void PreviewGLCanvas::onEraseBackground(wxEraseEvent & p_event)
    {
        // Empty function
    }

}; // namespace gw2b
