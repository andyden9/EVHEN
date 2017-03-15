//***************************************************************************************
// round.cpp
// A creator of a round of EVHEN (chaotic white-box cipher)
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
#include "round.h"
#include "prng.h"

namespace NWhiteBox
{

void InverseSboxes( CRound::s_boxes_t const& s_boxes_in, CRound::s_boxes_t& s_boxes_out )
{
    for( int i = 0; i < 16; ++i )
    {
        s_boxes_out[( i + ( i % 4 ) * 4 ) % 16].resize( 256, 0 );  // !!!!!!!!!!!!!!!!!!!!!!!!!!
        for( int j = 0; j < 256; ++j )
            s_boxes_out[( i + ( i % 4 ) * 4 ) % 16].at( s_boxes_in[i].at( j ) ) = (uint8_t)j; // !!!!!!!!!!!!!!!!!!!!!!
    }
}

//
// CRound
//

CRound::CRound( uint32_t min_mixes_count, uint32_t max_mixes_count, bool is_decr, bool is_last ) : m_is_last( is_last ),
m_min_mixes_count( min_mixes_count ), m_max_mixes_count( max_mixes_count ), m_is_decr( is_decr )
{
}

CRound::CRound(uint32_t min_mixes_count, uint32_t max_mixes_count, std::vector<mix_t> const& prev_mixes, 
	const NGFPoly::CPoly& prev_additive_mask, bool is_decr, bool is_last) : m_is_last(is_last),
m_min_mixes_count( min_mixes_count ), m_max_mixes_count( max_mixes_count ), m_is_decr( is_decr )
{
	Init(prev_mixes, prev_additive_mask);
}

CRound::CRound( CRound const& r ) : /*m_poly( r.m_poly ), m_anti_poly( r.m_anti_poly ),*/ m_irr_p( r.m_irr_p ),
m_is_last( r.m_is_last ), m_mixes( r.m_mixes ), m_min_mixes_count( r.m_min_mixes_count ), m_max_mixes_count( r.m_max_mixes_count ), 
m_is_decr(r.m_is_decr), m_additive_mask(r.m_additive_mask), m_mds_matrix(r.m_mds_matrix)
{
    for( int i = 0; i < 16; ++i )
    {
        m_s_boxes[i] = r.m_s_boxes[i];
        m_s_boxes_clear[i] = r.m_s_boxes_clear[i];
    }
}

CRound::~CRound()
{
}

void CRound::Init()
{
    std::vector<CRound::mix_t> v;
	NGFPoly::CPoly p;
    Init(v, p);
}

void CRound::Init(s_boxes_t const& s_boxes_clear, NGFMatrix::CCauchyMatrix const& m,
        uint8_t irr_p )
{
    std::vector<CRound::mix_t> v; 
	NGFPoly::CPoly p;
    Init(s_boxes_clear, m, irr_p, v, p);
}

void CRound::Init(s_boxes_t const& s_boxes_clear, NGFMatrix::CCauchyMatrix const& m,
	uint8_t irr_p, std::vector<mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask)
{
    // Fill S-boxes
    for( int i = 0; i < 16; ++i )
    {
        m_s_boxes[i] = m_s_boxes_clear[i] = s_boxes_clear[i];
    }

	ApplyPrevMixes(prev_mixes, prev_additive_mask);

	m_mds_matrix = m;

    m_irr_p = irr_p;

    if( m_is_last )
        return;

    CreateMixes();
}

void CRound::Clear()
{
	m_mds_matrix.Release();
    m_mixes.clear();
    m_irr_p = 0;
	m_additive_mask.clear();
}

void CRound::Init(std::vector<CRound::mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask)
{
    Clear();
	CreateSboxes(prev_mixes, prev_additive_mask);
	CreateMdsMatrix();
    if( m_is_last )
        return;
    CreateMixes();
}

void CRound::ApplyPrevMixes(std::vector<mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask)
{
    class calc_index
    {
    public:
        calc_index( bool decr ) : m_decr( decr ){}
        ~calc_index(){}
        
        int operator()( int i ) const
        {
            i =  ( !m_decr ) ? ( ( i + ( i % 4 ) * 4 ) % 16 ) : ( ((unsigned int)( i - ( i % 4 ) * 4 )) % 16 ); 
            return i;
        }

    private:
        bool m_decr;
    };
    
    
    if( prev_mixes.empty() )
    {
        for( int i = 0; i < 16; ++i )
        {
            m_s_boxes[i] = m_s_boxes_clear[i];
        }
        return;
    }

    calc_index ci( m_is_decr );

    for( int i = 0; i < 16; ++i )
    {     
        for( std::vector<uint8_t>::size_type j = 0; j < 256; ++j )
        {
            uint8_t index = (uint8_t)j;
            for( std::vector<CRound::mix_t>::size_type k = 0; k < prev_mixes.size(); ++k )
            {    
				index = NGF2exp8::gmul_tab(index, prev_mixes.at(k)[ci(i)].a, prev_mixes.at(k)[ci(i)].p);
            }
			if (prev_additive_mask.size() == 16)
				index ^= prev_additive_mask[ci(i)];
            m_s_boxes[i].at( index ) = m_s_boxes_clear[i].at( j );
        }
    }
}

void CRound::CreateSboxes(std::vector<CRound::mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask)
{
    // Create S-boxes chaotically
    //for( int i = 0; i < 16; ++i )
    //{
    //    m_s_boxes[i].resize( 256 );
    //    m_s_boxes_clear[i].clear();
    //    m_s_boxes_clear[i].push_back( 0 );
    //    for( uint8_t j = 1; j != 0; ++j )
    //        m_s_boxes_clear[i].push_back( j );
    //    NPrng::shuffle( m_s_boxes_clear[i] );
    //}
	for (int i = 0; i < 16; ++i)
	{
		m_s_boxes[i].resize(256);
		NWhiteBox::create_8bit_sboxes_chaotically(m_s_boxes_clear[i]);
	}
    ApplyPrevMixes(prev_mixes, prev_additive_mask);
}

void CRound::CreateMdsMatrix()
{
	m_irr_p = NGF2exp8::get_poly_by_index(NPrng::get_rnd_32() % 30);
	if (m_is_last)
		return;
	m_mds_matrix.Init(16, m_irr_p);
}

void CRound::CreateMixes()
{
    if( !m_max_mixes_count )
        return;

    // Create mixes
    uint32_t mixes_num = m_min_mixes_count + 
        ( ( m_min_mixes_count != m_max_mixes_count ) ? ( NPrng::get_rnd_32() %( m_max_mixes_count - m_min_mixes_count ) ) : 0 );
    
    std::vector<uint8_t> v_irr_p;

    for( uint32_t i = 0; i < 30; ++i )
        v_irr_p.push_back( NGF2exp8::get_poly_by_index( i ) );

    for( uint32_t i = 0; i < mixes_num; ++i )
    {
        CRound::mix_t mix_array;
        NPrng::shuffle( v_irr_p );                           
        for( int j = 0; j < 16; ++j )
        {   
            for( ; ; )
            {
                mix_array[j].a = NPrng::get_rnd_8();
                if( mix_array[j].a )
                    break;
            }
            mix_array[j].p = v_irr_p[j];
        }
        m_mixes.push_back( mix_array );
    }

	m_additive_mask = NGFPoly::create_randomly(16, true, false);
}

CRound const& CRound::operator =( CRound const& r )
{
    for( int i = 0; i < 16; ++i )
    {
        m_s_boxes[i] = r.m_s_boxes[i];
        m_s_boxes_clear[i] = r.m_s_boxes_clear[i];
    }

	m_mds_matrix = r.m_mds_matrix;
    m_irr_p = r.m_irr_p;
    m_is_last = r.m_is_last;
    m_mixes = r.m_mixes; 
    m_min_mixes_count = r.m_min_mixes_count;
    m_max_mixes_count = r.m_max_mixes_count;
	m_additive_mask = r.m_additive_mask;

    return *this;
}

}