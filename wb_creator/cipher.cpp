//***************************************************************************************
// cipher.cpp
// A creator of the chaotic white-box tables
//
// Copyright © 2016-2017 Dmitry Schelkunov. All rights reserved.
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

#include "cipher.h"
#include <stdio.h>
#include <stdlib.h>
#include "prng.h"


namespace NWhiteBox
{

static char* const license = {
	"Copyright © 2009-2010, 2016-2017 Dmitry Schelkunov. All rights reserved.\n"
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
	"along with this program.If not, see <http://www.gnu.org/licenses/>.\n\n"
};

std::string val_to_str( int val )
{
    char buf[10];
    if( _itoa_s( val, buf, sizeof( buf ), 10 ) )
        throw std::runtime_error( "Can\'t convert value to string!!!" );
    return buf;
}

std::string tbox_to_str( tbox_t const& item )
{
    std::string s( "{ " );
    for( int i = 0; i < sizeof( tbox_t ); ++i )
    {     
        s += val_to_str( item[i] );
        if( i != sizeof( tbox_t ) )
            s+= ", ";
        else
            s+= " ";
    }
    s += "}";
    return s;
}

void show_matrix(const NGFMatrix::CMatrix &m)
{
	if (!m.IsInit())
	{
		printf_s("Matrix is not initialized!!!\n");
		return;
	}
	printf_s("\n");
	for (uint32_t i = 0; i < m.Rows(); ++i)
	{
		for (uint32_t j = 0; j < m.Cols(); ++j)
		{
			printf_s("%#4x ", m[i][j]);
		}
	}
}

void show_matrix(const NGFMatrix::CCauchyMatrix &m)
{
	show_matrix(m.GetNativeMatrix());
}
 
//
// CCipherCreator
// 

CCipherCreator::CCipherCreator( uint32_t rnum, uint32_t min_mix_count, uint32_t max_mix_count ) : m_rnum( rnum ), 
m_min_mix_count( min_mix_count ), m_max_mix_count( max_mix_count )
{

}

CCipherCreator::~CCipherCreator()
{

}

void CCipherCreator::Init()
{
    if( m_rnum < 2 )
        throw std::runtime_error( "ERROR: Rounds number must be equal or greater than 2!!!" );

    // Create white-box lookup tables for every round
    CRound rnd( m_min_mix_count, m_max_mix_count );
    rnd.Init();
    m_rounds.push_back( rnd );

    for( uint32_t i = 1; i < m_rnum - 1; ++i )
    {
        rnd.Clear();
		rnd.Init(m_rounds[i - 1].GetMixes(), m_rounds[i - 1].GetAdditiveMask());
        m_rounds.push_back( rnd );
    }

    CRound last( m_min_mix_count, m_max_mix_count, false, true );
	last.Init(m_rounds[m_rnum - 2].GetMixes(), m_rounds[m_rnum - 2].GetAdditiveMask());
    m_rounds.push_back( last );

	// Create white-box inverse lookup tables (decryption lookup tables) for every round
    rnd.Clear();
    //rnd.SetMinMixesCount( 0 );	// uncomment for debugging
    //rnd.SetMaxMixesCount( 0 );	// uncomment for debugging
    rnd.SetDecryption( true );
    CRound::s_boxes_t   s_boxes_inv;
    InverseSboxes( last.GetClearSboxes(), s_boxes_inv );
	rnd.Init(s_boxes_inv, m_rounds[m_rnum - 2].GetMdsMatrix(),
        m_rounds[m_rnum - 2].GetIrreduciblePoly() );
    m_anti_rounds.push_back( rnd );

    for( uint32_t i = m_rnum - 2; i >= 1; --i )
    {
        rnd.Clear();     
        InverseSboxes( m_rounds[i].GetClearSboxes(), s_boxes_inv );
		rnd.Init(s_boxes_inv, m_rounds[i - 1].GetMdsMatrix(),
			m_rounds[i - 1].GetIrreduciblePoly(), m_anti_rounds[m_anti_rounds.size() - 1].GetMixes(), 
			m_anti_rounds[m_anti_rounds.size() - 1].GetAdditiveMask());
        m_anti_rounds.push_back( rnd );
    }

    last.Clear();
    last.SetDecryption( true );
	NGFMatrix::CCauchyMatrix m;
    InverseSboxes( m_rounds[0].GetClearSboxes(), s_boxes_inv );
	last.Init(s_boxes_inv, m, m_rounds[0].GetIrreduciblePoly(), m_anti_rounds[m_anti_rounds.size() - 1].GetMixes(), 
		m_anti_rounds[m_anti_rounds.size() - 1].GetAdditiveMask());
    m_anti_rounds.push_back( last );
}

void CCipherCreator::Flash( std::string const& fname_encr, std::string const& fname_decr )
{
    FlashOneFile( fname_encr, "wb_encr_tbl", m_rounds );
    FlashOneFile( fname_decr, "wb_decr_tbl", m_anti_rounds );
}

void CCipherCreator::FlashOneFile( std::string const& fname, std::string const& tbl_name, std::vector<CRound> const& rounds )
{
    FILE* f;
    errno_t err = fopen_s( &f, fname.c_str(), "w" );  
    if( err != 0 )
        throw std::runtime_error( std::string( "ERROR: Can\'t open \'" ) + fname + "\' file!!!\n" );

    std::string::size_type pos = fname.find_first_of( '.' );
    std::string str_name;
    for( std::string::size_type i = 0; i < pos; ++i )
        str_name += fname[i];

    std::string str_wb( "/*****************************************************************************************\n" );
    str_wb += "Chaotically generated EVHEN white-box tables\n\n";
	str_wb += license;
    str_wb += "*****************************************************************************************/\n\n\n";
    str_wb += "#include \"stdtypes.h\"\n#ifndef " + str_name + "_H\n#define " + str_name + "_H\n\n";

    // Convert rounds to T-boxes
    for( uint32_t i = 0; i < m_rnum; ++i )
    {
        str_wb += "const tbox_t " + tbl_name + "_" + val_to_str( i ) + "[16][256] = { \n";

		NGFPoly::CPoly additive_masks_sum;
		additive_masks_sum.reserve(16);
		additive_masks_sum.volatile_size(false);

		NGFMatrix::CMatrix inv_mds;
		const NGFMatrix::CMatrix *mds(0);

		if (!rounds[i].IsLast())
		{
			if (rounds[i].IsDecyption())
			{
				rounds[i].GetMdsMatrix().Inverse(inv_mds);
				mds = &inv_mds;
			}
			else
			{
				mds = &rounds[i].GetMdsMatrix().GetNativeMatrix();
			}
		}

        for( uint32_t j = 0; j < 16; ++j )
        {   
            str_wb += "{ ";

            tbox_t tbox_clear;
            
            if( !rounds[i].IsLast() )
            {
                for( uint32_t cnt = 0; cnt < sizeof( tbox_t ); ++cnt  )
					tbox_clear[cnt] = (*mds)[cnt][j];
            }
            else
            {
                for( uint32_t cnt = 0; cnt < sizeof( tbox_t ); ++cnt )
                    tbox_clear[cnt] = ( cnt == j ) ? 1 : 0;
            }

			NGFPoly::CPoly rnd_additive_mask(j < 15 ? NGFPoly::create_randomly(16, true, false) : (additive_masks_sum ^ rounds[i].GetAdditiveMask()));
			additive_masks_sum = rnd_additive_mask ^ additive_masks_sum;

            for( uint32_t k = 0; k < 256; ++k )
            {
                tbox_t tbox;
                
                for( uint32_t cnt = 0; cnt < sizeof( tbox_t ); ++cnt )
                {
                    tbox[cnt] = NGF2exp8::gmul_tab( rounds[i].GetSboxes()[j].at( k ), 
                        tbox_clear[cnt], rounds[i].GetIrreduciblePoly() );
                    for( std::vector<CRound::mix_t>::size_type mix_cnt = 0; mix_cnt < rounds[i].GetMixes().size(); ++mix_cnt )
                    {
                        tbox[cnt] =  NGF2exp8::gmul_tab( ( rounds[i].GetMixes()[mix_cnt] )[cnt].a, tbox[cnt],
                            ( rounds[i].GetMixes()[mix_cnt] )[cnt].p );
                    }

					if (!rounds[i].IsLast())
					{
						if (rounds[i].GetAdditiveMask().size() == 16)
						{
							tbox[cnt] ^= rnd_additive_mask[cnt];
						}
					}
                }
                
                str_wb += tbox_to_str( tbox );
                if( k != 255 )
                    str_wb += ",\n";
                else
                    str_wb += "\n";
            }

            if( j != 15 )
                str_wb += "},\n";
            else
                str_wb += "}\n";  
        }
        
        str_wb += "};\n";       
    }

    str_wb += "\n#endif // " + str_name + "_H\n";
    fwrite( str_wb.c_str(), sizeof( char ), str_wb.size(), f );

    fclose( f );
}

}
