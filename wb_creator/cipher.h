//***************************************************************************************
// cipher.h
// A creator of the chaotic white-box tables
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

#ifndef CIPHER_H
#define CIPHER_H

#include "round.h"
#include <string>

namespace NWhiteBox
{

class CCipherCreator
{
public:
    CCipherCreator( uint32_t rnum, uint32_t min_mix_count, uint32_t max_mix_count );
    virtual ~CCipherCreator();

public:
    void Flash( std::string const& fname_encr, std::string const& fname_decr );
    void FlashOneFile( std::string const& fname, std::string const& tbl_name, std::vector<CRound> const& rounds );
    void Init();

public:
    uint32_t GetRoundsNum() const
    {
        return m_rnum;
    }

    std::vector<CRound> const& GetRounds() const
    {
        return m_rounds;
    }

    std::vector<CRound> const& GetAntiRounds() const
    {
        return m_anti_rounds;
    }

    uint32_t GetMinMixCount() const
    {
        return m_min_mix_count;
    }

    uint32_t GetMaxMixCount() const
    {
        return m_max_mix_count;
    }

private:
    uint32_t                m_rnum;
    uint32_t                m_min_mix_count;
    uint32_t                m_max_mix_count;
    std::vector<CRound>     m_rounds;
    std::vector<CRound>     m_anti_rounds;
    
};

}

#endif // CIPHER_H