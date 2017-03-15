//***************************************************************************************
// matrix.h
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

#ifndef GFMATRIX_H
#define GFMATRIX_H

#include "poly.h"

namespace NGFMatrix
{
	
class CMatrix
{
public:
	typedef uint32_t    size_type;

public:
	CMatrix() : m_rows(0), m_cols(0), m_matrix(0){};
	
	CMatrix(const CMatrix& m) : m_rows(0), m_cols(0), m_matrix(0)
	{
		if (!m.IsInit())
			return;
		Init(m.m_rows, m.m_cols);
		for (size_type i = 0; i < m_rows; ++i)
			for (size_type j = 0; j < m_cols; ++j)
				m_matrix[i * m_rows + j] = m[i][j];
	}

	virtual ~CMatrix(){};

public:
	bool Init(uint32_t rows, uint32_t cols)
	{
		if (rows < 1 || cols < 1)
			return false;

		Release();

		m_matrix = new uint8_t[rows * cols];
		m_rows = rows;
		m_cols = cols;
		for (size_type i = 0; i < rows; ++i)
			for (size_type j = 0; j < cols; ++j)
				m_matrix[i * rows + j] = 0;
		return true;
	}

	void Release()
	{
		if (m_matrix)
			delete[] m_matrix;
		m_rows = m_cols = 0;
		m_matrix = 0;
	}

public:
	bool IsInit() const
	{
		return !(m_rows == 0);
	}

	uint32_t Rows() const
	{
		return m_rows;
	}

	uint32_t Cols() const
	{
		return m_cols;
	}

public:
	const CMatrix& operator=(const CMatrix& m)
	{
		Release();
		if (!m.IsInit())
			return *this;
		Init(m.m_rows, m.m_cols);
		for (size_type i = 0; i < m_rows; ++i)
			for (size_type j = 0; j < m_cols; ++j)
				m_matrix[i * m_rows + j] = m[i][j];

		return *this;
	}

	const uint8_t* operator[](size_type i) const
	{
		return &m_matrix[m_rows * (i % m_rows)];
	}

	uint8_t* operator[](size_type i)
	{
		return &m_matrix[m_rows * (i % m_rows)];
	}

private:
	uint8_t		*m_matrix;
	uint32_t	m_rows;
	uint32_t	m_cols;

};

// Square Cauchy matrix
class CCauchyMatrix
{
public:
	typedef CMatrix::size_type    size_type;

public:
	CCauchyMatrix();
	CCauchyMatrix(const CCauchyMatrix& m);
	virtual ~CCauchyMatrix();

public:
	bool Init(size_type size, uint8_t field, bool gen_values = true);
	void Release();
	
public:
	bool IsInit() const
	{
		return !(m_size == 0);
	}

	uint32_t Size() const
	{
		return m_size;
	}

	const CMatrix& GetNativeMatrix() const
	{
		return m_matrix;
	}

	CMatrix& GetNativeMatrix()
	{
		return m_matrix;
	}

public:
	uint8_t Determinant(size_type row = 0xffffffff, size_type col = 0xffffffff) const;
	CCauchyMatrix MinorMatrix(uint32_t row, uint32_t col) const;
	CMatrix Inverse() const;
	void Inverse(CMatrix&) const;

public:
	const CCauchyMatrix& operator=(const CCauchyMatrix& m);

	const uint8_t* operator[](size_type i) const
	{
		return m_matrix[i];
	}

	uint8_t* operator[](size_type i)
	{
		return m_matrix[i];
	}

public:
	std::vector<uint8_t>& GetInitVector()
	{
		return m_init;
	}

	const std::vector<uint8_t>& GetInitVector() const
	{
		return m_init;
	}

	uint8_t GetField() const
	{
		return m_field;
	}

	void SetField(uint8_t field)
	{
		m_field = field;
	}

private:
	uint32_t				m_size;
	CMatrix					m_matrix;
	std::vector<uint8_t>	m_init;
	uint8_t					m_field;
};

CMatrix multiply(const CMatrix &m1, const CMatrix &m2, uint8_t field);

};

#endif // GFMATRIX_H