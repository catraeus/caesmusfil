
// $Id: caes_Signal.hpp 31 2015-05-30 22:45:07Z duncang $

//=================================================================================================
// Original File Name : Signal.h
// Original Author    : duncang
// Creation Date      : May 9, 2011
// Copyright          : Copyright © 2011 by Catraeus and Duncan Gray
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
    This subsystem is the lowest layer that has data. It can make or take an bus of channel arrays.
      When asked for data, it just uses a "current position" to return an array of pointers to the locations
      in the actual data arrays, no copyint.
*/
//=================================================================================================

#ifndef __SIGNAL_HPP_
#define __SIGNAL_HPP_

#include <math.h>
#include <stdio.h>

#include <caesbase/CaesTypes.hpp>

class Signal  {
  private:
    enum eConst {
      FRAME_LIM_5H = 1000000000 // 5 hr, 47 min, 13.33333 s
    };
  public:
                      Signal       ( void                     );
    virtual          ~Signal       ( void                     );
    //infrastructure
    void              Setup        ( void                     );
    double          **ReBase       ( llong    i_ch, llong i_N );
    llong             GetCh        ( void                     ) {                       return           ch   ;};
//    void              ReSize       ( llong    i_N             ) { ReBase(ch, i_N);      return                ;};
    llong             GetN         ( void                     ) {                       return         size   ;};
    llong             GetMaxN      ( void                     ) {                       return FRAME_LIM_5H   ;};
    void              SetT         ( double   i_t             );
    double            GetT         ( void                     ) {                       return (double)size / FS ;};
    void              MakeBlank    ( void                     );
    //manipulate
    void              GrabHere     ( double **o_v             );
    void              SetPosFrames ( llong    i_pos           );
    void              SetFS        ( double   i_FS            ) { FS = i_FS;            return          ;};
    double            GetFS        ( void                     ) {                       return FS       ;};
    void              SetTLPV      ( double   i_TLPV          ) {TLPV = i_TLPV;         return          ;};
    double            GetTLPV      ( void                     ) {                       return TLPV     ;};
    void              SetTLPD      ( double   i_TLPD          ) {TLPV = i_TLPD;         return          ;};
    double            GetTLPD      ( void                     ) {                       return TLPD     ;};
    void              SetIsIQ      ( void                     ) {isIQ = true;           return          ;};
    void              SetNotIQ     ( void                     ) {isIQ = false;          return          ;};
    bool              GetIsIQ      ( void                     ) {                       return isIQ     ;};
    void              SetResident  ( bool     i_res           ) {res = i_res; return          ;};
    bool              GetResident  ( void                     ) {                       return res ;};
    //info
    double          **GetAry       ( void                     ) {                       return ary      ;};  //!< return <ch> pointers to double* arrays bases
  private:
    //OCD
    void              RemoveDC     ( void                     );
    //Infrastructure
  public:
  private:
    // The signal's composition
    double        **ary     ;
    llong           ch      ; //!<Number of Channels
    double          FS      ; //!<Sample rate in Hz.
    llong           N       ; //!<Length of stream as number of frames
    llong           size    ; //!<Size of allocated space

    // Meta-information about it's nature.
    bool            isIQ    ; //!<Came from a demodulator with I & Q
    double          TLPV    ; //!<Outside world Voltage of the TLP.
    double          TLPD    ; //!<double domain number of the TLP. Normally 1.0

    // Transient information about its immediate state
    bool            res; //!<It has been transferred in.
    llong           posFrames;//!<The current position, in frames, that we will grab from
  };

#endif // WAVEIN_H_
