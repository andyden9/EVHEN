//***************************************************************************************
// poly.h
// Polynomials over GF(2^8)
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

#ifndef POLY_H
#define POLY_H

#include "gf2exp8.h"
#include <vector>

namespace NGFPoly
{

class CPoly
{
public:
    typedef std::vector<uint8_t>::size_type     size_type;

public:
	CPoly() : m_validate_size(true)
    {}

	CPoly(std::vector<uint8_t> const& v) : m_v(v), m_validate_size(true)
    {}

	CPoly(CPoly const& p) : m_v(p.m_v), m_validate_size(p.m_validate_size)
    {}

	CPoly(uint8_t val, size_type pos) : m_validate_size(true)
    {
        m_v.resize( pos + 1 );
        m_v[pos] = val;
    }
   
    virtual ~CPoly()
    {}

public:
	void volatile_size(bool v)
	{
		m_validate_size = v;
	}

	bool volatile_size() const
	{
		return m_validate_size;
	}

public:
    uint8_t operator[]( size_type index ) const
    {
        return m_v[index];
    }

    uint8_t& operator[]( size_type index )
    {
        return m_v[index];
    }

    size_type size() const
    {   
        return m_v.size();
    }

    void clear()
    {
        m_v.clear();
    }

    operator bool() const
    {
        return !m_v.empty();
    }

    void reserve( size_type count )
    {
        //m_v.reserve( count );
		m_v.resize(count);
    }

    void push( uint8_t val )
    {
        m_v.push_back( val );
    }

    void increase( std::vector<uint8_t> const& v )
    {
        m_v.insert( m_v.end(), v.begin(), v.end() );
    }

public:
    CPoly const& operator *() const
    {
        return *this;
    }

    CPoly& operator *()
    {
        return *this;
    }

    CPoly operator =( CPoly const& p )
    {
        m_v = p.m_v;
        return *this;
    }

    bool operator ==( CPoly const& p )
    {
        return m_v == p.m_v;
    }

    bool operator !=( CPoly const& p )
    {
        return !( *this == p );
    }

    bool operator <( CPoly const& p ) const
    {
        if( !(*this) && !p )
            return false;

        if( size() < p.size() )
            return true;

        if( size() > p.size() )
            return false;

        for( size_type i = m_v.size() - 1; i; --i )
        {
            if( m_v[i] == p.m_v[i] )
                continue;
            return m_v[i] < p.m_v[i]; 
        }

        return m_v[0] < p.m_v[0]; 
    }

    bool operator <=( CPoly const& p ) const
    {
        return ( (*this) < p || *(this) == p );
    }

    bool operator >( CPoly const& p ) const
    {
        return !( (*this) <= p );
    }

    bool operator >=( CPoly const& p ) const
    {
        return !( (*this) < p );
    }

    void validate_size()
    {
		if (!(*this) || !m_validate_size)
            return;

        size_type count = m_v.size() - 1;

        for( ; count != 0 && !m_v[count]; --count )
        {
        }

        if( count == 0 && !m_v[count] )
        {
            m_v.clear();
        }
        else
        {
            m_v.resize( count + 1 );
        }
    }

private:
    std::vector<uint8_t>	m_v;
	bool					m_validate_size;


};

CPoly operator ^( CPoly const& p1, CPoly const& p2 );
CPoly operator +( CPoly const& p1, CPoly const& p2 );
CPoly operator -( CPoly const& p1, CPoly const& p2 );
CPoly operator >>( CPoly const& p, CPoly::size_type k );
CPoly operator <<( CPoly const& p, CPoly::size_type k );
CPoly mul( CPoly const& p1, CPoly const& p2, uint8_t irr_p );
CPoly div( CPoly const& p1, CPoly const& p2, uint8_t irr_p, CPoly& q ); // returns remainder
CPoly euclid_algo( CPoly const& p1, CPoly const& p2, uint8_t irr_p );
CPoly normalize( CPoly const& p, uint8_t irr_p );

CPoly create_randomly(uint32_t size, bool nzero = true, bool volatile_size = true);

CPoly gpoly_increase_power(CPoly const& init_poly, uint32_t b);
CPoly gpoly_create(uint32_t power, uint32_t b);
}

#endif // POLY_H