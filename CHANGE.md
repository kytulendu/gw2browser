Change log
----------

Gw2Browser_2026-xx-xx (1.0.9.1)
-------------------------------
Change:
- Linux port
- Improve index loading and .dat scanning performance.
- Add AFNT file support.
- Add find by file id function.
- Add model's lightmap, specular and dye map export.
- Add support for BC7 texture compression.
- Categorize CTEX.
- Model viewer lightmap support.
- Model viewer now load DXT5 texture directly from ATEX file.
- Properly implement specular mapping for Guild Wars 2 texture.
- Set Model viewer light position at camera position.
- Update category name.
- Use KDE Breeze icons as the old one are hard to see on Windows 10.
- Minor model viewer purrformance improvement.

Fix:
- Many crashes and bugs fixed.

Gw2Browser_2017-01-07 (1.0.8.1)
-------------------------------
Change:
- Add string, text and eula file viewer.
- Add sound player.
- Add normal mapping, specular mapping, model lighting, Anisotropic texture filtering
to model renderer. Set anisotropic texture filtering to maximum supported by GPU.
- Add ModelViewer toggle indicator.
- Reduce texture memory usage when model viewer load texture.
- Change model renderer fov to 45 degree.
- When export model with textures, skip if the texture file is already exists.
- PNG file support.
- Add text file detection.
- Faster speed when scanning string files.
- When export model, also export all materials.
- Add first-person camera (for use with map viewer in future).

Fix:
- Fix memory leak in model viewer.
- Fix string and eula file export bug.
- Fix text clipping when model is close to camera.
- Fix crash on viewing model that have no material or uvs or missing texture.
- Fix crash on reading some video file.
- Fix model viewer not work on Intel HD Graphics.

Gw2Browser_2016-04-10 (1.0.7.1)
-------------------------------
Change:
- Gw2Browser can now compile with TDM-GCC (and possibly MinGW and GCC on linux as well).
- Use wxAUI for interface manager.
- New about box.
- Sound Bank and Eula file support.
- Improve string file support, add support for Chinese language string.
- Some small improvement in ModelReader.
- Wavefront .obj model export improvement.
Model now export with texture/material and can import to Blender using
default Wavefront .obj import settings, no need to rotate the imported model.
- More stability, less crash.

Gw2Browser_2016-01-01
---------------------
Change:
- Model viewer now use OpenGL for rendering the model, require OpenGL 3.3 support.
- Faster extraction of uncompressed files.
- Wavefront .obj model export improvement.
No need to rotate the exported model file in Blender, may need to
rotate it to face the camera in Blender tho.
- Improved model viewer's wireframe view.
- Khral is now really confused, since he have taken 25 stack of confusion
during the development of OpenGL model viewer. (really, he's not good at math)
- Change string file reader csv export delimiter from comma (,) to semicolon (;).
since comma some times use in the strings.
- Categorized string files by language.
- Merged Packed Ogg and MP3 reader.
- Merge onTreeExtractConverted and onTreeExtractRaw to onTreeExtractFile.
- Model file now have it's own icon.

Fix:
- Fix some texture not in it's sub category.
- Fix Wavefront .obj formatting if the model not have uv.
- Eliminate z-fighting (hopefully).

Gw2Browser_2014-10-06
---------------------
Change:
- String and Sound files support. Strings can be export to CSV,
If open the CSV file in Notepad++, go to Encoding -> Convert to ANSI
and go to Encoding again -> Encode in UTF-8 for the text to display correctly.
- Ogg, Configuration and ARAP file detection.
- Add icon for model files.
- Some change in category name.
- Set Gw2Browser to center of screen.

Fix:
- Fix memory leak on viewing WebP files.
- Fix extraction of uncompressed files larger than 65532 bytes.
- Fix MP3 files doesn't have icon.

Issue:
- Some textures not go to it's sub category. *Fixed in Gw2Browser_2016-01-01*

Gw2Browser_2014-04-24
---------------------
Change:
- Add texture id info used with the model in model viewer, texture file need to be extract manually.

Gw2Browser_2014-03-25
---------------------
Change:
- Hack for read 126x64 ATEX files.
- Model Reader now use high-res texture, still need to figure out ModelReader to not load LOD models.
- WebP support, can export to png. WebP can open by Chrome browser.
- Bink 2 video detect, to play it you need The RAD Video Tools
but the player will play only 1 second of video and it will crash.
*Fixed in Gw2Browser_2014-10-06*
- ID3 (MP3), Embedded OpenType font, TextPack Variant, Map content and Shader Cache files detection.
- Simple Wireframe view.
- Force enable 4x MSAA.

Removed:
- ABFF file support.

Fix:
- Fix crash when viewing Luminance DDS file.
- Fix camera not look at center of model.

Issue:
- Memory leak on viewing WebP files. *Fixed in Gw2Browser_2014-10-06*

Gw2Browser_2014-01-30
---------------------
Change:
- Files will sort as ascending order
- JPEG files support.
- When export as raw, file extension will add automatically, example, export DDS file(s) as raw,
the file(s) will have .dds as extension or export string file(s), the file(s) will have extension .strs.
- Change hex viewer font to easy to read font, display only printable ASCII character.
- Use icons from Open Icon Library.
- Re-catalog files.
- Replace XNA Math with OpenGL Mathematics, doesn't finish yet.

Issue:
- ModelViewer camera not look at center of model. *Fixed in Gw2Browser_2014-03-25*
- Some times after the program finish re-index .dat file, some of catalog may missing,
simply restart Gw2Browser will fix this.

Gw2Browser_2014-01-07
---------------------
Change:
- 64 bit build.
