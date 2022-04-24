
//=================================================================================================
// Original File Name : RiffCk.cpp
// Original Author    : duncang
// Creation Date      : Sep 28, 2013
// Copyright          : Copyright © 2013 - 2022 by Catraeus and Duncan Gray
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
#include <stddef.h>

#include <caesmusfil/RIFF/RiffCk.hpp>

RiffCk::sCk4CCSpec *RiffCk::ck4CCSpec = NULL;
void    RiffCk::BuildDecoder(void) {
  if(ck4CCSpec != NULL) return;
  uint i;
  sCk4CCSpec *tS;
  ck4CCSpec = new sCk4CCSpec[ERF_COUNT_OF];
  for(i=0; i<ERF_COUNT_OF; i++) ck4CCSpec[i].form = new char[  5];
  for(i=0; i<ERF_COUNT_OF; i++) ck4CCSpec[i].name = new char[256];
  i = 0;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_RIFF; tS->valid =  false; strcpy(tS->form, (char *)"RIFF"); strcpy(tS->name, (char *)""); tS->creator = &CkRIFF::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_WAVE; tS->valid =  false; strcpy(tS->form, (char *)"WAVE"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_FLAC; tS->valid =  false; strcpy(tS->form, (char *)"fLaC"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_bext; tS->valid =  true;  strcpy(tS->form, (char *)"bext"); strcpy(tS->name, (char *)""); tS->creator = &CkBext::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_fmt_; tS->valid =  true;  strcpy(tS->form, (char *)"fmt "); strcpy(tS->name, (char *)""); tS->creator = &CkFmt ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_fact; tS->valid =  true;  strcpy(tS->form, (char *)"fact"); strcpy(tS->name, (char *)""); tS->creator = &CkFact::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_PAD_; tS->valid =  true;  strcpy(tS->form, (char *)"PAD "); strcpy(tS->name, (char *)""); tS->creator = &CkPAD ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_JUNK; tS->valid =  true;  strcpy(tS->form, (char *)"JUNK"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_cue_; tS->valid =  true;  strcpy(tS->form, (char *)"cue "); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_wavl; tS->valid =  true;  strcpy(tS->form, (char *)"wavl"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_data; tS->valid =  true;  strcpy(tS->form, (char *)"data"); strcpy(tS->name, (char *)""); tS->creator = &CkData::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_afsp; tS->valid =  true;  strcpy(tS->form, (char *)"afsp"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_PEAK; tS->valid =  true;  strcpy(tS->form, (char *)"PEAK"); strcpy(tS->name, (char *)""); tS->creator = &CkPEAK::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_slnt; tS->valid =  true;  strcpy(tS->form, (char *)"slnt"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_list; tS->valid =  true;  strcpy(tS->form, (char *)"list"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_info; tS->valid =  true;  strcpy(tS->form, (char *)"info"); strcpy(tS->name, (char *)""); tS->creator = &CkInfo::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_labl; tS->valid =  true;  strcpy(tS->form, (char *)"labl"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_note; tS->valid =  true;  strcpy(tS->form, (char *)"note"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_ltxt; tS->valid =  true;  strcpy(tS->form, (char *)"ltxt"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_plst; tS->valid =  true;  strcpy(tS->form, (char *)"plst"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_smpl; tS->valid =  true;  strcpy(tS->form, (char *)"smpl"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_inst; tS->valid =  true;  strcpy(tS->form, (char *)"inst"); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  tS = &(ck4CCSpec[i]); tS->formNo = ERF_Unk_; tS->valid =  true;  strcpy(tS->form, (char *)"...."); strcpy(tS->name, (char *)""); tS->creator = &CkUnk ::Create; i++;
  for(i=0; i<ERF_COUNT_OF; i++) ck4CCSpec[i].tag = (*(uint *)(ck4CCSpec[i].form));
}

        RiffCk::RiffCk       ( byte *i_pHdr, eDir i_dir, NumSys *i_tns )
: tns(i_tns)  {
  RiffCk::BuildDecoder();
  ResetCk();
  uint d;
  dir = i_dir;
  img      = i_pHdr;
  pConts   = img + (size_t)(FORM_SIZE + SIZE_SIZE);
  pSize    = (uint *)(img + (size_t) FORM_SIZE);
  if(dir == ED_READ) {
    isValid  = false;
    FormExtrct(form, img);
    d        = *((uint *)(img + FORM_SIZE));
    size     = d;
    imgSize  = d%2 == 0 ? d : d + 1;
  }
  else
    BlankBody();
}
        RiffCk::~RiffCk      ( void         ) {
  }
void    RiffCk::ResetCk      ( void         ) {
  img      =   0 ;
  form[0]  = '\0';
  tag      =   0 ;
  size     =   0 ;
  imgSize  =   0 ;
  res      =   0 ;

  isValid  = false;

  isHier   = false;
  subSize  =   0 ;
  order    =   0 ;
  pParent  =   0 ;
  pChild   =   0 ;
  pPred    =   0 ;
  pSucc    =   0 ;
  return;
  }
void    RiffCk::FormExtrct   ( char *o_c, byte *i_b) {
  o_c[0] = i_b[0];
  o_c[1] = i_b[1];
  o_c[2] = i_b[2];
  o_c[3] = i_b[3];
  o_c[4] = '\0';
  return;
  }
void    RiffCk::FormPlace    ( byte *o_b, eForm i_f ) {
  for(uint i=0; i<FORM_SIZE; i++) o_b[i] = ck4CCSpec[i_f].form[i];
  return;
}
void    RiffCk::FormWrite    ( char *o_c, eForm i_f ) {
  uint i;
  for(i=0; i<FORM_SIZE; i++) o_c[i] = (char)ck4CCSpec[i_f].form[i];
  o_c[i] = '\0';
  return;
}
RiffCk::eForm RiffCk::FormIntrp    ( byte *i_b ) {
  char  tStr[5];
  eForm tForm;
  FormExtrct(tStr, i_b);
  tForm = ERF_Unk_;
  for(ullong i=0; i<ERF_COUNT_OF; i++) {
    if(strcmp(tStr, ck4CCSpec[i].form) == 0) {
      tForm = ck4CCSpec[i].formNo;
      i = ERF_COUNT_OF;
    }
  }
  return tForm;
}

RiffCk *RiffCk::RiffFactory  ( byte *i_pHdr, NumSys *i_ctns ) {
  RiffCk *pCk;
  char    tHdr[5];

  RiffCk::BuildDecoder();
  pCk = 0;
  FormExtrct(tHdr, i_pHdr);
  //gotIt = false;
  for(uint i=0; i<ERF_COUNT_OF; i++) {
    if(strcmp(tHdr, ck4CCSpec[i].form) == 0) {
      if(ck4CCSpec[i].creator != 0)
        pCk = ck4CCSpec[i].creator(i_pHdr, ED_READ, i_ctns);
      i = ERF_COUNT_OF;
    }
  }
  if(pCk == 0)
    pCk = CkUnk::Create(i_pHdr, ED_READ, i_ctns);
  return pCk;
}
void    RiffCk::ParseBody    ( void         ) {
  return;
  }
void    RiffCk::BlankBody    ( void         ) { // Do nothing because this is the virtualizer.
  return;
  }






//==========  .RIFF.  =============================================================================================================
        CkRIFF::CkRIFF       ( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  isHier   = true;
  size     = 4;
  if(dir == ED_WRITE)
    BlankBody();
  return;
}
        CkRIFF::~CkRIFF      ( void         ) {
}
void    CkRIFF::ParseBody    ( void         ) {
  byte *pForm;

  subSize = *((uint *)(img + FORM_SIZE)) - FORM_SIZE;
  pForm = img + FORM_SIZE + SIZE_SIZE;
//  FormExtrct(form,     pForm);
  FormExtrct(formWave, pForm);
  if(strcmp(formWave, ck4CCSpec[ERF_WAVE].form) == 0)
    isValid = true;
  else
    isValid = false;
  return;
}
void    CkRIFF::BlankBody    ( void         ) {
  byte *pOff;
  int  *pSize;
  pOff = img;
  RiffCk::FormPlace(pOff, ERF_RIFF);
  pOff += FORM_SIZE;
  pSize = (int *)pOff;
  *pSize = 0;
  pOff += SIZE_SIZE;
  RiffCk::FormPlace(pOff, ERF_WAVE);
  RiffCk::FormWrite(form, ERF_WAVE);
  return;
}



//=========== .bext.  ==============================================================================================================
        CkBext::CkBext       ( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  return;
}
void    CkBext::ParseBody    ( void         ) {
  return;
}




//==========  .fmt .  ==============================================================================================================
        CkFmt::CkFmt             ( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  wf             = new WaveFmt(tns);
  pFmtStruct     = NULL;
  pFmt16Struct   = NULL;
  pFmt18Struct   = NULL;
  pFmt40Struct   = NULL;
  fmtCode        = WaveFmt::WF_PCM;
  fmtCodeStr     = NULL;
  fmtFS          =  1.0;
  fmtCh          =  2;
  fmtBitDepth    = 32;
  fmtBlkAlign    =  4;
  fmtByteRate    =  8;
  fmtExtSize     =  0;
  fmtExtBitDepth = 32;
  fmtExtChMask   = 0xffffffff;
  fmtGUID        = NULL;
  pGUID          = NULL;
  return;
}
void    CkFmt::ParseBody         ( void                      ) {
  //=======================================
  // Point to the format image in memory.
  pConts = (byte *)(img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE);
  wf->Analyze(pConts, size);
  isValid = true;
  return;
}
void    CkFmt::BlankBody         ( llong         i_ch, double i_FS   ) {
  byte *pOff;
  pOff = img;
  RiffCk::FormPlace(pOff, ERF_fmt_);
  pOff += FORM_SIZE;
  *pSize = 16;
  size = 16;
  pOff += SIZE_SIZE;
  wf->Build(pOff, i_ch, i_FS);
  size = wf->GetSize();
  return;
}
ullong  CkFmt::SetCh             ( ullong        i_ch        ) {
  return wf->SetCh(i_ch);
}
void    CkFmt::SetType           ( NumSys::eType i_type      ) {
  WaveFmt::eFmtTag fmtTag;
  uint bitDepth;
  switch(i_type) {
    case NumSys::ET_INT08:
      fmtTag    = WaveFmt::WF_PCM;
      bitDepth  =  8;
      break;
    case NumSys::ET_INT16:
      fmtTag    = WaveFmt::WF_PCM;
      bitDepth  = 16;
      break;
    case NumSys::ET_INT24:
      fmtTag    = WaveFmt::WF_PCM;
      bitDepth  = 24;
      break;
    case NumSys::ET_INT32:
      fmtTag    = WaveFmt::WF_PCM;
      bitDepth  =  32;
      break;
    case NumSys::ET_FLOAT:
      fmtTag    = WaveFmt::WF_IEEE_FLOAT;
      bitDepth  =  32;
      break;
    case NumSys::ET_DOUBLE:
      fmtTag    = WaveFmt::WF_IEEE_FLOAT;
      bitDepth  =  64;
      break;
    default:
      fmtTag    = WaveFmt::WF_PCM;
      bitDepth  =  32;
      break;
  }
  wf->SetFmtTag(fmtTag);
  wf->SetBitDepth(bitDepth);
  return;
}
double  CkFmt::SetFS             ( double        i_FS        ) {
  return wf->SetFS(i_FS);
}





//==========  .data.  =============================================================================================================
        CkData::CkData           ( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  return;
}
void    CkData::ParseBody        ( void      ) {
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;
  return;
}
void    CkData::BlankBody        ( void      ) {
  byte *pOff;
  int  *pSize;
  pOff = img;
  RiffCk::FormPlace(pOff, ERF_data);
  pOff += FORM_SIZE;
  pSize = (int *)pOff;
  *pSize = 0;

  return;
}
void    CkData::SetSize  (uint i_b) {
  uint *p;
  p = (uint *)(img + FORM_SIZE);
  if((i_b & 0x00000001) != 0) i_b++;
  *p = i_b;
  size = i_b;
  return;
}


//=========  .afsp.  ==============================================================================================================
        CkAfsp::CkAfsp( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  isHier   = false;
  int d    = *((uint *)(i_pHdr + FORM_SIZE));
  size     = d%2 == 0 ? d : d + 1;
  img      = new byte[FetchSize()];
  return;
}
void    CkAfsp::ParseBody(void) {
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;
  return;
}



//=========  .PEAK.  ==============================================================================================================
        CkPEAK::CkPEAK( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns)
, peakInfo ( NULL) {
  llong ddd;
  int     d;
  isHier   = false;
  d        = *((uint *)(i_pHdr + FORM_SIZE));
  size     = d%2 == 0 ? d : d + 1;
  ddd      = FetchSize();
  img      = new byte[ddd];
  return;
}
void    CkPEAK::ParseBody(void) {
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;
  peakInfo = (sPeak *)pConts;
  return;
}



//==========  .fact.  =============================================================================================================
        CkFact::CkFact( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  uint d;
  isHier   = false;
  img      = i_pHdr;
  d        = *((uint *)(img + FORM_SIZE));
  size     = d;
  imgSize  = d%2 == 0 ? d : d + 1;
  nFrm = 0;
//img      = new byte[FetchSize()];
  return;
}
void    CkFact::ParseBody(void) {
  nFrm = 0;
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;

  //=======================================
  // Get the alleged number of frames.
  nFrm = (llong)(*(uint *)pConts);
  return;
}



//=========  .PAD .  ==============================================================================================================
        CkPAD::CkPAD( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  uint d;
  isHier   = false;
  img      = i_pHdr;
  d        = *((uint *)(img + FORM_SIZE));
  size     = d;
  imgSize  = d%2 == 0 ? d : d + 1;
//img      = new byte[FetchSize()];
  return;
}
void    CkPAD::ParseBody(void) {
  //=======================================
  // Point to the contents for future generations.
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;
  return;
}



//==========  .Info.  =============================================================================================================
        CkInfo::CkInfo( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {// FIXME - An Info chunk might be hierarchical.
  uint d;
  isHier   = false;
  img      = i_pHdr;
  d        = *((uint *)(img + FORM_SIZE));
  size     = d;
  imgSize  = d%2 == 0 ? d : d + 1;
//img      = new byte[FetchSize()];
  return;
}
void    CkInfo::ParseBody(void) {
  //=======================================
  // Point to the contents for future generations.
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;
  return;
}



//=======   <unknown>  ============================================================================================================
        CkUnk::CkUnk( byte *i_pHdr, eDir i_dir, NumSys *i_tns   ) : RiffCk::RiffCk(i_pHdr, i_dir, i_tns) {
  uint d;
  isHier   = false;
  img      = i_pHdr;
  d        = *((uint *)(img + FORM_SIZE));
  size     = d;
  imgSize  = d%2 == 0 ? d : d + 1;
//img      = new byte[FetchSize()];
  return;
}
void    CkUnk::ParseBody(void) {
  pConts = img + (size_t)FORM_SIZE + (size_t)SIZE_SIZE;
  return;
}
