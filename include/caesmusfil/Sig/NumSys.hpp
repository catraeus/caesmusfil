
//=================================================================================================
// Original File Name : NumSys.hpp
// Original Author    : duncang
// Creation Date      : Jul 6, 2014 11:27:16 AM
// Copyright          : Copyright © 2014 by Catraeus and Duncan Gray
//
//=================================================================================================
/*
This file is part of the caesmusfil libraries.

caesmusfil is free software: you can redistribute it and/or modify it under the terms of
  the GNU Lesser General Public License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

caesmusfil is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
  the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with dmmsnoop.
  If not, see <https://www.gnu.org/licenses/>.
*/
//=================================================================================================
// Description:
/*
    A wav file-specific number system handler, to perform the yadda, yadda ... yadda work.

*/
//=================================================================================================

#ifndef __NUM_SYS_HPP_
#define __NUM_SYS_HPP_

#include <caesbase/CaesTypes.hpp>

class WaveFiler;

class NumSys {
  public:
    enum eType {
      ET_INT08   = 0,  //!<Serialization is 2's compliment integer.
      ET_INT16   = 1,  //!<Serialization is 2's compliment integer.
      ET_INT24   = 2,  //!<Serialization is 2's compliment integer.
      ET_INT32   = 3,  //!<Serialization is 2's compliment integer.
      ET_INT64   = 4,  //!<Serialization is 2's compliment integer.
      ET_FLOAT   = 5,  //!<Serialization IEEE float.
      ET_DOUBLE  = 6,  //!<Serialization IEEE float.
      ET_INVALID = 7   //!<Sorry folks.
    };
    enum eStore {
      ES_INTER,
      ES_CAT,
      ES_BLOCK
    };
    enum eRing {
      ER_REAL,
      ER_CPLX
    };
    enum eGrain {
      EG_INT_2C,
      EG_INT_1C,
      EG_INT_OB,
      EG_FLOAT_IEEE,
      EG_FLOAT_U,
      EG_FLOAT_A
    };
    enum eField {
      EF_INTEGER,  //!<Serialization is 2's compliment integer.
      EF_REAL,     //!<Serialization IEEE float.
      EF_COMPLEX,  //!<Serialization IEEE float.
      EF_INVALID   //!<Sorry folks.
    };
  private:
  enum eConst {
    EC_MASK_LO   = 0xfffffff8,
    EC_MASK_HI   = 0xffffffff,
    EC_MAX_DEPTH = 64,
    EC_MIN_DEPTH = 8
    };
  protected:
             NumSys();
    virtual ~NumSys();

  public:
    void     SetBitDepth   (llong  i_d);
    void     SetByteDepth  (llong  i_d);
    void     SetNumType    (eType  i_t);
    void     SetField      (eField i_f);
    llong    GetBitDepth   (void)       {return bitDepth;  };
    llong    GetByteDepth  (void)       {return byteDepth; };
    eType    GetType       (void)       {return type;      };
    eField   GetField      (void)       {return field;     };
    char    *GetFieldName  (void)       {return typeNames[type];};
    void     Reset         (void)                                 ;
  private:
    //* - Initialized by create/reset        at compile
    //. - Initialized by creation            at instantiation
    //= - Initialized by reference           at instantiation
    //+ - Initialized by delete/create/reset at all reset events
  public:
    static char    *typeNames[];  //*
    static llong    numNames;     //*
  private:
           friend class WaveFiler;
           llong    bitDepth;              //+
           llong    byteDepth;             //+
           eType    type;                  //+
           eField   field;                 //+
  };

#endif /* __NFIELD_HPP_ */
