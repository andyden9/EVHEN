//***************************************************************************************
// prng.cpp
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
#include "prng.h"
#include <iostream>

#ifdef WIN32
#pragma comment(lib, "crypt32.lib")
#include <Windows.h>
#include <Wincrypt.h>
#endif // WIN32

namespace NPrng
{

mpf_t seed;

void iterate_PLCM(mpf_t res, mpf_t x, mpf_t p)
{
	int cmp_z = mpf_cmp_d(x, 0);
	int cmp_p = mpf_cmp(x, p);
	int cmp_half = mpf_cmp_d(x, 0.5);
	int cmp_1 = mpf_cmp_d(x, 1);
	if (cmp_z >= 0 && cmp_p <= 0) // 0 <= x <= p
	{
		mpf_div(res, x, p);
	}
	else if (cmp_p && cmp_half <= 0) // p < x <= 0.5
	{
		mpf_t s1, s2, half_one;
		mpf_init2(s1, 256);
		mpf_init2(s2, 256);
		mpf_init2(half_one, 256);
		mpf_set_d(half_one, 0.5);

		mpf_sub(s1, x, p);
		mpf_sub(s2, half_one, p);
		mpf_div(res, s1, s2);
	}
	else if (cmp_half && cmp_1 <= 0) // 0.5 < x <= 1
	{
		mpf_t s1, one;
		mpf_init2(s1, 256);
		mpf_init2(one, 256);
		mpf_set_d(one, 1);
		mpf_sub(s1, one, x);
		
		iterate_PLCM(res, s1, p);
	}

}

#ifdef WIN32
class CWinSpecificPrngCtx
{
public:
	CWinSpecificPrngCtx()
	{
		if ( !::CryptAcquireContext(
			&m_hCryptProv,
			NULL,
			NULL,
			PROV_RSA_FULL,
			CRYPT_NEWKEYSET))
		{
			DWORD err = ::GetLastError();
			if (err == NTE_EXISTS)
			{ 
				if (!::CryptAcquireContext(
					&m_hCryptProv,
					NULL,
					NULL,
					PROV_RSA_FULL,
					CRYPT_DELETEKEYSET))
				{
					DWORD err = ::GetLastError();
					printf("ERROR: CryptAcquireContext!!!\n");
					exit(err);
				}
				if (m_hCryptProv)
					::CryptReleaseContext(m_hCryptProv, 0);

				if (!::CryptAcquireContext(
					&m_hCryptProv,
					NULL,
					NULL,
					PROV_RSA_FULL,
					CRYPT_NEWKEYSET))
				{
					DWORD err = ::GetLastError();
					printf("ERROR: CryptAcquireContext!!!\n");
					exit(err);
				}
			}
			else
			{
				printf("ERROR: CryptAcquireContext!!!\n");
				exit(err);
			}
		}

		mpf_init2(seed, 256);

		uint8_t buf[32];

		if (!CryptGenRandom(
			m_hCryptProv,
			32,
			(PBYTE)buf))
		{
			printf("ERROR: CryptGenRandom!!!\n");
			DWORD err = ::GetLastError();
			exit(err);
		}

		char t[10];
		std::string s;

		for (uint32_t i = 0; i < 32; ++i)
		{
			sprintf_s(t, 10, "%u", buf[i]);
			s += t;
		}
		std::string::iterator it;
		uint32_t ex = 0;
		for (it = s.begin(); it != s.end() && ex < 4; ++it, ++ex){}

		s.insert(it, '.');
		s += "@-4";

		int e = mpf_set_str(seed, s.c_str(), 10);
		if (e)
		{
			printf("ERROR: mpf_set_str!!!\n");
			exit(e);
		}

	}

	void get(void* buf, uint32_t size, uint32_t len)
	{
		if (size < len)
			throw std::runtime_error("ERROR: Illegal size!!!\n");

		if (!CryptGenRandom(
			m_hCryptProv,
			len,
			(PBYTE)buf))
		{
			printf("ERROR: CryptGenRandom!!!\n");
			DWORD err = ::GetLastError();
			exit(err);
		}
	}

	~CWinSpecificPrngCtx()
	{
		if (m_hCryptProv)
			::CryptReleaseContext(m_hCryptProv, 0);
	}

private:
	HCRYPTPROV   m_hCryptProv;
};

CWinSpecificPrngCtx g_win_prng_ctx;

#endif // WIN32


void get_rnd_128( void* buf, uint32_t size )
{
	if (size != 16)
		throw std::runtime_error("ERROR: Illegal size!!!\n");

#ifdef WIN32
	g_win_prng_ctx.get(buf, size, 16);
#endif // WIN32
}

uint32_t get_rnd_32()
{
	
	uint32_t val;
#ifdef WIN32
	g_win_prng_ctx.get(&val, sizeof(val), sizeof(val));
#endif // WIN32
	return val;
}

uint8_t get_rnd_8()
{
	return (uint8_t)get_rnd_32();
}

NGFPoly::CPoly get_rnd_128_poly()
{   
    uint8_t rnd_val[16];

    NPrng::get_rnd_128( rnd_val, sizeof( rnd_val ) );

    NGFPoly::CPoly r;
    for( int i = 0; i < sizeof( rnd_val ); ++i )
    {
        if( rnd_val[i] )
            r.push( rnd_val[i] );
        else
            r.push( 1 );
    }

    return r;

}

}