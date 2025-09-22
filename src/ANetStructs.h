/** \file       ANetStructs.h
 *  \brief      Mainly contains structs used by ArenaNet in their files.
 *  \author     Rhoot
 */

/**
 * Copyright (C) 2014-2019 Khralkatorrix <https://github.com/kytulendu>
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

#ifndef ANETSTRUCTS_H_INCLUDED
#define ANETSTRUCTS_H_INCLUDED

#include "stdafx.h"

namespace gw2b {

    /** Contains language code use by string files. */
    namespace language {
        enum Type {
            English = 0,
            Korean = 1,
            French = 2,
            German = 3,
            Spanish = 4,
            Chinese = 5,
        };
    };

    /** Contains the various four-character code for identify files in the dat. */
    enum FourCC {
        // Offset 0
        FCC_ATEX = 0x58455441,
        FCC_ATTX = 0x58545441,
        FCC_ATEC = 0x43455441,
        FCC_ATEP = 0x50455441,
        FCC_ATEU = 0x55455441,
        FCC_ATET = 0x54455441,
        FCC_3DCX = 0x58434433,
        FCC_DXT = 0x00545844,
        FCC_DDS = 0x20534444,
        FCC_strs = 0x73727473,
        FCC_asnd = 0x646e7361,
        FCC_RIFF = 0x46464952,  // resource interchange file format
        FCC_TTF = 0x00000100,   // files with this signature seems to be ttf but it is Embedded OpenType fonts with ttf header
        FCC_OggS = 0x5367674f,
        FCC_ARAP = 0x50415241,  // relate to temp folder name of CoherentUI
        FCC_CTEX = 0x58455443,  // DXT5 compressed texture, custom format.

        // Texture codec
        FCC_DXT1 = 0x31545844,
        FCC_DXT2 = 0x32545844,
        FCC_DXT3 = 0x33545844,
        FCC_DXT4 = 0x34545844,
        FCC_DXT5 = 0x35545844,
        FCC_DXTN = 0x4e545844,
        FCC_DXTL = 0x4c545844,
        FCC_DXTA = 0x41545844,
        FCC_BC7X = 0x58374342,
        FCC_R32F = 0x00000072,

        // RIFF FourCC
        FCC_WEBP = 0x50424557,

        // PF FourCC
        FCC_ARMF = 0x464d5241,
        FCC_ASND = 0x444e5341,
        FCC_ABNK = 0x4b4e4241,
        FCC_ABIX = 0x58494241,
        FCC_AMSP = 0x50534d41,
        FCC_CDHS = 0x53484443,
        FCC_CINP = 0x504e4943,
        FCC_cntc = 0x63746e63,
        FCC_MODL = 0x4c444f4d,
        FCC_GEOM = 0x4d4f4547,
        FCC_DEPS = 0x53504544,
        FCC_EULA = 0x616c7565,
        FCC_hvkC = 0x436b7668,
        FCC_locl = 0x6c636f6c,  // store config, e-mail and hashed password
        FCC_mapc = 0x6370616d,
        FCC_mpsd = 0x6473706d,
        FCC_PIMG = 0x474d4950,
        FCC_AMAT = 0x54414d41,
        FCC_anic = 0x63696e61,
        FCC_emoc = 0x636f6d65,
        FCC_prlt = 0x746c7270,
        FCC_cmpc = 0x63706d63,
        FCC_txtm = 0x6d747874,
        FCC_txtV = 0x56747874,
        FCC_txtv = 0x76747874,
        FCC_PNG = 0x474e5089,
        FCC_cmaC = 0x43616d63,
        FCC_mMet = 0x74654d6d,
        FCC_AFNT = 0x544e4641,

        // Not quite FourCC
        FCC_MZ = 0x5a4d,        // Executable or Dynamic Link Library
        FCC_PF = 0x4650,
        FCC_MP3 = 0xfbff,       // MPEG-1 Layer 3
        FCC_JPEG = 0xffd8ff,
        FCC_ID3 = 0x334449,     // MP3 with an ID3v2 container
        FCC_BINK2 = 0x32424b,   // Bink 2 video
        FCC_UTF8 = 0xbfbbef,    // UTF-8 encoding
    };

    /** Contains the various (known) file formats in the dat. */
    enum ANetFileType {
        ANFT_Unknown,                   /**< Unknown format. */

        // Texture types
        ANFT_TextureStart,              /**< Values in between this and ANFT_TextureEnd are texture types. */
        ANFT_ATEX,                      /**< ATEX texture, generic use. */
        ANFT_ATTX,                      /**< ATTX texture, used for terrain (in GW1). */
        ANFT_ATEC,                      /**< ATEC texture, unknown use. */
        ANFT_ATEP,                      /**< ATEP texture, used for maps. */
        ANFT_ATEU,                      /**< ATEU texture, used for UI. */
        ANFT_ATET,                      /**< ATET texture, unknown use. */
        ANFT_CTEX,                      /**< CTEX texture, unknown use. */
        ANFT_DDS,                       /**< DDS texture, not an ANet specific format. */
        ANFT_JPEG,                      /**< JPEG Image, not an ANet specific format. */
        ANFT_WEBP,                      /**< WebP Image, not an ANet specific format. */
        ANFT_PNG,                       /**< PNG Image, not an ANet specific format. */
        ANFT_TextureEnd,                /**< Values in between this and ANFT_TextureStart are texture types. */

        // Sound
        ANFT_SoundStart,                /**< Values in between this and ANFT_SoundEnd are sound types. */
        ANFT_Sound,                     /**< Sound file of unknown type. */
        ANFT_asndMP3,                   /**< asnd MP3 format file. */
        ANFT_asndOgg,                   /**< asnd OGG format file. */
        ANFT_PackedMP3,                 /**< PF packed MP3 file. */
        ANFT_PackedOgg,                 /**< PF packed Ogg file. */
        ANFT_Ogg,                       /**< Uncompressed Ogg file. */
        ANFT_MP3,                       /**< Uncompressed MP3 file. */
        ANFT_SoundEnd,                  /**< Values in between this and ANFT_SoundStart are sound types. */

        // RIFF
        ANFT_RIFF,                      /**< Resource Interchange File Format container. */

        // PF
        ANFT_PF,                        /**< PF file of unknown type. */
        ANFT_Manifest,                  /**< Manifest file. */
        ANFT_TextPackManifest,          /**< TextPack Manifest file. */
        ANFT_TextPackVariant,           /**< TextPack Variant file. */
        ANFT_TextPackVoices,            /**< TextPack Voices file. */
        ANFT_Bank,                      /**< Soundbank file, contains other files. */
        ANFT_BankIndex,                 /**< Soundbank files index */
        ANFT_Model,                     /**< Model file. */
        ANFT_ModelCollisionManifest,    /**< Model collision manifest file. */
        ANFT_DependencyTable,           /**< Dependency table. */
        ANFT_EULA,                      /**< EULA file. */
        ANFT_GameContent,               /**< Game content file. */
        ANFT_GameContentPortalManifest, /**< Game content portal Manifest file. */
        ANFT_MapCollision,              /**< Map collision properties. */
        ANFT_MapParam,                  /**< Map file. */
        ANFT_MapShadow,                 /**< Map shadow file. */
        ANFT_MapMetadata,               /**< Map metadata file. */
        ANFT_PagedImageTable,           /**< Paged Image Table file. */
        ANFT_Material,                  /**< Compiled DirectX 9 shader. */
        ANFT_Composite,                 /**< Composite data. */
        ANFT_Cinematic,                 /**< Cinematic data. */
        ANFT_AnimSequences,             /**< Animation Sequences data. */
        ANFT_EmoteAnimation,            /**< Emote animation data. */
        ANFT_AudioScript,               /**< Audio script file. */
        ANFT_ShaderCache,               /**< Shader cache file. */
        ANFT_Config,                    /**< Configuration file. */

        // Binary
        ANFT_Binary,                    /**< Binary file of unknown type. */
        ANFT_DLL,                       /**< DLL file. */
        ANFT_EXE,                       /**< Executable file. */

        // Misc
        ANFT_StringFile,                /**< Strings file. */
        ANFT_FontFile,                  /**< Font file. */
        ANFT_BitmapFontFile,            /**< Bitmap font file. */
        ANFT_Bink2Video,                /**< Bink2 video file. */
        ANFT_ARAP,
        ANFT_UTF8,                      /**< UTF-8 encoding. */
        ANFT_TEXT,                      /**< Text file. */
    };

    /** Compression flags that appear in the MFT entries. */
    enum ANetCompressionFlags {
        ANCF_Uncompressed = 0,          /**< File is uncompressed. */
        ANCF_Compressed = 8,            /**< File is compressed. */
    };

    /** Flags appearing in MFT entries based on their use. */
    enum ANetMftEntryFlags {
        ANMEF_None = 0,                 /**< No flags set. */
        ANMEF_InUse = 1,                /**< Entry is in use. */
    };

    /** GW2 FVF fields. */
    enum ANetFlexibleVertexFormat {
        ANFVF_Position = 0x00000001,    /**< 12 bytes. Position as three 32-bit floats in the order x, y, z. */
        ANFVF_Weights = 0x00000002,     /**< 4 bytes. Contains bone weights. */
        ANFVF_Group = 0x00000004,       /**< 4 bytes. Related to bone weights. */
        ANFVF_Normal = 0x00000008,      /**< 12 bytes. Normal as three 32-bit floats in the order x, y, z. */
        ANFVF_Color = 0x00000010,       /**< 4 bytes. Vertex color. */
        ANFVF_Tangent = 0x00000020,     /**< 12 bytes. Tangent as three 32-bit floats in the order x, y, z. */
        ANFVF_Bitangent = 0x00000040,   /**< 12 bytes. Bitangent as three 32-bit floats in the order x, y, z. */
        ANFVF_TangentFrame = 0x00000080,    /**< 12 bytes. */
        ANFVF_UV32Mask = 0x0000ff00,    /**< 8 bytes for each set bit. Contains UV-coords as two 32-bit floats in the order u, v. */
        ANFVF_UV16Mask = 0x00ff0000,    /**< 4 bytes for each set bit. Contains UV-coords as two 16-bit floats in the order u, v. */
        ANFVF_Unknown1 = 0x01000000,    /**< 48 bytes. Unknown data. */
        ANFVF_Unknown2 = 0x02000000,    /**< 4 bytes. Unknown data. */
        ANFVF_Unknown3 = 0x04000000,    /**< 4 bytes. Unknown data. */
        ANFVF_Unknown4 = 0x08000000,    /**< 16 bytes. Unknown data. */
        ANFVF_PositionCompressed = 0x10000000,  /**< 6 bytes. Position as three 16-bit floats in the order x, y, z. */
        ANFVF_Unknown5 = 0x20000000,    /**< 12 bytes. Unknown data. */
    };

