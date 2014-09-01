/* \file       Readers/StringReader.h
*  \brief      Contains the declaration of the String reader class.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>

This file is part of Gw2Browser.

Gw2Browser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef READERS_STRINGREADER_H_INCLUDED
#define READERS_STRINGREADER_H_INCLUDED

#include "FileReader.h"

namespace gw2b {

	class StringReader : public FileReader {
	public:
		/** Constructor.
		*  \param[in]  p_data       Data to be handled by this reader.
		*  \param[in]  p_fileType   File type of the given data. */
		StringReader( const Array<byte>& p_data, ANetFileType p_fileType );
		/** Destructor. Clears all data. */
		virtual ~StringReader( );

		/** Gets the type of data contained in this file. Not to be confused with
		*  file type.
		*  \return DataType    type of data. */
		virtual DataType dataType( ) const override {
			return DT_String;
		}
		/** Gets an appropriate file extension for the contents of this reader.
		*  \return wxString    File extension. */
		virtual const wxChar* extension( ) const override {
			return wxT( ".csv" );
		}
		/** Converts the data associated with this file into CSV file.
		*  \return Array<byte> converted data. */
		virtual Array<byte> convertData( ) const;
		/** Gets the strings contained in the data owned by this reader.
		*  \return wxString     CSV format of strings. */
		wxString getString( ) const;
		/** Determines whether the header is valid.
		*  \return bool    true if valid, false if not. */
		static bool isValidHeader( const byte* p_data );
	}; // class StringReader

}; // namespace gw2b

#endif // READERS_STRINGREADER_H_INCLUDED
