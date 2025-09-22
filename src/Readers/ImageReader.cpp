/** \file       Readers/ImageReader.cpp
 *  \brief      Contains the definition of the image reader class.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2014-2017 Khralkatorrix <https://github.com/kytulendu>
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

#include <wx/mstream.h>

#include <webp/decode.h> // libwebp
#include <gw2dattools/compression/inflateTextureFileBuffer.h>
#include <gw2dattools/exception/Exception.h>

#include "ImageReader.h"

#ifdef RGB
#undef RGB
#endif

namespace gw2b {

    union ImageReader::BGRA {
        struct {
            uint8 b;
            uint8 g;
            uint8 r;
            uint8 a;
        };
        uint8 parts[4];
        uint32 color;
    };

    union ImageReader::RGBA {
        struct {
            uint8 r;
            uint8 g;
            uint8 b;
            uint8 a;
        };
        uint8 parts[4];
        uint32 color;
    };

    struct ImageReader::BGR {
        uint8   b;
        uint8   g;
        uint8   r;
    };

    struct ImageReader::RGB {
        uint8   r;
        uint8   g;
        uint8   b;
    };

    union ImageReader::DXTColor {
        struct {
            uint16 red1 : 5;
            uint16 green1 : 6;
            uint16 blue1 : 5;
            uint16 red2 : 5;
            uint16 green2 : 6;
            uint16 blue2 : 5;
        };
        struct {
            uint16 color1;
            uint16 color2;
        };
    };

    struct ImageReader::DXT1Block {
        DXTColor colors;
        uint32   indices;
    };

    struct ImageReader::DXT3Block {
        uint64   alpha;
        DXTColor colors;
        uint32   indices;
    };

    struct ImageReader::DCXBlock     // Should be 3DCXBlock, but names can't start with a number D:
    {
        uint64  green;
        uint64  red;
    };

    struct ImageReader::DDSPixelFormat {
        uint32          size;                   /**< Structure size; set to 32 (bytes). */
        uint32          flags;                  /**< Values which indicate what type of data is in the surface. */
        uint32          fourCC;                 /**< Four-character codes for specifying compressed or custom formats. */
        uint32          rgbBitCount;            /**< Number of bits in an RGB (possibly including alpha) format. */
        uint32          rBitMask;               /**< Red (or lumiannce or Y) mask for reading color data. */
        uint32          gBitMask;               /**< Green (or U) mask for reading color data. */
        uint32          bBitMask;               /**< Blue (or V) mask for reading color data. */
        uint32          aBitMask;               /**< Alpha mask for reading alpha data. */
    };

    struct ImageReader::DDSHeader {
        uint32          magic;                  /**< Identifies a DDS file. This member must be set to 0x20534444. */
        uint32          size;                   /**< Size of structure. This member must be set to 124. */
        uint32          flags;                  /**< Flags to indicate which members contain valid data. */
        uint32          height;                 /**< Surface height (in pixels). */
        uint32          width;                  /**< Surface width (in pixels). */
        uint32          pitchOrLinearSize;      /**< The pitch or number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture. */
        uint32          depth;                  /**< Depth of a volume texture (in pixels), otherwise unused. */
        uint32          mipMapCount;            /**< Number of mipmap levels, otherwise unused. */
        uint32          reserved1[11];          /**< Unused. */
        DDSPixelFormat  pixelFormat;            /**< The pixel format. */
        uint32          caps;                   /**< Specifies the complexity of the surfaces stored. */
        uint32          caps2;                  /**< Additional detail about the surfaces stored. */
        uint32          caps3;                  /**< Unused. */
        uint32          caps4;                  /**< Unused. */
        uint32          reserved2;              /**< Unused. */
    };

    //----------------------------------------------------------------------------
    //      ImageReader
    //----------------------------------------------------------------------------

    ImageReader::ImageReader( const Array<byte>& p_data, DatFile& p_datFile, ANetFileType p_fileType )
        : FileReader( p_data, p_datFile, p_fileType ) {
    }

    ImageReader::~ImageReader( ) {
    }

    wxImage ImageReader::getImage( ) const {
        Assert( m_data.GetSize( ) >= 4 );
        Assert( isValidHeader( m_data.GetPointer( ), m_data.GetSize( ) ) );

        // Read the correct type of data
        auto fourcc = *reinterpret_cast<const uint32*>( m_data.GetPointer( ) );
        if ( ( ( fourcc & 0xffffff ) != FCC_JPEG ) && ( fourcc != FCC_PNG ) ) {

            wxSize size;
            BGR* colors = nullptr;
            uint8* alphas = nullptr;

            if ( ( fourcc == FCC_DDS ) ) {
                if ( !this->readDDS( size, colors, alphas ) ) {
                    return wxImage( );
                }
            } else if ( fourcc == FCC_RIFF ) {  // WebP
                if ( !this->readWebP( size, colors, alphas ) ) {
                    return wxImage( );
                }
            } else {
                if ( !this->readATEX( size, colors, alphas ) ) {
                    return wxImage( );
                }
            }

            // Create image and fill it with color data
            wxImage image( size.x, size.y );
            image.SetData( reinterpret_cast<unsigned char*>( colors ) );

            // Set alpha if the format has any
            if ( alphas ) {
                image.SetAlpha( alphas );
            }

            return image;

        } else {
            wxImage image;
            wxMemoryInputStream stream( m_data.GetPointer( ), m_data.GetSize( ) );
            if ( fourcc == FCC_PNG ) {
                image.LoadFile( stream, wxBITMAP_TYPE_PNG );
            } else {    // JPEGs
                image.LoadFile( stream, wxBITMAP_TYPE_JPEG );
            }
            return image;
        }
    }

    Array<byte> ImageReader::getDecompressedATEX( ) const {
        Assert( m_data.GetSize( ) >= 4 );
        Assert( isValidHeader( m_data.GetPointer( ), m_data.GetSize( ) ) );

        auto fourcc = *reinterpret_cast<const uint32*>( m_data.GetPointer( ) );
        if ( fourcc == FCC_ATEX ) {
            auto data = reinterpret_cast<const uint8_t*>( m_data.GetPointer( ) );
            auto atex = reinterpret_cast<const ANetAtexHeader*>( data );
            auto format = atex->formatInteger;

            uint32_t uncompressedSize = this->getUncompressedATEXSize( atex->width, atex->height, format );
            Array<byte> buffer( uncompressedSize );

            // Decompress
            try {
                gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), data, uncompressedSize, reinterpret_cast<uint8_t*>( buffer.GetPointer( ) ) );
            } catch ( const gw2dt::exception::Exception& err ) {
                wxLogMessage( wxT( "Failed decompress ATEX texture: %s" ), wxString( err.what( ) ) );
                return Array<byte>( );
            }

            return buffer;
        }

        return Array<byte>( );
    }

    bool ImageReader::readDDS( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const {
        // Get header
        auto header = reinterpret_cast<const DDSHeader*>( m_data.GetPointer( ) );

        // Ensure some of the values are correct
        if ( header->magic != FCC_DDS ||
            header->size != sizeof( DDSHeader ) -4 ||
            header->pixelFormat.size != sizeof( DDSPixelFormat ) ) {
            return false;
        }

        po_colors = nullptr;
        po_alphas = nullptr;

        // Determine the pixel format
        if ( header->pixelFormat.flags & 0x40 ) {               // 0x40 = DDPF_RGB, uncompressed data
            if ( !this->processUncompressedDDS( header, reinterpret_cast<RGB*&>( po_colors ), po_alphas ) ) {
                return false;
            }
        } else if ( header->pixelFormat.flags & 0x4 ) {         // 0x4 = DDPF_FOURCC, compressed
            const BGRA* data = reinterpret_cast<const BGRA*>( &m_data[sizeof( *header )] );
            switch ( header->pixelFormat.fourCC ) {
            case FCC_DXT1:
                this->processDXT1( data, header->width, header->height, po_colors, po_alphas );
                break;
            case FCC_DXT2:
            case FCC_DXT3:
                this->processDXT3( data, header->width, header->height, po_colors, po_alphas );
                break;
            case FCC_DXT4:
            case FCC_DXT5:
                this->processDXT5( data, header->width, header->height, po_colors, po_alphas );
                break;
            case FCC_R32F:
                break;
            }
        } else if ( header->pixelFormat.flags & 0x20000 ) {     // 0x20000 = DDPF_LUMINANCE, single-byte color
            if ( !this->processLuminanceDDS( header, reinterpret_cast<RGB*&>( po_colors ) ) ) {
                return false;
            }
        }

        if ( !!po_colors ) {
            po_size.Set( header->width, header->height );
        }

        return !!po_colors;
    }

    bool ImageReader::processLuminanceDDS( const DDSHeader* p_header, RGB*& po_colors ) const {
        // Ensure the image is 8-bit
        if ( p_header->pixelFormat.rgbBitCount != 8 ) {
            return false;
        }

        // Determine the size of the pixel data
        uint numPixels = p_header->width * p_header->height;
        uint dataSize = ( numPixels * p_header->pixelFormat.rgbBitCount ) >> 3;

        // Image data buffer too small?
        if ( m_data.GetSize( ) < ( sizeof( *p_header ) + dataSize ) ) {
            return false;
        }

        po_colors = allocate<RGB>( numPixels );

        // Read the data (we've already determined that the data is 8bpp above)
        auto pixelData = static_cast<const uint8*>( &m_data[sizeof( *p_header )] );

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( p_header->height ); y++ ) {
            uint32 curPixel = ( y * p_header->width );

            for ( uint x = 0; x < p_header->width; x++ ) {
                ::memset( &po_colors[curPixel], pixelData[curPixel], sizeof( po_colors[curPixel] ) );
                curPixel++;
            }
        }

        return true;
    }

    bool ImageReader::processUncompressedDDS( const DDSHeader* p_header, RGB*& po_colors, uint8*& po_alphas ) const {
        // Ensure the image is 32-bit. Until a non-32 bit texture is found,
        // there's no point adding support for it
        if ( p_header->pixelFormat.rgbBitCount != 32 ) {
            return false;
        }

        // Determine the size of the pixel data
        uint numPixels = p_header->width * p_header->height;
        uint dataSize = ( numPixels * p_header->pixelFormat.rgbBitCount ) >> 3;

        // Image data buffer too small?
        if ( m_data.GetSize( ) < ( sizeof( *p_header ) + dataSize ) ) {
            return false;
        }

        // Color data
        RGBA shift;
        po_colors = allocate<RGB>( numPixels );
        shift.r = lowestSetBit( p_header->pixelFormat.rBitMask );
        shift.g = lowestSetBit( p_header->pixelFormat.gBitMask );
        shift.b = lowestSetBit( p_header->pixelFormat.bBitMask );

        // Alpha data
        bool hasAlpha = ( p_header->pixelFormat.flags & 0x1 );    // 0x1 = DDPF_ALPHAPIXELS, alpha is present
        if ( hasAlpha ) {
            po_alphas = allocate<uint8>( numPixels );
            shift.a = lowestSetBit( p_header->pixelFormat.aBitMask );
        }

        // Read the data (we've already determined that the data is 32bpp)
        auto pixelData = reinterpret_cast<const uint32*>( &m_data[sizeof( *p_header )] );

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( p_header->height ); y++ ) {
            uint32 curPixel = ( y * p_header->width );

            for ( uint x = 0; x < p_header->width; x++ ) {
                po_colors[curPixel].r = ( pixelData[curPixel] & p_header->pixelFormat.rBitMask ) >> shift.r;
                po_colors[curPixel].g = ( pixelData[curPixel] & p_header->pixelFormat.gBitMask ) >> shift.g;
                po_colors[curPixel].b = ( pixelData[curPixel] & p_header->pixelFormat.bBitMask ) >> shift.b;

                if ( hasAlpha ) {
                    po_alphas[curPixel] = ( pixelData[curPixel] & p_header->pixelFormat.aBitMask ) >> shift.a;
                }

                curPixel++;
            }
        }

        return true;
    }

    size_t ImageReader::getUncompressedATEXSize( const uint16& p_width, const uint16& p_height, const uint32& p_format ) const {
        // Calculate uncompressed data size
        // from gw2formats\src\TextureFile.cpp
        uint32 numBlocks = ( ( p_width + 3 ) >> 2 ) * ( ( p_height + 3 ) >> 2 );
        switch ( p_format ) {
        case FCC_DXT1:
        case FCC_DXTA:
            return numBlocks * 8;
        case FCC_DXT2:
        case FCC_DXT3:
        case FCC_DXT4:
        case FCC_DXT5:
        case FCC_DXTL:
        case FCC_DXTN:
        case FCC_3DCX:
        case FCC_BC7X:
            return numBlocks * 16;
        default:
            wxLogMessage( wxT( "Unsupported ATEX texture format." ) );
            return false;
        }
    }

    bool ImageReader::readATEX( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const {
        // Determine mipmap0 size and bail if the file is too small
        if ( m_data.GetSize( ) >= sizeof( ANetAtexHeader ) + sizeof( uint32 ) ) {
            auto mipMap0Size = *reinterpret_cast<const uint32*>( &m_data[sizeof( ANetAtexHeader )] );

            if ( mipMap0Size + sizeof( ANetAtexHeader ) > m_data.GetSize( ) ) {
                po_size.Set( 0, 0 );
                return false;
            }
        } else {
            po_size.Set( 0, 0 );
            return false;
        }

        // Init some fields
        auto data = reinterpret_cast<const uint8_t*>( m_data.GetPointer( ) );
        auto atex = reinterpret_cast<const ANetAtexHeader*>( data );

        po_colors = nullptr;
        po_alphas = nullptr;

        uint16 width = atex->width;
        uint16 height = atex->height;

        // Hack for read 126x64 ATEX
        if ( width == 126 && height == 64 ) {
            width = 128;
        }

        uint32_t uncompressedSize = this->getUncompressedATEXSize( width, height, atex->formatInteger );

        // Allocate output
        auto buffer = allocate<BGRA>( width * height );

        // Decompress
        try {
            gw2dt::compression::inflateTextureFileBuffer( m_data.GetSize( ), data, uncompressedSize, reinterpret_cast<uint8_t*>( buffer ) );
        } catch ( const gw2dt::exception::Exception& err ) {
            wxLogMessage( wxT( "Failed decompress ATEX texture: %s" ), wxString( err.what( ) ) );
            freePointer( buffer );
            return false;
        }

        switch ( atex->formatInteger ) {
        case FCC_DXT1:
            this->processDXT1( buffer, width, height, po_colors, po_alphas );
            break;
        case FCC_DXT2:
        case FCC_DXT3:
        case FCC_DXTN:
            this->processDXT3( buffer, width, height, po_colors, po_alphas );
            break;
        case FCC_DXT4:
        case FCC_DXT5:
            this->processDXT5( buffer, width, height, po_colors, po_alphas );
            break;
        case FCC_DXTA:
            this->processDXTA( reinterpret_cast<uint64*>( buffer ), width, height, po_colors );
            break;
        case FCC_DXTL:
            this->processDXT5( buffer, width, height, po_colors, po_alphas );
            for ( uint i = 0; i < ( static_cast<uint>( width ) * static_cast<uint>( height ) ); i++ ) {
                po_colors[i].r = ( po_colors[i].r * po_alphas[i] ) / 0xff;
                po_colors[i].g = ( po_colors[i].g * po_alphas[i] ) / 0xff;
                po_colors[i].b = ( po_colors[i].b * po_alphas[i] ) / 0xff;
            }
            break;
        case FCC_3DCX:
            this->process3DCX( reinterpret_cast<RGBA*>( buffer ), width, height, po_colors, po_alphas );
            break;
        default:
            freePointer( buffer );
            return false;

        }

        freePointer( buffer );

        if ( po_colors ) {
            po_size.Set( width, height );
            return true;
        }

        return false;
    }

    bool ImageReader::readWebP( wxSize& po_size, BGR*& po_colors, uint8*& po_alphas ) const {
        // Init some fields
        auto data = reinterpret_cast<const uint8_t*>( m_data.GetPointer( ) );
        size_t data_size = m_data.GetSize( );
        po_colors = nullptr;
        po_alphas = nullptr;

        WebPDecoderConfig config;
        WebPBitstreamFeatures* bitstream = &config.input;

        VP8StatusCode status = VP8_STATUS_OK;

        status = WebPGetFeatures( reinterpret_cast<const uint8_t*>( data ), data_size, bitstream );

        if ( status != VP8_STATUS_OK ) {
            wxLogMessage( wxT( "This file isn't WebP!" ) );
            return false;
        }

        if ( bitstream->has_animation ) {
            wxLogMessage( wxT( "Not support Animation WebP." ) );
            return false;
        }

        uint numPixels = bitstream->width * bitstream->height;

        // Why it need to convert RGB to BGR?
        auto decoded_data = reinterpret_cast<BGRA*>( WebPDecodeRGBA( data, data_size, nullptr, nullptr ) );

        if ( decoded_data == nullptr ) {
            wxLogMessage( wxT( "Invalid WebP file format." ) );
            return false;
        }

        po_colors = allocate<BGR>( numPixels );
        po_alphas = allocate<uint8>( numPixels );

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( bitstream->height ); y++ ) {
            uint32 curPixel = ( y * bitstream->width );

            for ( uint x = 0; x < static_cast<uint>( bitstream->width ); x++ ) {
                ::memcpy( &po_colors[curPixel].b, &decoded_data[curPixel].b, sizeof( po_colors[curPixel].b ) );
                ::memcpy( &po_colors[curPixel].g, &decoded_data[curPixel].g, sizeof( po_colors[curPixel].g ) );
                ::memcpy( &po_colors[curPixel].r, &decoded_data[curPixel].r, sizeof( po_colors[curPixel].r ) );

                if ( bitstream->has_alpha ) {
                    ::memcpy( &po_alphas[curPixel], &decoded_data[curPixel].a, sizeof( po_alphas[curPixel] ) );
                }
                curPixel++;
            }
        }

        WebPFree( decoded_data );

        if ( po_colors ) {
            po_size.Set( bitstream->width, bitstream->height );
            return true;
        }

        return false;
    }

    bool ImageReader::isValidHeader( const byte* p_data, size_t p_size ) {
        if ( p_size < 0x10 ) {
            return false;
        }
        auto fourcc = *reinterpret_cast<const uint32*>( p_data );
        if ( fourcc == FCC_PNG ) {
            return true;
        }
        if ( ( fourcc & 0xffffff ) == FCC_JPEG ) {
            return true;
        }

        if ( fourcc == FCC_RIFF ) {
            // ensure it is WebP
            if ( p_size >= 12 ) {
                fourcc = *reinterpret_cast<const uint32*>( p_data + 8 );
            } else {
                return false;
            }

            if ( fourcc == FCC_WEBP ) {
                return true;
            } else {
                return false;
            }
        }

        // Is this a DDS file?
        if ( fourcc == FCC_DDS ) {
            if ( p_size < sizeof( DDSHeader ) ) {
                return false;
            }
            auto dds = reinterpret_cast<const DDSHeader*>( p_data );

            bool isUncompressedRgb = !!( dds->pixelFormat.flags & 0x40 );
            bool isCompressedRgb = !!( dds->pixelFormat.flags & 0x4 );
            bool isLuminanceTexture = !!( dds->pixelFormat.flags & 0x20000 );
            bool is8Bit = ( dds->pixelFormat.rgbBitCount == 8 );
            bool is32Bit = ( dds->pixelFormat.rgbBitCount == 32 );

            return ( isCompressedRgb || ( isLuminanceTexture && is8Bit ) || ( isUncompressedRgb && is32Bit ) );
        }

        if ( ( fourcc == FCC_ATEX ) || ( fourcc == FCC_ATTX ) || ( fourcc == FCC_ATEP ) ||
            ( fourcc == FCC_ATEU ) || ( fourcc == FCC_ATEC ) || ( fourcc == FCC_ATET ) ) {
            auto compression = *reinterpret_cast<const uint32*>( p_data + 4 );
            return ( compression == FCC_DXT1 ) ||
                ( compression == FCC_DXT2 ) ||
                ( compression == FCC_DXT3 ) ||
                ( compression == FCC_DXT4 ) ||
                ( compression == FCC_DXT5 ) ||
                ( compression == FCC_DXTN ) ||
                ( compression == FCC_DXTL ) ||
                ( compression == FCC_DXTA ) ||
                ( compression == FCC_3DCX ) ||
                ( compression == FCC_BC7X );
        }

        return false;
    }

    void ImageReader::processDXTColor( BGR* p_colors, uint8* p_alphas, const DXTColor& p_blockColor, bool p_isDXT1 ) const {
        // Color 0
        p_colors[0].r = ( p_blockColor.red1 << 3 ) | ( p_blockColor.red1 >> 2 );
        p_colors[0].g = ( p_blockColor.green1 << 2 ) | ( p_blockColor.green1 >> 4 );
        p_colors[0].b = ( p_blockColor.blue1 << 3 ) | ( p_blockColor.blue1 >> 2 );
        // Color 1
        p_colors[1].r = ( p_blockColor.red2 << 3 ) | ( p_blockColor.red2 >> 2 );
        p_colors[1].g = ( p_blockColor.green2 << 2 ) | ( p_blockColor.green2 >> 4 );
        p_colors[1].b = ( p_blockColor.blue2 << 3 ) | ( p_blockColor.blue2 >> 2 );
        // Colors 2 and 3
        if ( !p_isDXT1 || p_blockColor.color1 > p_blockColor.color2 ) {
            p_colors[2].r = ( p_colors[0].r * 2 + p_colors[1].r ) / 3;
            p_colors[2].g = ( p_colors[0].g * 2 + p_colors[1].g ) / 3;
            p_colors[2].b = ( p_colors[0].b * 2 + p_colors[1].b ) / 3;
            p_colors[3].r = ( p_colors[0].r + p_colors[1].r * 2 ) / 3;
            p_colors[3].g = ( p_colors[0].g + p_colors[1].g * 2 ) / 3;
            p_colors[3].b = ( p_colors[0].b + p_colors[1].b * 2 ) / 3;
            // Alpha is only being set for DXT1
            if ( p_alphas ) {
                ::memset( p_alphas, 0xff, 4 );
            }
        } else {
            p_colors[2].r = ( p_colors[0].r + p_colors[1].r ) >> 1;
            p_colors[2].g = ( p_colors[0].g + p_colors[1].g ) >> 1;
            p_colors[2].b = ( p_colors[0].b + p_colors[1].b ) >> 1;
            ::memset( &p_colors[3], 0, sizeof( p_colors[3] ) );
            // Alpha is only being set for DXT1
            if ( p_alphas ) {
                ::memset( p_alphas, 0xff, 3 );
                p_alphas[3] = 0x0;
            }
        }
    }

    void ImageReader::processDXT1( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
        uint numPixels = ( p_width * p_height );

        const DXT1Block* blocks = reinterpret_cast<const DXT1Block*>( p_data );

        po_colors = allocate<BGR>( numPixels );
        po_alphas = allocate<uint8>( numPixels );

        const uint numHorizBlocks = p_width >> 2;
        const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
            for ( uint x = 0; x < numHorizBlocks; x++ ) {
                const DXT1Block& block = blocks[( y * numHorizBlocks ) + x];
                this->processDXT1Block( po_colors, po_alphas, block, x * 4, y * 4, p_width );
            }
        }
    }

    void ImageReader::processDXT1Block( BGR* p_colors, uint8* p_alphas, const DXT1Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
        uint32 indices = p_block.indices;
        BGR colors[4];
        uint8 alphas[4];

        this->processDXTColor( colors, alphas, p_block.colors, true );

        for ( uint y = 0; y < 4; y++ ) {
            uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

            for ( uint x = 0; x < 4; x++ ) {
                BGR& color = p_colors[curPixel];
                uint8& alpha = p_alphas[curPixel];

                uint index = ( indices & 3 );
                ::memcpy( &color, &colors[index], sizeof( color ) );
                alpha = alphas[index];

                curPixel++;
                indices >>= 2;
            }
        }
    }

    void ImageReader::processDXTA( const uint64* p_data, uint p_width, uint p_height, BGR*& po_colors ) const {
        uint numPixels = ( p_width * p_height );

        po_colors = allocate<BGR>( numPixels );

        const uint numHorizBlocks = p_width >> 2;
        const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
            for ( uint x = 0; x < numHorizBlocks; x++ ) {
                uint64 block = p_data[( y * numHorizBlocks ) + x];
                this->processDXTABlock( po_colors, block, x * 4, y * 4, p_width );
            }
        }
    }

    void ImageReader::processDXTABlock( BGR* p_colors, uint64 p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
        uint8  alphas[8];

        // Alpha 1 and 2
        alphas[0] = ( p_block & 0xff );
        alphas[1] = ( p_block & 0xff00 ) >> 8;
        p_block >>= 16;
        // Alpha 3 to 8
        if ( alphas[0] > alphas[1] ) {
            for ( uint i = 2; i < 8; i++ ) {
                alphas[i] = ( ( 8 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 7;
            }
        } else {
            for ( uint i = 2; i < 6; i++ ) {
                alphas[i] = ( ( 6 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 5;
            }
            alphas[6] = 0x00;
            alphas[7] = 0xff;
        }

        for ( uint y = 0; y < 4; y++ ) {
            uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

            for ( uint x = 0; x < 4; x++ ) {
                ::memset( &p_colors[curPixel], alphas[p_block & 0x7], sizeof( p_colors[curPixel] ) );
                curPixel++;
                p_block >>= 3;
            }
        }
    }

    void ImageReader::processDXT3( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
        uint numPixels = ( p_width * p_height );
        const DXT3Block* blocks = reinterpret_cast<const DXT3Block*>( p_data );

        po_colors = allocate<BGR>( numPixels );
        po_alphas = allocate<uint8>( numPixels );

        const uint numHorizBlocks = p_width >> 2;
        const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
            for ( uint x = 0; x < numHorizBlocks; x++ ) {
                const DXT3Block& block = blocks[( y * numHorizBlocks ) + x];
                this->processDXT3Block( po_colors, po_alphas, block, x * 4, y * 4, p_width );
            }
        }
    }

    void ImageReader::processDXT3Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
        uint32 indices = p_block.indices;
        uint64 blockAlpha = p_block.alpha;
        BGR colors[4];

        this->processDXTColor( colors, nullptr, p_block.colors, false );

        for ( uint y = 0; y < 4; y++ ) {
            uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

            for ( uint x = 0; x < 4; x++ ) {
                BGR& color = p_colors[curPixel];
                uint8& alpha = p_alphas[curPixel];

                uint index = ( indices & 3 );
                ::memcpy( &color, &colors[index], sizeof( color ) );
                alpha = ( ( blockAlpha & 0xf ) << 4 ) | ( blockAlpha & 0xf );

                curPixel++;
                indices >>= 2;
                blockAlpha >>= 4;
            }
        }
    }

    void ImageReader::processDXT5( const BGRA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
        uint numPixels = ( p_width * p_height );
        const DXT3Block* blocks = reinterpret_cast<const DXT3Block*>( p_data );

        po_colors = allocate<BGR>( numPixels );
        po_alphas = allocate<uint8>( numPixels );

        const uint numHorizBlocks = p_width >> 2;
        const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
            for ( uint x = 0; x < numHorizBlocks; x++ ) {
                const DXT3Block& block = blocks[( y * numHorizBlocks ) + x];
                this->processDXT5Block( po_colors, po_alphas, block, x * 4, y * 4, p_width );
            }
        }
    }

    void ImageReader::processDXT5Block( BGR* p_colors, uint8* p_alphas, const DXT3Block& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
        uint32 indices = p_block.indices;
        uint64 blockAlpha = p_block.alpha;
        BGR    colors[4];
        uint8  alphas[8];

        this->processDXTColor( colors, nullptr, p_block.colors, false );

        // Alpha 1 and 2
        alphas[0] = ( blockAlpha & 0xff );
        alphas[1] = ( blockAlpha & 0xff00 ) >> 8;
        blockAlpha >>= 16;
        // Alpha 3 to 8
        if ( alphas[0] > alphas[1] ) {
            for ( uint i = 2; i < 8; i++ ) {
                alphas[i] = ( ( 8 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 7;
            }
        } else {
            for ( uint i = 2; i < 6; i++ ) {
                alphas[i] = ( ( 6 - i ) * alphas[0] + ( i - 1 ) * alphas[1] ) / 5;
            }
            alphas[6] = 0x00;
            alphas[7] = 0xff;
        }

        for ( uint y = 0; y < 4; y++ ) {
            uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

            for ( uint x = 0; x < 4; x++ ) {
                BGR& color = p_colors[curPixel];
                uint8& alpha = p_alphas[curPixel];

                uint index = ( indices & 3 );
                ::memcpy( &color, &colors[index], sizeof( color ) );
                alpha = alphas[blockAlpha & 7];

                curPixel++;
                indices >>= 2;
                blockAlpha >>= 3;
            }
        }
    }

    void ImageReader::process3DCX( const RGBA* p_data, uint p_width, uint p_height, BGR*& po_colors, uint8*& po_alphas ) const {
        uint numPixels = ( p_width * p_height );
        const DCXBlock* blocks = reinterpret_cast<const DCXBlock*>( p_data );

        po_colors = allocate<BGR>( numPixels );
        po_alphas = nullptr; // 3DCX does not use alpha

        const uint numHorizBlocks = p_width >> 2;
        const uint numVertBlocks = p_height >> 2;

#pragma omp parallel for
        for ( int y = 0; y < static_cast<int>( numVertBlocks ); y++ ) {
            for ( uint x = 0; x < numHorizBlocks; x++ ) {
                const DCXBlock& block = blocks[( y * numHorizBlocks ) + x];
                // 3DCX actually uses RGB and not BGR, so *pretend* that's what the output is
                this->process3DCXBlock( reinterpret_cast<RGB*>( po_colors ), block, x * 4, y * 4, p_width );
            }
        }
    }

    void ImageReader::process3DCXBlock( RGB* p_colors, const DCXBlock& p_block, uint p_blockX, uint p_blockY, uint p_width ) const {
        const float floatToByte = 127.5f;
        const float byteToFloat = ( 1.0f / floatToByte );

        uint64 red = p_block.red;
        uint64 green = p_block.green;
        uint8 reds[8];
        uint8 greens[8];

        // Reds 1 and 2
        reds[0] = ( red & 0xff );
        reds[1] = ( red & 0xff00 ) >> 8;
        red >>= 16;
        // Reds 3 to 8
        if ( reds[0] > reds[1] ) {
            for ( uint i = 2; i < 8; i++ ) {
                reds[i] = ( ( 8 - i ) * reds[0] + ( i - 1 ) * reds[1] ) / 7;
            }
        } else {
            for ( uint i = 2; i < 6; i++ ) {
                reds[i] = ( ( 6 - i ) * reds[0] + ( i - 1 ) * reds[1] ) / 5;
            }
            reds[6] = 0x00;
            reds[7] = 0xff;
        }
        // Greens 1 and 2
        greens[0] = ( green & 0xff );
        greens[1] = ( green & 0xff00 ) >> 8;
        green >>= 16;
        //Greens 3 to 8
        if ( greens[0] > greens[1] ) {
            for ( uint i = 2; i < 8; i++ ) {
                greens[i] = ( ( 8 - i ) * greens[0] + ( i - 1 ) * greens[1] ) / 7;
            }
        } else {
            for ( uint i = 2; i < 6; i++ ) {
                greens[i] = ( ( 6 - i ) * greens[0] + ( i - 1 ) * greens[1] ) / 5;
            }
            greens[6] = 0x00;
            greens[7] = 0xff;
        }

        struct {
            float r; float g; float b;
        } normal;
        for ( uint y = 0; y < 4; y++ ) {
            uint curPixel = ( p_blockY + y ) * p_width + p_blockX;

            for ( uint x = 0; x < 4; x++ ) {
                RGB& color = p_colors[curPixel];

                // Get normal
                normal.r = ( ( float ) reds[red & 7] * byteToFloat ) - 1.0f;
                normal.g = ( ( float ) greens[green & 7] * byteToFloat ) - 1.0f;

                // Compute blue, based on red/green
                normal.b = ::sqrt( 1.0f - normal.r * normal.r - normal.g * normal.g );

                // Store normal
                color.r = ( ( normal.r + 1.0f ) * floatToByte );
                color.g = ( ( normal.g + 1.0f ) * floatToByte );
                color.b = ( ( normal.b + 1.0f ) * floatToByte );

                // Invert green as that seems to be the more common format
                color.g = 0xff - color.g;

                curPixel++;
                red >>= 3;
                green >>= 3;
            }
        }
    }

}; // namespace gw2b
