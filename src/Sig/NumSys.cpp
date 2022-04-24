
//=================================================================================================
// Original File Name : NumSys.cpp
// Original Author    : duncang
// Creation Date      : Jul 6, 2014 11:27:16 AM
// Copyright          : Copyright © 2014 - 2022 by Catraeus and Duncan Gray
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
   See the header file for this source file for a detailed description.
*/
//=================================================================================================

#include "stdlib.h"

#include <caesmusfil/Sig/NumSys.hpp>

char *NumSys::typeNames[] = {
    ((char[]){'I', 'n', 't', '0', '0', '\0'}),
    ((char[]){'I', 'n', 't', '1', '6', '\0'}),
    ((char[]){'I', 'n', 't', '2', '4', '\0'}),
    ((char[]){'I', 'n', 't', '3', '2', '\0'}),
    ((char[]){'I', 'n', 't', '6', '4', '\0'}),
    ((char[]){'f', 'l', 'o', 'a', 't', '\0'}),
    ((char[]){'d', 'o', 'u', 'b', 'l', 'e', '\0'}),
    ((char[]){'f', 'n', 'v', 'a', 'l', 'i', 'd', '\0'})};
llong NumSys::numNames  = 7;

NumSys::NumSys() {
  Reset();
  }

NumSys::~NumSys() {
}
void  NumSys::Reset(void) {
  bitDepth  = 0;
  byteDepth = 0;
  type      = ET_INVALID;
  field     = EF_INVALID;
  return;
}
void  NumSys::SetBitDepth(llong i_d) {
  if(i_d <   EC_MIN_DEPTH)
    i_d  =   EC_MIN_DEPTH;
  if(i_d > EC_MAX_DEPTH)
    i_d  = EC_MAX_DEPTH;
  i_d = (i_d & (llong)(((llong)EC_MASK_HI << (llong)32) | (llong)EC_MASK_LO)); // Shortcut to make ciel mod-8

  bitDepth  = i_d;
  byteDepth = i_d >> 3;

  switch(field) {
    case EF_REAL:
      switch(bitDepth) {
        case 32: type = ET_FLOAT;   break;
        case 64: type = ET_DOUBLE;  break;
        default: type = ET_INVALID; break;
        }
      break;
    case EF_INTEGER:
      switch(bitDepth) {
        case  8: type = ET_INT08;   break;
        case 16: type = ET_INT16;   break;
        case 24: type = ET_INT24;   break;
        case 32: type = ET_INT32;   break;
        case 64: type = ET_INT64;   break;
        default: type = ET_INVALID; break;
        }
      break;
    default:     type = ET_INVALID; break;
      break;
    }
  return;
  }
void  NumSys::SetByteDepth(llong i_d) {
  SetBitDepth(i_d * 8);
  return;
  }
void  NumSys::SetNumType(eType i_type) {
  switch(i_type) {
    case ET_INT08:
      bitDepth =  8;
      field = EF_INTEGER;
      break;
    case ET_INT16:
      bitDepth = 16;
      field = EF_INTEGER;
      break;
    case ET_INT24:
      bitDepth = 24;
      field = EF_INTEGER;
      break;
    case ET_INT32:
      bitDepth = 32;
      field = EF_INTEGER;
      break;
    case ET_INT64:
      bitDepth = 64;
      field = EF_INTEGER;
      break;
    case ET_FLOAT:
      bitDepth = 32;
      field = EF_REAL;
      break;
    default:
      i_type = ET_DOUBLE;
      bitDepth = 64;
      field = EF_REAL;
      break;
    }
  type = i_type;
  return;
  }
void  NumSys::SetField(eField i_field) {
  switch(i_field) {
    case EF_INTEGER:
      switch(bitDepth) {
        case  8:
          byteDepth = 1;
          type = ET_INT08;
          break;
        case 16:
          byteDepth = 2;
          type = ET_INT16;
          break;
        case 24:
          byteDepth = 3;
          type = ET_INT24;
          break;
          // reduce test time by not testing for the default set-to value
        case 64:
          byteDepth = 8;
          type = ET_INT64;
          break;
        default:
          bitDepth  = 32;
          byteDepth =  4;
          type      = ET_INT32;
          break;
        }
      break;
      case EF_REAL:
        switch(bitDepth) {
          case 32:
            byteDepth =  4;
            type      = ET_FLOAT;
            break;
            // reduce test time by not testing for the default set-to value
          default:
            bitDepth  = 64;
            byteDepth =  8;
            type      = ET_DOUBLE;
            break;
          }
        break;
    default:
      bitDepth  = 32;
      byteDepth =  4;
      field     = EF_INTEGER;
      type      = ET_INT32;
      break;
    }
  return;
}