#pragma pack(push, 1)

    /** Gw2.dat file header. */
    struct ANetDatHeader {
        byte version;                   /**< Version of the .dat file format. */
        byte identifier[3];             /**< 0x41 0x4e 0x1a */
        uint32 headerSize;              /**< Size of this header. */
        uint32 unknownField1;
        uint32 chunkSize;               /**< Size of each chunk in the file. */
        uint32 cRC;                     /**< CRC of the 16 first bytes of the header. */
        uint32 unknownField2;
        uint64 mftOffset;               /**< Offset to the MFT, from the start of the file. */
        uint32 mftSize;                 /**< Size of the MFT, in bytes. */
        uint32 flags;
    };

    /** Gw2.dat file MFT header (entry 0). */
    struct ANetMftHeader {
        byte identifier[4];             /**< 'Mft' 0x1a */
        uint64 unknownField1;
        uint32 numEntries;              /**< Amount of entries in the MFT, including this. */
        uint64 unknownField2;
    };

    /** Gw2.dat file MFT entry. */
    struct ANetMftEntry {
        uint64 offset;                  /**< Location in the dat that the file is stored at. */
        uint32 size;                    /**< Uncompressed size of the file. */
        uint16 compressionFlag;         /**< Entry compression flags. See ANetCompressionFlags. */
        uint16 entryFlags;              /**< Entry flags. See ANetMftEntryFlags. */
        uint32 counter;                 /**< Was 'counter' in GW1, seems unused in GW1. */
        uint32 crc;                     /**< Was 'crc' in GW1, seems to have different usage in GW2. */
    };

    /** Gw2.dat fileId->mftEntry table entry. */
    struct ANetFileIdEntry {
        uint32 fileId;                  /**< File ID. */
        uint32 mftEntryIndex;           /**< Index of the file in the mft. */
    };

    /** ANet file reference data. */
    struct ANetFileReference {
        uint16 parts[3];                /**< Part1 is always above 0x100, Part2 is always between 0x100 and 0x101, Part3 is always 0x00 */
    };

    /** ATEX file header. */
    struct ANetAtexHeader {
        union {
            byte identifier[4];         /**< File identifier (FourCC). */
            uint32 identifierInteger;   /**< File identifier (FourCC), as integer. */
        };
        union {
            byte format[4];             /**< Format of the contained data. */
            uint32 formatInteger;       /**< Format of the contained data, as integer. */
        };
        uint16 width;                   /**< Width of the texture, in pixels. */
        uint16 height;                  /**< Height of the texture, in pixels. */
    };

    /** PF file header. */
    struct ANetPfHeader {
        byte identifier[2];             /**< Always 'PF'. */
        uint16 unknownField1;
        uint16 unknownField2;           /**< Must always be 0 according to the exe. */
        uint16 pkFileVersion;           /**< PF-version of this file (0xc). */

        union {
            byte type[4];               /**< Type of data contained in this PF file. */
            uint32 typeInteger;         /**< Type of data contained in this PF file, as integer for easy comparison. */
        };
    };

    /** PF file chunk header. */
    struct ANetPfChunkHeader {
        union {
            byte chunkType[4];          /**< Identifies the chunk type. */
            uint32 chunkTypeInteger;    /**< Identifies the chunk type, as integer for easy comparison. */
        };
        uint32 chunkDataSize;           /**< Total size of this chunk, excluding this field and mChunkType, but \e including the remaining fields. */
        uint16 chunkVersion;            /**< Version of this chunk. */
        uint16 chunkHeaderSize;         /**< Size of the chunk header. */
        uint32 offsetTableOffset;       /**< Offset to the offset table. */
    };

    /** MODL file, MODL chunk permutations. */
    struct ANetModelMaterialPermutations {
        uint64 token;
        uint32 materialCount;
        int32 materialsOffset;
    };

    /** MODL file, MODL chunk material info data. */
    struct ANetModelMaterialData {
        uint64 token;
        uint32 materialId;
        int32 materialFileOffset;       /**< Offset to material file reference. */
        uint32 materialFlags;           /**< Mesh flags. */
        uint32 sortOrder;
        uint32 textureCount;            /**< Amount of texture references. */
        int32 texturesOffset;           /**< Offset to texture references. */
        uint32 constantsCount;          /**< Amount of constantss for use by the material. */
        int32 constantsOffset;          /**< Offset to constants data. */
        uint32 matConstLinksCount;      /**< Amount of matConstLinks pointed to by mHashesOffset. Only contains matConstLinks used by vectors. */
        int32 matConstLinksOffset;      /**< Offset to matConstLinks. */
        uint32 uvTransLinksCount;
        int32 uvTransLinksOffset;
        uint32 texTransforms4Count;
        int32 texTransforms4Offset;
        byte texCoordCount;
    };

    /** MODL file, MODL chunk texture reference data. */
    struct ANetModelTextureReference {
        int32 offsetToFileReference;    /**< Offset to the texture file reference. */
        uint32 textureFlags;
        uint64 token;                   /**< Hash used to associate the texture with a variable in the material. */
        uint64 blitId;
        uint32 uvAnimId;
        byte uvPSInputIndex;
    };

#pragma pack(pop)

}; // namespace gw2mw

#endif // ANETSTRUCTS_H_INCLUDED
