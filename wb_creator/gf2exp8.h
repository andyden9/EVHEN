//***************************************************************************************
// gf2exp8.h
// GF(2^8) multiplication lookup tables
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
#ifndef GF2EXP8_H
#define GF2EXP8_H

#include "stdtypes.h"

namespace NGF2exp8
{
extern const uint8_t rijndael_field_poly;
extern const uint8_t rijndael_primitive_element;

uint8_t get_poly_by_index( int i );
uint8_t gmul_tab( uint8_t a, uint8_t b, uint8_t p );
uint8_t inv_tab( uint8_t b, uint8_t p );
uint8_t gdiv_tab( uint8_t a, uint8_t b, uint8_t p );

// Rijndael field specific
uint8_t* rijndael_create_exp_table(uint8_t tbl[256]);
uint8_t	 rijndael_get_primitive_element_power(int power);
}

#endif // GF2EXP8_H