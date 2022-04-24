
//=================================================================================================
// Original File Name : Signal.cpp
// Original Author    : duncang
// Creation Date      : May 9, 2011
// Copyright          : Copyright © 2011 - 2022 by Catraeus and Duncan Gray
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

#include <math.h>

#include <caesmusfil/Sig/Signal.hpp>

         Signal::Signal       ( void                  )
: ary ( NULL ) {
  MakeBlank();
}
         Signal::~Signal      ( void                  ) {
}
//=================================================================================================
// Initiation
void     Signal::Setup        ( void                  ) {
  posFrames       = 0;
  return;
  }
void     Signal::MakeBlank    ( void                  ) {
  if(ary != 0) {
    for(llong chDex = 0; chDex < ch; chDex++) {
      delete ary[chDex];
      }
    delete ary;
    ary = 0;
    }
  ch        = 1;
  size      = 1;
  res       = false;
  posFrames = 0;
  FS        = 48000.0;

  isIQ      = false;
  TLPV      = 1.0;
  TLPD      = 1.0;
  return;
  }
void     Signal::SetT         ( double i_t            ) {
  llong oldN;
  llong newN;
  llong oldC;

  i_t *= GetFS();
  i_t += DBL_ROUNDER;
  newN   = (llong)i_t;
  oldN   = GetN();
  if(oldN == newN)
    return;
  oldC = GetCh();
  ReBase(oldC, newN);
  return;
}

double **Signal::ReBase       ( llong i_ch, llong i_N ) {

  MakeBlank();
       if(i_ch <= 0 ) ch = 1;
  else if(i_ch >  4 ) ch = 4;
  else                ch = i_ch;
       if(i_N  <=           0 )    size =          1;
  else if(i_N  > FRAME_LIM_5H )    size = FRAME_LIM_5H;
  else                             size = i_N;
  ary = new double*[ch];
  for(llong i=0; i<ch; i++)
    ary[i] = new double[size];
  res = false;
  return ary;
  }
void     Signal::RemoveDC     ( void                  ) {
  double   tDC;
  double  *pSrc;

  for(llong j=0; j<ch; j++) {
    tDC  = 0.0;           pSrc = ary[j];    for(llong i=0; i<size; i++)   tDC += *(pSrc++);
    tDC /= (double)size;  pSrc = ary[j];    for(llong i=0; i<size; i++)          *(pSrc++) -= tDC;
    }
  return;
  }
//=================================================================================================
// Operational
void     Signal::GrabHere     ( double **o_v          ) { // Would also be called GetPosPtr();
  for(llong chDex = 0; chDex < ch; chDex++)
    o_v[chDex] = &(ary[chDex][posFrames]);
  return;
  }
void     Signal::SetPosFrames ( llong    i_set        ) {
       if(i_set > size) posFrames = size;
  else if(i_set < 0) posFrames = 0;
  else               posFrames = i_set;
  return;
}
//=================================================================================================
//Metadata
