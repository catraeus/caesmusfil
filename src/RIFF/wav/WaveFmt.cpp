
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
   See the header file for this source file for a detailed description.
*/
//=================================================================================================

#include <caesbase/CaesTypes.hpp>

#include <caesmusfil/RIFF/wav/WaveFmt.hpp>

const WaveFmt::sFmtListSpec WaveFmt::fmtListSpec[] = { // MAGIC This is in order based on the enum assignments, do not reorder.
    {WF_UNK,        0x00000000, (char *)"Unknown"           }, // Only for internal processing.  Don't let this show up in a file, but remember Postel's Law
    {WF_PCM,        0x00000001, (char *)"PCM"               }, // Fundimental mathematics, can't do better.
    {WF_ADPCM_MS,   0x00000002, (char *)"ADPCM, Microsoft®" }, // From when MS was young and foolish and thougt they could specify transport things.
    {WF_IEEE_FLOAT, 0x00000003, (char *)"IEEE Float"        }, // Venerable, still works after all these decades
    {WF_ALAW,       0x00000006, (char *)"A-Law, ITU G.711"  }, // Bell Labs (blessed be thy name) invented it, and europe had to scent mark.
    {WF_ULAW,       0x00000007, (char *)"μ-Law, ITU G.711"  }, // From ancient times (1950's) when they were really stressed for data rate.
    {WF_ADPCM_IMA,  0x00000011, (char *)"ADPCM, IBM® IMA"   }, // From the heady days of speech compression, laughable in the days of 10GbE!
    {WF_MP3,        0x00000055, (char *)"MPEG Layer 3"      }, // Another robust stable format, latter day IEEE Float for lossy worlds
    {WF_EXTENSIBLE, 0x0000FFFE, (char *)"WaveFmt Extensible"}, // Because even Windoze developers can do better.
    {WF_EXPMTL,     0x0000FFFF, (char *)"Experimental"      }, // Hopefully never makes it out of the lab.
  };


     WaveFmt::WaveFmt(NumSys *i_tns)
: tns(i_tns) {
  GUID    = new WaveGUID();
  Reset();
  }
     WaveFmt::~WaveFmt(){
  }
void WaveFmt::Analyze(byte *i_pFmtImg, long i_size) {

  pFmtImg = (sWaveFmt *)i_pFmtImg;
  fmtTagFound = pFmtImg->fmtTag;
  fmtTag = WF_UNK;
  for(int i=0; i<WF_COUNT_OF; i++) { // If we find an unenumerated tag, treat as if a 16-byte and inform the user somehow upstairs
    if(fmtTagFound == fmtListSpec[i].tag) {
      fmtTag  = fmtListSpec[i].index;
      i       = WF_COUNT_OF;
      }
    }
  ch       = (long)pFmtImg->ch;
  FS       = (double)pFmtImg->FS;
  byteVSec = (double)pFmtImg->byteVSec;
  blkAlign = (long)pFmtImg->blkAlign;
  bitVSmp  = (long)pFmtImg->bitVSmp;
       if(i_size == WFC_FMT_16) {
    }
  else if(i_size == WFC_FMT_18) {
    extSize = pFmtImg->extSize;
    return;
    }
  else if(i_size == WFC_FMT_40) {
    extSize  = pFmtImg->extSize;
    vBitVSmp = pFmtImg->vBitVSmp;
    chMask   = pFmtImg->chMask;
    GUID->SetID(pFmtImg->GUID);
    }
  else
    return;
  tns->SetBitDepth(bitVSmp);
  if(fmtTag == WF_IEEE_FLOAT)
    tns->SetField(NumSys::EF_REAL);
  else if(fmtTag == WF_PCM)
    tns->SetField(NumSys::EF_INTEGER);
  else if(fmtTag == WF_EXTENSIBLE) {
    WaveGUID::GRec *gg = WaveGUID::gList;
    WaveGUID::GRec *gr;
    WaveGUID::GUID *lg;
    byte *tbf;
    byte *tbp;

    gr  = &gg[WaveGUID::EG_IEEE_FLOAT];
    lg  = &(gr->id);
    tbf = &(lg->bytes[0]);
    gr  = &gg[WaveGUID::EG_PCM];
    lg  = &(gr->id);
    tbp = &(lg->bytes[0]);
    if(GUID->Equals(tbf)   )
      tns->SetField(NumSys::EF_REAL);
    if(GUID->Equals(tbp)   )
      tns->SetField(NumSys::EF_INTEGER);
    else
      tns->SetField(NumSys::EF_INVALID);
  }
  else
    tns->SetField(NumSys::EF_INVALID);
  return;
  }
