//***************************************************************************************
// matrix.cpp
// CMatrix and CCauchyMatrix classes
// CMatrix describes a matrix over GF 2^8
// CCauchyMatrix describes a square Cauchy matrix  over GF 2^8 (MDS matrix)
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

#include "matrix.h"
#include "prng.h"
#include "sbox.h"

namespace NGFMatrix
{

CMatrix multiply(const CMatrix &m1, const CMatrix &m2, uint8_t field)
{
	CMatrix m;

	if (m1.Cols() != m2.Rows() || m1.Rows() != m2.Rows())
		return m;

	m.Init(m1.Rows(), m2.Cols());

	for (uint32_t i = 0; i < m1.Rows(); ++i)
		for (uint32_t k = 0; k < m2.Cols(); ++k)
			for (uint32_t j = 0; j < m1.Cols(); ++j)
				m[i][k] ^= NGF2exp8::gmul_tab(m1[i][j], m2[j][k], field);

	return m;
}


CCauchyMatrix::CCauchyMatrix() : m_size(0), m_field(0){}
CCauchyMatrix::CCauchyMatrix(const CCauchyMatrix &m) : m_size(m.m_size), m_matrix(m.m_matrix), m_init(m.m_init), m_field(m.m_field){}
CCauchyMatrix::~CCauchyMatrix(){}

const CCauchyMatrix& CCauchyMatrix::operator=(const CCauchyMatrix& m)
{
	m_size = m.m_size;
	m_matrix = m.m_matrix;
	m_init = m.m_init;
	m_field = m.m_field;

	return *this;
}

bool CCauchyMatrix::Init(CCauchyMatrix::size_type size, uint8_t field, bool gen_values)
{
	if (size < 1 || size > 32)
		return false;

	Release();
	m_init.clear();

	m_size = size;
	m_field = field;
	m_matrix.Init(size, size);

	if (!gen_values)
		return true;

	// Create elements randomly
	//for (std::vector<uint8_t>::size_type i(0); i < 256; ++i)
	//	m_init.push_back((uint8_t)i);
	//NPrng::shuffle(m_init);
	NWhiteBox::create_8bit_sboxes_chaotically(m_init);

	for (size_type i = 0; i < m_size; ++i)
		for (size_type j = 0; j < m_size; ++j)
			m_matrix[i][j] = NGF2exp8::inv_tab(m_init[i] ^ m_init[j + m_size], m_field);

	return true;
}

void CCauchyMatrix::Release()
{
	m_size = 0;
	m_matrix.Release();
	m_init.clear();
	m_field = 0;
}

uint8_t CCauchyMatrix::Determinant(size_type row, size_type col) const
{
	if (!IsInit())
		return 0;
	uint32_t size = (row >= m_size || col >= m_size) ? m_size : (m_size - 1);
	uint8_t numerator(1), denominator(1);
	for (uint32_t i = 1; i < size; ++i)
	{
		for (uint32_t j = 0; j < i; ++j)
		{
			numerator = NGF2exp8::gmul_tab(NGF2exp8::gmul_tab(m_init[i < row ? i : (i + 1)] ^ m_init[j < row ? j : (j + 1)],
				m_init[(j < col ? j : (j + 1)) + m_size] ^ m_init[(i < col ? i : (i + 1)) + m_size], m_field),
				numerator, m_field);
		}
	}

	for (uint32_t i = 0; i < size; ++i)
	{
		for (uint32_t j = 0; j < size; ++j)
		{
			denominator = NGF2exp8::gmul_tab(m_init[i < row ? i : (i + 1)] ^ m_init[(j < col ? j : (j + 1)) + m_size],
				denominator, m_field);
		}
	}
	
	return NGF2exp8::gmul_tab(numerator, NGF2exp8::inv_tab(denominator, m_field), m_field);
}

CCauchyMatrix CCauchyMatrix::MinorMatrix(uint32_t row, uint32_t col) const
{
	CCauchyMatrix m;
	m.Init(m_size - 1, false);

	if (!IsInit())
	{
		return m;
	}

	for (uint32_t i = 0; i < m_size; ++i)
	{
		if (i == row)
			continue;
		m.GetInitVector().push_back(m_init[i]);
	};
		
	for (uint32_t j = 0; j < m_size; ++j)
	{
		if (j == col)
			continue;
		m.GetInitVector().push_back(m_init[j + m_size]);
	};

	for (size_type i = 0; i < m.Size(); ++i)
		for (size_type j = 0; j < m.Size(); ++j)
			m.GetNativeMatrix()[i][j] = NGF2exp8::inv_tab(m.GetInitVector()[i] ^ m.GetInitVector()[j + m.Size()], m_field);
	
	return m;
}

CMatrix CCauchyMatrix::Inverse() const
{
	CMatrix m;
	Inverse(m);
	return m;
}

void CCauchyMatrix::Inverse(CMatrix& m) const
{
	m.Release();
	if (!IsInit())
		return;

	m.Init(m_size, m_size);

	uint8_t mul = NGF2exp8::inv_tab(Determinant(), m_field);

	for (uint32_t i = 0; i < m_size; ++i)
		for (uint32_t j = 0; j < m_size; ++j)
			m[j][i] = NGF2exp8::gmul_tab(mul, Determinant(i, j), m_field);

}

};