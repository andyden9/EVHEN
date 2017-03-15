//***************************************************************************************
// poly.cpp
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
#include "poly.h"
#include "prng.h"

namespace NGFPoly
{

CPoly operator ^( CPoly const& p1, CPoly const& p2 )
{   
    CPoly const* min_poly ;
    CPoly const* max_poly;

    if( p1.size() > p2.size() )
    {
        min_poly = &p2;
        max_poly = &p1;
    }
    else
    {
        min_poly = &p1;
        max_poly = &p2;
    }

    std::vector<uint8_t> vret;
    vret.reserve( max_poly->size() );
    CPoly::size_type i;
    
    for( i = 0; i < min_poly->size(); ++i )
        vret.push_back( (*min_poly)[i] ^ (*max_poly)[i] );
        
    for( ; i < max_poly->size(); ++i )
        vret.push_back( (*max_poly)[i] );

    CPoly ret( vret );
	ret.volatile_size(max_poly->volatile_size());
    ret.validate_size();
    return ret;
}

CPoly operator +( CPoly const& p1, CPoly const& p2 )
{
    return p1 ^ p2;
}

CPoly operator -( CPoly const& p1, CPoly const& p2 )
{
    return p1 ^ p2;
}

CPoly operator >>( CPoly const& p, CPoly::size_type k )
{
	if (!p || !k)
        return p;

    CPoly ret;
	ret.volatile_size(p.volatile_size());
    ret.reserve( p.size() );

	for (CPoly::size_type i = 0; i < p.size(); ++i)
	{
		if ((i + k) < p.size())
		{
			ret[i] = p[i + k];
		}
		else
		{
			ret[i] = 0;
		}
	}

    ret.validate_size();
    return ret;
}

CPoly operator <<( CPoly const& p, CPoly::size_type k )
{
    if( !p || !k )
        return p;

    CPoly ret;
    ret.reserve( p.size() );
	ret.volatile_size(p.volatile_size());

	for (CPoly::size_type i = 0; i < p.size(); ++i)
	{
		if ((p.size() - 1 - i + k) < ret.size())
		{
			ret[p.size() - 1 - i + k] = p[p.size() - 1 - i];
		}
	}
    
    ret.validate_size();
    return ret;
}

CPoly mul( CPoly const& p1, CPoly const& p2, uint8_t irr_p )
{
    CPoly ret;

    if( !p1 || !p2 )
        return ret;

    for( CPoly::size_type i = 0; i < p1.size(); ++i )
    {
        std::vector<uint8_t> v;

        for( CPoly::size_type j = 0; j < p2.size(); ++j )
        {
            v.push_back( NGF2exp8::gmul_tab( p1[i], p2[j], irr_p ) );
        }
        if( i != 0 )
        {
            ret.push( v[v.size() - 1] );

            for( CPoly::size_type j = 0; j < ( v.size() - 1 ); ++j )
            {
                ret[j + i] ^= v[j];
            }
        }
        else
        {
            ret = v;
        }
    }
    
    ret.validate_size();
    return ret;
}

CPoly div( CPoly const& p1, CPoly const& p2, uint8_t irr_p, CPoly& q )
{
    if( !p1 )
    {
        q = p1;
        return p1;
    }

    if( !p2 )
        std::runtime_error( "Error: Division by zero!!!\n" );

    if( p2.size() > p1.size() )
    {
        q = CPoly();
        return p1;
    }

    CPoly d( p1 );
    std::vector<uint8_t> v;
    v.resize( p1.size() - p2.size() + 1 );

    for( ; d.size() >= p2.size(); )
    {
        uint8_t t = v[d.size() - p2.size()] = NGF2exp8::gdiv_tab( d[d.size() - 1], p2[p2.size() - 1], irr_p );
        CPoly p( t, d.size() - p2.size() );
        p = mul( p, p2, irr_p );
        d = d - p;
    }

    q = v;
    d.validate_size();
    q.validate_size();
    return d;
}

CPoly normalize( CPoly const& p, uint8_t irr_p )
{
    if( !p )
        return p;

    CPoly ret( p );
    ret.validate_size();

    uint8_t val = NGF2exp8::inv_tab( ret[ret.size() - 1], irr_p );

    for( CPoly::size_type i = 0; i < ret.size(); ++i )
        ret[i] = NGF2exp8::gmul_tab( ret[i], val, irr_p );

    return ret;
}

CPoly euclid_algo( CPoly const& p1, CPoly const& p2, uint8_t irr_p )
{  
    CPoly p1_norm( p1 );
    CPoly p2_norm( p2 );

    CPoly u( p1_norm );
    CPoly v( p2_norm );

    CPoly d1( 1, 0 ), d2, d, q, /*s,*/ r/*, t, x1, x2( 1, 0 ), x*/;

    u.validate_size();
    v.validate_size();

    for( ; ; )
    {        
        if( u < v )
        {
            std::swap( u, v );
            std::swap( d1, d2 );
        }

        r = div( u, v, irr_p, q );

        if( !r )
            break;

        d = mul( q, d1, irr_p );
        d = div( d, p1_norm, irr_p, /*t*/q );
        d = d2 - d;

        d2 = d1; d1 = d; u = v; v = r;

    }

    if( v.size() != 1 )
        return CPoly();

    uint8_t val = NGF2exp8::inv_tab( v[0], irr_p );
    for( CPoly::size_type i = 0; i < d.size(); ++i )
        d[i] = NGF2exp8::gmul_tab( d[i], val, irr_p ); 

    return d;
}

CPoly create_randomly(uint32_t size, bool nzero, bool volatile_size)
{
	CPoly p;
	for (uint32_t i = 0; i < size; ++i)
	{
		uint8_t r = NPrng::get_rnd_8();
		if (nzero)
			for (; !r; r = NPrng::get_rnd_8()){}
		p.push(r);
	}
	p.volatile_size(volatile_size);
	return p;

}

CPoly gpoly_increase_power(CPoly const& init_poly, uint32_t b)
{
	CPoly p;
	p.reserve(init_poly.size() + 1);
	
	p[p.size() - 1] = init_poly[init_poly.size() - 1];

	p[0] = NGF2exp8::gmul_tab(init_poly[0], NGF2exp8::rijndael_get_primitive_element_power(p.size() + b - 2), NGF2exp8::rijndael_field_poly);
	
	if (init_poly.size() <= 1)
		return p;

	for (CPoly::size_type i = 1; i < init_poly.size(); ++i)
		p[i] = init_poly[i - 1] ^ NGF2exp8::gmul_tab(init_poly[i], NGF2exp8::rijndael_get_primitive_element_power(p.size() + b - 2), NGF2exp8::rijndael_field_poly);

	return p;
}

CPoly gpoly_create(uint32_t power, uint32_t b)
{
	CPoly p(1, 0);

	for (uint32_t i(0); i < power; ++i)
		p = gpoly_increase_power(p, b);
	
	return p;
}

}