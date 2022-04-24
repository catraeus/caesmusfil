
//=================================================================================================
// Original File Name : FileWave.cpp
// Original Author    : duncang
// Creation Date      : March 17, 2002
// Copyright          : Copyright © 2002 - 2022 by Catraeus and Duncan Gray
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

#include <string.h>
#include <stdio.h>

#include <caesmusfil/RIFF/wav/WaveGUID.hpp>

WaveGUID::GRec *WaveGUID::gList  = 0;
     WaveGUID::WaveGUID() { // Ayep, 16 bytes is 128 bits.  It's a GUID.
  byte TB_UNK       [] = {0x00,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_PCM       [] = {0x01,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_ADPCM_MS  [] = {0x02,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_IEEE_FLOAT[] = {0x03,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_CVSD      [] = {0x05,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_ALAW      [] = {0x06,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_ULAW      [] = {0x07,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_ADPCM_IMA [] = {0x11,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  byte TB_MP3       [] = {0x55,0x00,0x00,0x00, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71};
  if(gList == 0) {
   gList = new GRec[EG_COUNT_OF];
   GRec *tG;
   tG = &(gList[EG_UNK       ]); BuildGRec(tG, EG_UNK        , TB_UNK        , (char *)"Unknown"                );
   tG = &(gList[EG_PCM       ]); BuildGRec(tG, EG_PCM        , TB_PCM        , (char *)"PCM, Linear"            );
   tG = &(gList[EG_ADPCM_MS  ]); BuildGRec(tG, EG_ADPCM_MS   , TB_ADPCM_MS   , (char *)"ADPCM - Microsoft®"     );
   tG = &(gList[EG_IEEE_FLOAT]); BuildGRec(tG, EG_IEEE_FLOAT , TB_IEEE_FLOAT , (char *)"IEEE floating point"    );
   tG = &(gList[EG_CVSD      ]); BuildGRec(tG, EG_CVSD       , TB_CVSD       , (char *)"CVSD - IBM®"            );
   tG = &(gList[EG_ALAW      ]); BuildGRec(tG, EG_ALAW       , TB_ALAW       , (char *)"PCM, ITU-T G.711 A-Law" );
   tG = &(gList[EG_ULAW      ]); BuildGRec(tG, EG_ULAW       , TB_ULAW       , (char *)"PCM, ITU-T G.711 µ-Law" );
   tG = &(gList[EG_ADPCM_IMA ]); BuildGRec(tG, EG_ADPCM_IMA  , TB_ADPCM_IMA  , (char *)"ADPCM, IMA - IBM®"      );
   tG = &(gList[EG_MP3       ]); BuildGRec(tG, EG_MP3        , TB_MP3        , (char *)"MPEG Layer 3"           );
  }
  gRec.name     = new char[EC_NAME_SIZE];
  gRec.preso    = new char[EC_NAME_SIZE];
  byte tGa[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
  SetID(tGa);
  ToString(&(gRec.id), gRec.preso);
}
     WaveGUID::~WaveGUID() {
  delete gRec.name;
  delete gRec.preso;
  for(uint i=0; i<EG_COUNT_OF; i++) {
    delete gList[i].name;
    delete gList[i].preso;
  }
  delete gList;
  gList = 0;
  }
void WaveGUID::BuildGRec(GRec *io_gRec, eType i_type, byte *i_id, char *i_name) {
  io_gRec->type = i_type;
  for(uint i=0; i<EC_ID_SIZE; i++)
    io_gRec->id.bytes[i] = i_id[i];
  io_gRec->name  = new char[EC_NAME_SIZE];
  io_gRec->preso = new char[EC_NAME_SIZE];
  ToString(&(io_gRec->id), io_gRec->preso);
  strcpy(io_gRec->name, i_name);
  return;
}
void WaveGUID::SetID(byte *i_id) {
  bool gotIt;
  uint i;

  for(uint i=0; i<EC_ID_SIZE; i++)
    gRec.id.bytes[i] = i_id[i];
  ToString((GUID *)i_id, gRec.preso);
  for(i=0; ((i<EG_COUNT_OF) && (!gotIt)); i++) {
    for(uint j=0; j<EC_ID_SIZE; j++) {
      if(gList[i].id.bytes[j] !=i_id[j]) {
        gotIt = true;
        j = EC_ID_SIZE;
      }
    }
  }
  if(gotIt) {
    gRec.type = gList[i].type;
    strcpy(gRec.name,  gList[i].name);
  }
  else {
    gRec.type = EG_UNK;
    strcpy(gRec.name, gList[EG_UNK].name);
  }
  return;
}
bool WaveGUID::Equals(byte *i_id) {
  for(uint i=0; i<EC_ID_SIZE; i++)
    if(i_id[i] != gRec.id.bytes[i])
      return false;
  return true;
}
void WaveGUID::ToString(GUID *i_id, char *s) {
  long    i;
  char    t[256];

  s[0] = '\0';                                     strcat(s, "{");
  sprintf(t, "%08x", i_id->s.dd);    strcat(s, t); strcat(s, "-");
  sprintf(t, "%04x", i_id->s.d1);    strcat(s, t); strcat(s, "-");
  sprintf(t, "%04x", i_id->s.d2);    strcat(s, t); strcat(s, "-");
  for(i =  8; i < 10; i++) {
    sprintf(t, "%02x", i_id->bytes[i]);
    strcat(s, t);
  }
  strcat(s, "-");

  for(i = 10; i < 16; i++) {
    sprintf(t, "%02x", i_id->bytes[i]);
    strcat(s, t);
  } strcat(s, "}");
}
/*
void WaveGUID::ByteToHexString(char *t, byte i) {
  unsigned char j;
  j = i & 0x0f;
  if(j < 10) t[1] = '0' + j;
  else       t[1] = 'A' + j - 10;
  j = (i & 0xf0) >> 4;
  if(j < 10) t[0] = '0' + j;
  else       t[0] = 'A' + j - 10;
  t[2] = '\0';
  }
*/