void WaveFmt::Build(byte *i_pImg, llong i_ch, double i_FS) {

  pFmtImg  = (sWaveFmt *)i_pImg;
  fmtTag   = WF_PCM;
  pFmtImg->fmtTag   = (ushort)WF_PCM;
  ch       = pFmtImg->ch       = i_ch;
  FS       = pFmtImg->FS       = (uint)i_FS;
  bitVSmp  = pFmtImg->bitVSmp  = 32;
  blkAlign = pFmtImg->blkAlign = bitVSmp / 8 * ch;
  byteVSec = pFmtImg->byteVSec = blkAlign * FS;
  size     = 16;
  if(fmtTag == WF_IEEE_FLOAT)
    tns->SetField(NumSys::EF_REAL);
  else if(fmtTag == WF_PCM)
    tns->SetField(NumSys::EF_INTEGER);
  else if(fmtTag == WF_EXTENSIBLE) {
    WaveGUID::GRec *gg = WaveGUID::gList;
    WaveGUID::GRec gr;
    WaveGUID::GUID lg;
    byte *tbf;
    byte *tbp;

    gr = gg[WaveGUID::EG_IEEE_FLOAT];
    lg = gr.id;
    tbf = lg.bytes;
    gr = gg[WaveGUID::EG_PCM];
    lg = gr.id;
    tbp = lg.bytes;
    if(GUID->Equals(tbf)   )
      tns->SetField(NumSys::EF_REAL);
    if(GUID->Equals(tbp)   )
      tns->SetField(NumSys::EF_REAL);
    else
      tns->SetField(NumSys::EF_INVALID);
    size = 44;
  }
  else
    tns->SetField(NumSys::EF_INVALID);
  return;
}
void WaveFmt::Reset(void) {
  fmtTag     = WF_UNK;
  fmtType    = FT_UNK;
  pFmtImg    = 0;
  ch         = 1;
  FS         = 1.0;
  byteVSec   = 4.0;
  blkAlign   = 4;
  bitVSmp    = 32;
  //====
  extSize    = 0;
  //====
  vBitVSmp   = 32;
  chMask     = 0xffffffff;
  byte blob[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  GUID->SetID(blob);

  //====
  return;
  }

ullong WaveFmt::SetCh(ullong i_ch) {
  ch          = i_ch > 4 ? 4 : i_ch;
  blkAlign    = ch * bitVSmp / 8;
  byteVSec    = blkAlign * FS;

  pFmtImg->ch       = (ushort)ch;
  pFmtImg->blkAlign = (ushort)blkAlign;
  pFmtImg->byteVSec = (uint)  byteVSec;

  return ch;
}
void  WaveFmt::SetFmtTag(eFmtTag i_tag) {
  if(i_tag == WF_IEEE_FLOAT) {
    bitVSmp = bitVSmp - (bitVSmp % 32);
    if(bitVSmp != 64)
      bitVSmp = 8;
  }
  else {
    i_tag = WF_PCM;
    bitVSmp = bitVSmp - (bitVSmp % 8);
  }
  fmtTag = i_tag;
  blkAlign    = ch * bitVSmp / 8;
  byteVSec    = blkAlign * FS;

  pFmtImg->blkAlign = (ushort)blkAlign;
  pFmtImg->byteVSec = (uint)  byteVSec;
  pFmtImg->bitVSmp  = (ushort)bitVSmp;
  if(bitVSmp > 64)
    bitVSmp = 64;

  return;
}
ullong WaveFmt::SetBitDepth(ullong i_depth) {
  i_depth &= 0x00000000000000f8; // pretty tricksy, eh.  mod 8
  if(i_depth > 64)
    i_depth = 64;
  bitVSmp     = i_depth;
  blkAlign    = ch * bitVSmp / 8;
  byteVSec    = blkAlign * FS;

  pFmtImg->blkAlign = (ushort)blkAlign;
  pFmtImg->byteVSec = (uint)  byteVSec;
  pFmtImg->bitVSmp  = (ushort)bitVSmp;
  return bitVSmp;
}
double WaveFmt::SetFS(double i_FS) {
  uint iFS;
       if(i_FS > 10000000.0) i_FS = 10000000.0;
  else if(i_FS <        0.0) i_FS =        0.0;
  iFS = (uint)(i_FS + DBL_ROUNDER);
  FS = (double)iFS;
  pFmtImg->FS = iFS;
  byteVSec    = blkAlign * FS;
  pFmtImg->byteVSec = (uint)  byteVSec;
  return FS;
}
