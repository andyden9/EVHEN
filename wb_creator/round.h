//***************************************************************************************
// round.h
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

#ifndef ROUND_H
#define ROUND_H

#include "poly.h"
#include <vector>
#include "matrix.h"
#include "sbox.h"

namespace NWhiteBox
{

class CRound
{
public:
    struct tuple // x * a. Multiplication in GF(2^8). p - irreducible.
    {
        uint8_t a;
        uint8_t p;
    };

    struct mix_t
    {
        tuple const& operator[]( size_t index ) const
        {
            return m_mix[index];
        }

        tuple& operator[]( size_t index )
        {
            return m_mix[index];
        }

        tuple m_mix[16];
    };

    typedef std::vector<uint8_t>    s_boxes_t[16];

public:
    CRound( uint32_t min_mixes_count, uint32_t max_mixes_count, bool is_decr = false, bool is_last = false );
	CRound(uint32_t min_mixes_count, uint32_t max_mixes_count, std::vector<mix_t> const& prev_mixes, 
		const NGFPoly::CPoly& prev_additive_mask, bool is_decr = false, bool is_last = false );
    CRound( CRound const& r );
    virtual ~CRound();

public:
    void Init();
	void Init(std::vector<mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask);
    void Init( s_boxes_t const& s_boxes_clear, NGFMatrix::CCauchyMatrix const& m,
		uint8_t irr_p, std::vector<mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask);
	void Init( s_boxes_t const& s_boxes_clear, NGFMatrix::CCauchyMatrix const& m,
        uint8_t irr_p ); 
    void Clear();

private:
    void CreateMixes();
	void CreateMdsMatrix();
	void CreateSboxes(std::vector<mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask);
	void ApplyPrevMixes(std::vector<mix_t> const& prev_mixes, const NGFPoly::CPoly& prev_additive_mask);

public:
    CRound const& operator =( CRound const& r );

public:
    s_boxes_t const& GetSboxes() const
    {
        return m_s_boxes;
    }

    s_boxes_t const& GetClearSboxes() const
    {
        return m_s_boxes_clear;
    }

	NGFMatrix::CCauchyMatrix const& GetMdsMatrix() const
	{
		return m_mds_matrix;
	}

	NGFPoly::CPoly const& GetAdditiveMask() const
	{
		return m_additive_mask;
	}

    uint8_t GetIrreduciblePoly() const
    {
        return m_irr_p;
    }       

    std::vector<mix_t> const& GetMixes() const
    {
        return m_mixes;
    }

    bool IsLast() const
    {
        return m_is_last;
    }

    uint32_t GetMinMixesCount() const
    {
        return m_min_mixes_count;
    }

    uint32_t GetMaxMixesCount() const
    {
        return m_max_mixes_count;
    }

    void SetMinMixesCount( uint32_t min_mixes_count )
    {
        m_min_mixes_count = min_mixes_count;
    }

    void SetMaxMixesCount( uint32_t max_mixes_count )
    {
        m_max_mixes_count = max_mixes_count;
    }

    bool IsDecyption() const
    {
        return m_is_decr;
    }

    void SetDecryption( bool is_decr )
    {
        m_is_decr = is_decr;
    }

private:
    s_boxes_t					m_s_boxes;
    s_boxes_t					m_s_boxes_clear;
    uint8_t						m_irr_p;
    std::vector<mix_t>			m_mixes;
    bool						m_is_last; 
    uint32_t					m_min_mixes_count;
    uint32_t					m_max_mixes_count;
    bool						m_is_decr;
	NGFPoly::CPoly				m_additive_mask;
	NGFMatrix::CCauchyMatrix	m_mds_matrix;
};

void InverseSboxes( CRound::s_boxes_t const& s_boxes_in, CRound::s_boxes_t& s_boxes_out );

}

#endif // ROUND_H