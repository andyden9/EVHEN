//***************************************************************************************
// main.cpp
// Entry point of the EVHEN keys generator
//
// Copyright © 2009-2010, 2016-2017 Dmitry Schelkunov. All rights reserved.
// Contacts: <d.schelkunov@gmail.com>, <http://dschelkunov.blogspot.com/>
//
// This file is part of wb_creator.
//
// wb_creator is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// wb_creator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with wb_creator.If not, see <http://www.gnu.org/licenses/>.
//
//***************************************************************************************

#include "poly.h"
#include "prng.h"
#include "cipher.h"


static char* const hello = { 
    "EVHEN white-box lookup tables generator\n\n"
    "Copyright (c) 2009-2010, 2016-2017 Dmitry Schelkunov. All rights reserved.\n"
	"Contacts: <d.schelkunov@gmail.com>, <http://dschelkunov.blogspot.com/>\n\n"
	
	"This program is free software : you can redistribute it and / or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation, either version 3 of the License, or\n"
	"(at your option) any later version.\n\n"

	"This program is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\n"
	"GNU General Public License for more details.\n\n"

	"You should have received a copy of the GNU General Public License\n"
	"along with this program.If not, see <http://www.gnu.org/licenses/>.\n\n\n"
    "USAGE: wb_creator.exe number_of_rounds min_number_of_mixes max_number_of_mixes\n\n"
};

int main( int argc, char* argv[] )
{
    
    printf_s( "%s", hello );
    
    if( argc != 4 )
    {
        printf_s( "%s", "Use wb_creator.exe number_of_rounds min_number_of_mixes max_number_of_mixes!\n" );
        return 1;
    }

    uint32_t rounds_num = (uint32_t)atol( argv[1] );
    uint32_t min_mixes_num = (uint32_t)atol( argv[2] );
    uint32_t max_mixes_num = (uint32_t)atol( argv[3] );

    try
    {
        if( min_mixes_num > max_mixes_num )
            throw std::runtime_error( "ERROR: min_mixes_number must be less or equal to max_mixes_number!!!\n" );
        
        NWhiteBox::CCipherCreator c( rounds_num, min_mixes_num, max_mixes_num );
        c.Init();
        c.Flash( "wb_encr_tbl.h", "wb_decr_tbl.h" );
    }
    catch( std::runtime_error& e )
    {
        printf_s( "%s", e.what() );
        getchar();
        return 2;
    }
    catch( ... )
    {
        printf_s( "%s", "Unknown internal error!\n" );
        getchar();
        return 3;
    }
        
    return 0;
}

