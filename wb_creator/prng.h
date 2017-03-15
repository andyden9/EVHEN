//***************************************************************************************
// prng.h
// Pseudo random numbers generator (Windows specific)
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

#ifndef PRNG_H
#define PRNG_H

#include <stdio.h>
#include "mpir.h"
#include "poly.h"
#include <algorithm>


namespace NPrng
{
void get_rnd_128( void* buf, uint32_t size );
uint32_t get_rnd_32();
uint8_t get_rnd_8();
NGFPoly::CPoly get_rnd_128_poly();

extern mpf_t seed;

void iterate_PLCM(mpf_t res, mpf_t x, mpf_t p);



template <class T>
void shuffle( std::vector<T>& v )
{
    class Fn_rnd
    {
    public:
        uint32_t operator()( uint32_t index ) const
        {
			if (!index)
				return 0;

			return get_rnd_32() % index;
        }
    } f;

    std::random_shuffle( v.begin(), v.end(), f );
}

}

#endif // PRNG_H