
//=================================================================================================
// Original File Name : WaveFmt.hpp
// Original Author    : duncang
// Creation Date      : Sep 4, 2015
// Copyright          : Copyright © 2015 - 2022 by Catraeus and Duncan Gray
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
    Helper class to encapsulate things about how the fmt_ subchunk works.
*/
//=================================================================================================

#ifndef __WAVE_FMT_HPP_
#define __WAVE_FMT_HPP_

//#include <map>
#include <string>

#include <caesmusfil/Sig/NumSys.hpp>
#include <caesmusfil/RIFF/wav/WaveGUID.hpp>

class WaveFmt {
  private:
  public:
    enum eConst {
      WFC_FMT_16 = 16,
      WFC_FMT_18 = 18,
      WFC_FMT_40 = 40
      };
    enum eFmtTag { // MAGIC The order of this must be kept strict to the list sFmtListSpec in the static instance.
      WF_UNK        =  0, // 0x0000
      WF_PCM        =  1, // 0x0001
      WF_ADPCM_MS   =  2, // 0x0002
      WF_IEEE_FLOAT =  3, // 0x0003
      WF_ALAW       =  4, // 0x0006
      WF_ULAW       =  5, // 0x0007
      WF_ADPCM_IMA  =  6, // 0x0011
      WF_MP3        =  7, // 0x0055
      WF_EXTENSIBLE =  8, // 0xfffe
      WF_EXPMTL     =  9, // 0xffff
      WF_COUNT_OF   = 10
      };
  private:
    struct sFmtListSpec {
      eFmtTag index;
      uint    tag;
      char   *name;
      };
    static const sFmtListSpec fmtListSpec[];
  public:
    enum eFmtType {
      FT_16,
      FT_18,
      FT_40,
      FT_UNK
      };
  public:
                     WaveFmt      (NumSys *i_tns);
    virtual         ~WaveFmt      (    );
            void     Reset        (void);
            void     Analyze      (byte *i_pFmtImg, long i_size);
            void     Build        (byte *i_pFmtImg, llong i_ch, double i_FS  );

    const   eFmtTag  GetTag       (void)             {return fmtTag;                       };
    const   ullong   GetTagDex    (void)             {return (ullong)fmtTag;               };
    const   char    *GetTagName   (eFmtTag i_fmtTag) {return fmtListSpec[i_fmtTag].name;   };
    static const   char    *GetTagName   (ullong  i_fmtDex) {return fmtListSpec[i_fmtDex].name;   };
    const   char    *GetTagName   (void)             {return fmtListSpec[fmtTag].name;     };
    const   uint     GetTagCode   (void)             {return fmtTagFound;                  };
    static const   uint     GetTagCode   (ullong i_fmtDex) {return fmtListSpec[i_fmtDex].tag;     };
    static const   ullong   GetTagCount  (void)             {return (ullong)WaveFmt::WF_COUNT_OF; };
    const   long     GetNumCh     (void)             {return ch;                           };
    const   double   GetFS        (void)             {return FS;                           };
    const   double   GetByteVSec  (void)             {return byteVSec;                     };
    const   long     GetBlkAlign  (void)             {return blkAlign;                     };
    const   long     GetBitVSmp   (void)             {return bitVSmp;                      };
    const   long     GetExtSize   (void)             {return extSize;                      };
    const   long     GetVBitVSmp  (void)             {return vBitVSmp;                     };
    const   ulong    GetChMask    (void)             {return chMask;                       };
    const   char    *GetGUIDPreso (void)             {return GUID->GetPreso();             };
    const   char    *GetGUIDname  (void)             {return GUID->GetName();              };

            ullong   SetCh        (ullong  i_ch    );
            void     SetFmtTag    (eFmtTag i_tag   );
            ullong   SetBitDepth  (ullong  i_depth );
            double   SetFS        (double  i_FS    );

            llong    GetSize      (void            ) {return size;};
  private:
  public:
    /// The way to point to a memory region that has the raw data to bring into the class for later digestion.
    struct __attribute__((__packed__)) sWaveFmt {
      /// Enum that tells us which format we have on our hands.  rfc2361
      ushort fmtTag;
      /// The number of channels.
      ushort ch;
      /// INTEGER sample rate ... what were they thinking! (No that's not a QUESTION!
      uint  FS;
      /// Better be fmtFS * blockAlign.
      uint  byteVSec;
      /// This is redundant too, for uncompressed encodings. Must be ceil(bitDepth / 8) * fmtCh., else compressed.
      ushort blkAlign;
      /// The number of actual bits in the "blockAlign / fmtCh" bytes that are meaningful.
      ushort bitVSmp;
      //====
      /// 0 if extension size is zero.  22 if there is a GUID etc.   Who writes this stuff?
      ushort extSize;
      //====
      /// So that you can have bytes serialized for weird length PCM words etc (Cell Phones are 11 bits?)
      ushort vBitVSmp;
      /// Some abortive attempt at speaker location enumeration since the world has a uniform standard of speaker placement.
      uint  chMask;
      /// Finally, some sense to it all ... oh wait ... no
      byte   GUID[16];
      };
  private:
    eFmtTag   fmtTag;  // Only ever keep this, never the string name of it.  Use the string getter instead.
    uint      fmtTagFound;
    eFmtType  fmtType;
    sWaveFmt *pFmtImg;
    long      ch;
    double    FS;
    double    byteVSec;
    long      blkAlign;
    long      bitVSmp;
    //====
    long      extSize;
    //====
    long      vBitVSmp;
    uint      chMask;
    NumSys   *tns;
    WaveGUID *GUID;
    llong     size;

    //====
  private:

  };


#endif // __WAVE_FMT_HPP_
