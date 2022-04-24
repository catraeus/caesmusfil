
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

#include <malloc.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#include <caesbase/CaesString.hpp>
#include <caesbase/CaesTypes.hpp>

#include <caesmusfil/RIFF/wav/WaveFiler.hpp>

const char               WaveFiler::hdTstRiff[]     = "RIFF";
const char               WaveFiler::hdTstMp3[]      = "ID3";
const char               WaveFiler::hdTstFlac[]     = "fLaC";
WaveFiler::sFileSpec   **WaveFiler::fileSpecList    =  NULL;
//=================================================================================================
            WaveFiler::WaveFiler         ( Signal *i_sig ) {
  sig = i_sig;
  tns = new NumSys();
  taf = new AudioFiler();
  trf = new RiffMgr(taf, tns);
  if(fileSpecList == NULL) {
    fileSpecList = new sFileSpec*[numFileTypes];
    for(uint i=0; i<numFileTypes; i++)
      fileSpecList[i] = new sFileSpec;
    fileSpecList[0]->match = (char *)hdTstRiff; fileSpecList[0]->len = strlen(fileSpecList[0]->match);  fileSpecList[0]->cb = 0;
    fileSpecList[1]->match = (char *)hdTstMp3;  fileSpecList[1]->len = strlen(fileSpecList[1]->match);  fileSpecList[1]->cb = 0;
    fileSpecList[2]->match = (char *)hdTstFlac; fileSpecList[2]->len = strlen(fileSpecList[2]->match);  fileSpecList[2]->cb = 0;
  }
  Reset();
}
            WaveFiler::~WaveFiler        (void) {
  if( fileSpecList            != 0 ) {
    for(uint i=0; i<numFileTypes; i++)
      delete fileSpecList[i];
                                       delete fileSpecList            ; fileSpecList              = 0; }
  fprintf(stdout, "WaveFile destructor\n");
}
void        WaveFiler::OnFileChange      (void) {
  double   FS;
  ullong   NN;
  ullong   cc;
  ullong   bb;
  char    *tn;
  char    *GP;
  char    *GN;
  WaveFmt::eFmtTag tt;

  FS = trf->FmtGetFS();
  bb = trf->FmtGetBitDepth() / 8;
  if(bb == 0)
    bb = 1;
  cc = trf->FmtGetCh();
  tn = (char *)trf->FmtGetTagName();
  tt = trf->FmtGetTag();
  if((tt == WaveFmt::WF_EXTENSIBLE) && (trf->FmtGetSize() > 20)) {
    NN = trf->FactGetNFrm();
    GP = (char *)trf->FmtGetWaveGUID();
    GN = (char *)trf->FmtGetGUIDname();
  }
  else {
    NN = trf->DataGetSize() / bb / cc;
  }
  fprintf(stdout, "           FS: %lf\n" , FS);fflush(stdout);
  fprintf(stdout, "           ch: %lld\n", cc);fflush(stdout);
  fprintf(stdout, "            N: %lld\n", NN);fflush(stdout);
  fprintf(stdout, "     WordSize: %lld\n", bb);fflush(stdout);
  fprintf(stdout, "      TagName: %s\n"  , tn);fflush(stdout);
  tns->SetByteDepth(bb);
  if((tt == WaveFmt::WF_EXTENSIBLE) && (trf->FmtGetSize() > 20)) {
    fprintf(stdout, "       GUID_Hex: %s\n", GP);fflush(stdout);
    fprintf(stdout, "      GUID_Name: %s\n", GN);fflush(stdout);
    if(strcmp("PCM",trf->FmtGetGUIDname()) == 0)
      tns->SetField(NumSys::EF_INTEGER);
    else if(strcmp("IEEE Float",trf->FmtGetGUIDname()) == 0)
      tns->SetField(NumSys::EF_REAL);
    else
      tns->SetField(NumSys::EF_INVALID);
  }
  else {
    if(trf->FmtGetTag() == WaveFmt::WF_PCM)
      tns->SetField(NumSys::EF_INTEGER);
    else if(trf->FmtGetTag() == WaveFmt::WF_IEEE_FLOAT)
      tns->SetField(NumSys::EF_REAL);
    else
      tns->SetField(NumSys::EF_INVALID);
  }
  sig->SetFS(trf->FmtGetFS());
  sig->ReBase(cc, NN);
  ResAry();
  return;
}
void        WaveFiler::Reset             (void) {
  return;
}

double      WaveFiler::GetDuration       (void) {
  double T;
  T  = (double)(trf->GetNumFrms());
  T /= (double)(trf->FmtGetFS());
  return T;
}

//==================================================================================================================================
//==================================================================================================================================
//  The engine to take a file and expose it to the user
//==================================================================================================================================

//_____________________________________________________________________________

//_____________________________________________________________________________
void        WaveFiler::ResAry            (void) {
  NumSys::eType theType;
  ullong        ch;

  theType = tns->GetType();
  ch      = sig->GetCh();
  if     ((theType == NumSys::ET_FLOAT)  && (ch > 0) && (ch < SIZE_MAX_CH))     ResAryIEEE32(ch);
  else if((theType == NumSys::ET_DOUBLE) && (ch == 2))
    ResAryIEEE64stereo();
  else if((theType == NumSys::ET_DOUBLE) && (ch == 1))
    ResAryIEEE64mono();
  else if((theType == NumSys::ET_INT08)  && (ch == 2))
    ResAryInt08stereo();
  else if((theType == NumSys::ET_INT08)  && (ch == 1))
    ResAryInt08mono();
  else if((theType == NumSys::ET_INT16)  && (ch == 2))
    ResAryInt16stereo();
  else if((theType == NumSys::ET_INT16)  && (ch == 1))
    ResAryInt16mono();
  else if((theType == NumSys::ET_INT24)  && (ch > 0) && (ch < SIZE_MAX_CH))     ResAryInt24(ch);
  else if((theType == NumSys::ET_INT32)  && (ch > 0) && (ch < SIZE_MAX_CH))     ResAryInt32(ch);
  else {
    fprintf(stdout, "############# Unhandled Format from WAV file #############\n");
    }
  return;
  }


ullong      WaveFiler::ResAryInt08mono   (void) {
  byte   *tByteBlock; // careful that pread only gathers short data.
  ullong   blockSize;
  ullong   readBytes;
  double **ary = sig->GetAry();;
  ullong   N   = sig->GetN();

  tByteBlock = (byte *)malloc(N * sizeof(byte));
  if(tByteBlock == 0)
    return 0;

  blockSize = N * sizeof(byte);

  //readBytes  = (ullong)pread(fd, tByteBlock, blockSize, trf->GetDataBlockOff());
  readBytes = 0;
  if(readBytes != blockSize)
    return 0;

  byte  *pAry;
  double *pDst;

  pAry = tByteBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pAry);
    pDst += 1;
    pAry += 1;
    }

  free(tByteBlock);

  ullong kk     = 0x80;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(ullong i=0; i<N; i++)
    aa[i] *= bigNo;

  //fprintf(stdout, "Just read in Int-8 mono for real\n");
  //fflush(stdout);

  return N;
  }
ullong      WaveFiler::ResAryInt08stereo (void) {
  byte    *tByteBlock;
  ullong    blockSize;
  ullong    readBytes;
  double  **ary = sig->GetAry();;
  ullong    N   = sig->GetN();

  tByteBlock = (byte *)malloc(2 * N * sizeof(byte));
  if(tByteBlock == 0)
    return 0;

  blockSize = N * 2 * sizeof(byte);

  //readBytes  = (ullong)pread(fd, tByteBlock, blockSize, trf->GetDataBlockOff());
  readBytes = 0;
  if(readBytes != blockSize)
    return 0;

  byte  *pAry;
  double *pDst;

  pAry = tByteBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pAry);
    pDst += 1;
    pAry += 2;
    }

  pAry = tByteBlock;
  pAry++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pAry);
    pDst += 1;
    pAry += 2;
    }

  free(tByteBlock);

  ullong kk     = 0x80;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(ullong i=0; i<N; i++)
    aa[i] *= bigNo;
  aa = ary[1];
  for(ullong i=0; i<N; i++)
    aa[i] *= bigNo;

  //fprintf(stdout, "Just read in Int-16 stereo for real\n");
  //fflush(stdout);

  return N;
  }
ullong      WaveFiler::ResAryInt16mono   (void) {
  short    *tShortBlock; // because pread only gathers short data.
  ullong    blockSize;
  ullong    readBytes;
  double       **ary = sig->GetAry();;
  ullong         N   = sig->GetN();

  tShortBlock = (short *)malloc(N * sizeof(short));
  if(tShortBlock == 0)
    return 0;

  blockSize = N * sizeof(short);

  //readBytes  = (ullong)pread(fd, tShortBlock, blockSize, trf->GetDataBlockOff());
  readBytes = 0;
  if(readBytes != blockSize)
    return 0;

  short  *pAry;
  double *pDst;

  pAry = tShortBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pAry);
    pDst += 1;
    pAry += 1;
    }

  free(tShortBlock);

  ullong kk     = 0x8000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(ullong i=0; i<N; i++)
    aa[i] *= bigNo;

  //fprintf(stdout, "Just read in Int-16 mono for real\n");
  //fflush(stdout);

  return N;
  }
ullong      WaveFiler::ResAryInt16stereo (void) {
  short    *tShortBlock = 0;  // the temporary local landing pad for the bytes
  ullong    blockSize;        // How many bytes on disk per stride
  bool      readOK;           // did the audiofiler get our bytes?
  ullong    stride;           // how many frames in each go-around.
  ullong    res;              // smplRmd ... how much (in Frames) do we still have to do.
  double  **ary;
  ullong    N;
  ullong    currFrame;        // into the ary, one channel only
  ullong    currOff;          // into the file, in bytes
//=========================

// Figure out how big we are
  ary = sig->GetAry();
  N   = sig->GetN();
// Don't worry about the total data size being bigger than in the file, that was vetted long ago.
  if(N < SIZE_FRAME_FETCH)
    stride = N;
  else
    stride = SIZE_FRAME_FETCH;
  blockSize = stride * 2 * sizeof(short); // 2 since this is stereo
  tShortBlock = new short[blockSize];
  if(tShortBlock == 0)
    return 0;

//=========================
// Set up the loop
  res       = N;
  currOff = trf->DataGetBlockOff();
  currFrame = 0;
  short  *pAry;
  double *pDst;
  while(res > 0) {
    readOK  = taf->Fetch((byte *)tShortBlock, blockSize, currOff);
    if(!readOK)
      return 0;

    pAry = tShortBlock;
    pDst = &(ary[0][currFrame]);
    for(uint i=0; i<stride; i++){
     *pDst  = (double)(*pAry);
      pDst += 1;
      pAry += 2;
    }

    pAry = tShortBlock;
    pAry++;
    pDst = &(ary[1][currFrame]);
    for(uint i=0; i<stride; i++){
      *pDst = (double)(*pAry);
      pDst += 1;
      pAry += 2;
    }
    currFrame += stride;
    currOff   += blockSize;
    res       -= stride;
    if(res < stride) {
      stride    = res;
      blockSize = stride * 2 * sizeof(short);
    }
  }
  free(tShortBlock);

  ullong kk     = 0x8000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo  = 1.0 / bigNo;
  bigNo /= sig->GetTLPV();
  bigNo *= sig->GetTLPD();
  aa     = ary[0];
  for(ullong i=0; i<N; i++)
    aa[i] *= bigNo;
  aa = ary[1];
  for(ullong i=0; i<N; i++)
    aa[i] *= bigNo;
  sig->SetResident(true);
  fprintf(stdout, "Just read in Int-16 stereo for real\n"); fflush(stdout);

  return N;
  }
ullong      WaveFiler::ResAryInt24       (const ullong numCh) {
  byte     *tByteBlock;
  ullong    fetchSize;           // How many bytes on disk per smplSize
  ullong    fetchOff;            // into the file, in bytes
  bool      readOK;              // did the audiofiler get our bytes?
  int      *pReInt;
  byte     *pBsrc;
  byte     *pBdst;
  int      *pIsrc;
  double   *pDdst;
  double  **ary;
  ullong    N;
  llong     smplSize; // A little misleading, this is a single block worth of samples in a single channel.
  llong     smplRmd;  // The total number of samples left in the residency
  llong     smplOff;  // The position into the output array, will step by smplSize
//=========================

// Figure out how big we are
  ary = sig->GetAry();;
  N   = sig->GetN();

  if(N < SIZE_FRAME_FETCH)    smplSize = N;
  else                        smplSize = SIZE_FRAME_FETCH;
  pReInt  = new int[numCh * smplSize];
  fetchSize = smplSize * numCh * SIZE_INT24; // WARNING MAGIC due to non-standard integer system.
  tByteBlock = new byte[fetchSize];
  if(tByteBlock == 0)
    return 0;

//=========================


//=========================  Set up the loop
  smplRmd    = N;
  fetchOff   = trf->DataGetBlockOff();
  smplOff    = 0;
//======================================================================== THE LOOPS
  while(smplRmd > 0) {
    // == Int24 -> Int32 loop;
    //============================ Loop Preamble
    readOK  = taf->Fetch((byte *)tByteBlock, fetchSize, fetchOff);
    if(!readOK)      return 0;
    pBsrc  = tByteBlock;
    pBdst  = (byte *)pReInt;
     llong dbSize = smplSize * numCh;
     for(llong sDex = 0; sDex < dbSize; sDex++) {
      *pBdst = 0;           // Push 0 into the lsb of the dest
       pBdst++;
      *pBdst = *pBsrc;
       pBdst++; pBsrc++;
      *pBdst = *pBsrc;
       pBdst++; pBsrc++;
      *pBdst = *pBsrc;
       pBdst++; pBsrc++;
    }
    //== int -> double
    for(ullong chDex = 0; chDex < numCh; chDex++) { // Here is where we de-interleave
      pIsrc = &(pReInt[chDex]); // because it's interleaved
      pDdst = &(ary[chDex][smplOff]);
      for(uint i=0; i<smplSize; i++) {
        int    brab;
        double drou;
        brab = *pIsrc;
        drou = (double)(brab);
        *(pDdst++) = drou;
        pIsrc += numCh;// because it's interleaved
      }
    }

  //============================ Loop Postamble
    smplOff   += smplSize;
    fetchOff  += fetchSize;
    smplRmd   -= smplSize;
    if(smplRmd < smplSize) {
      smplSize   = smplRmd;
      fetchSize  = smplSize * numCh * SIZE_INT24; // WARNING MAGIC due to non-standard integer system.
    }
  } // Done with the transmogrification

  //==== Normalize by number system, TLP and TLV.
  ullong kk     = 0x80000000; // MAGIC The integer system of Int24 was left-justified
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo  = 1.0 / bigNo;
  bigNo /= sig->GetTLPV();
  bigNo *= sig->GetTLPD();
  for(ullong chDex = 0; chDex < numCh; chDex++) {
    aa = ary[chDex];
    for(ullong i=0; i<N; i++)
      aa[i] *= bigNo;
  }

  sig->SetResident(true);

  fprintf(stdout, "Just read in Int-24 %lld-channel just fine for real\n", numCh);
  fflush(stdout);

  delete pReInt;
  delete tByteBlock;
  return N;
  }
ullong      WaveFiler::ResAryInt32       (const ullong numCh) {
  int      *tDataBlock = 0;
  ullong    fetchSize;
  ullong    fetchOff;
  bool      readOK;
  double  **ary;
  ullong    N;
  llong     smplSize; // A little misleading, this is a single block worth of samples in a single channel.
  llong     smplRmd;  // The total number of samples left in the residency
  llong     smplOff;  // The position into the output array, will step by smplSize
//=========================
// Figure out how big we are
  ary = sig->GetAry();
  N   = sig->GetN();
// Don't worry about the total data size being bigger than in the file, that was vetted long ago.
  if(N < SIZE_FRAME_FETCH)
    smplSize = N;
  else
    smplSize = SIZE_FRAME_FETCH;
  fetchSize = smplSize * numCh * sizeof(int); // WARNING Only works because data on file system is magically type int.
  tDataBlock = new int[smplSize * numCh];
  if(tDataBlock == 0)
    return 0;

//=========================
// Set up the loop
  smplRmd    = N;
  fetchOff   = trf->DataGetBlockOff(); // Gotta ask the RIFF Filer where the offset is, then increment our own selves.
  smplOff    = 0;
  int    *pAry;
  double *pDst;
  while(smplRmd > 0) {
    readOK  = taf->Fetch((byte *)tDataBlock, fetchSize, fetchOff); // WARNING This is always in bytes
    if(!readOK) {
      delete tDataBlock;
      return 0;
    }
//==============================
// Do the loop
    for(ullong chDex = 0; chDex < numCh; chDex++) {
      pAry  = tDataBlock;
      pAry += chDex;
      pDst = &(ary[chDex][smplOff]);
      for(uint i=0; i<smplSize; i++){
        *pDst  = (double)(*pAry);
         pDst ++;
         pAry += numCh;
      }
    }
//==============================
// Care and loop feeding
    smplOff   += smplSize;
    fetchOff  += fetchSize;
    smplRmd   -= smplSize;
    if(smplRmd < smplSize) {
      smplSize    = smplRmd;
      fetchSize   = smplSize * numCh * sizeof(int);
    }
  }
//==============================
//  Now normalize
  ullong kk     = 0x80000000;/*FIXME*/ // Magic Number due to number system.
  kk--;
  double bigNo = (double)kk;
  bigNo /= sig->GetTLPV();
  bigNo *= sig->GetTLPD();
  double *aa;
  bigNo = 1.0 / bigNo;
  for(ullong chDex = 0; chDex < numCh; chDex++) {
    aa = ary[chDex];
    for(ullong i=0; i<N; i++)
      aa[i] *= bigNo;
  }
  sig->SetResident(true);
  fprintf(stdout, "Just read in Int-32 %llu-channel for real\n", numCh); fflush(stdout);

  delete tDataBlock;
  return N;
  }
ullong      WaveFiler::ResAryIEEE32      (const ullong numCh) {
  float    *tDataBlock = 0; // as in the Data chunk.  Interleaved, float ... The input sucked in from the file system.
  ullong    blockSize;      // number of samples regardless of channels, chunked or that last one or small size.
  ullong    fetchSize;      // in Bytes
  ullong    fetchOff;    // in Bytes
  bool      readOK;
  ullong    N;              // The size of the whole damned thing in samples.
  double  **ary;            // The output array of arrays.
  ullong    smplRmd;        // How many samples are left to take in given the number of chunks, chunk size and original size
  ullong    smplOff;
  float  *pAry;
  double *pDst;
//=========================
// Figure out how big we are
  ary = sig->GetAry();
  N   = sig->GetN();
// Don't worry about the total data size being bigger than in the file, that was vetted long ago.
  if(N < SIZE_FRAME_FETCH)
    blockSize = N;
  else
    blockSize = SIZE_FRAME_FETCH;
  fetchSize = blockSize * numCh * sizeof(float);
  tDataBlock = new float[blockSize * numCh]; // WARNING is float because this function magically reads in float from file system.
  if(tDataBlock == 0)
    return 0;

//=========================
// Set up the loop
  smplRmd       = N;
  fetchOff   = trf->DataGetBlockOff();
  smplOff = 0;
  while(smplRmd > 0) {
    readOK  = taf->Fetch((byte *)tDataBlock, fetchSize, fetchOff); // WARNING All in Bytes!
    if(!readOK) {
      delete tDataBlock;
      return 0;
    }
    for(ullong chDex = 0; chDex < numCh; chDex++) {
      pAry = tDataBlock;
      pAry += chDex;
      pDst = &(ary[chDex][smplOff]);
      for(uint i=0; i<blockSize; i++){
       *pDst  = (double)(*pAry);
        pDst ++;
        pAry += numCh;
      }
    }


    smplOff       += blockSize;
    fetchOff      += fetchSize;
    smplRmd       -= blockSize;
    if(smplRmd < blockSize) {
      blockSize    = smplRmd;
      fetchSize    = blockSize * numCh * sizeof(float);
    }
  }

  double bigNo = 1.0;
  bigNo /= sig->GetTLPV();
  bigNo *= sig->GetTLPD();
  double *aa;
  bigNo = 1.0 / bigNo;

  for(ullong chDex = 0; chDex < numCh; chDex++) {
    aa = ary[chDex];
    for(ullong i=0; i<N; i++)
      aa[i] *= bigNo;
  }

  sig->SetResident(true);
  fprintf(stdout, "Just read in float-32, %llu-channel stream for real\n", numCh); fflush(stdout);

  delete tDataBlock;
  return N;
  }
ullong      WaveFiler::ResAryIEEE64mono  (void) {
  ullong        readBytes;
  ullong        blockSize;
  //double       **ary = sig->GetAry();;
  ullong         N   = sig->GetN();

  blockSize = N * sizeof(double);

  readBytes = 0;
  //readBytes  = (ullong)pread(fd, ary[0], blockSize, trf->GetDataBlockOff());
  if(readBytes != blockSize)
    return 0;

  //fprintf(stdout, "Just read in double mono for real\n");
  //fflush(stdout);

  return N;
  }
ullong      WaveFiler::ResAryIEEE64stereo(void) {
  ullong        readBytes;
  ullong        blockSize;
  double        *tDblBlock;
  double       **ary = sig->GetAry();;
  ullong         N   = sig->GetN();

  tDblBlock = (double *)malloc(2 * N * sizeof(double));
  if(tDblBlock == 0)
    return 0;

  blockSize = N * sizeof(double) * 2;

  readBytes = 0;
  //readBytes  = (ullong)pread(fd, tDblBlock, blockSize, trf->GetDataBlockOff());
  if(readBytes != blockSize)
    return 0;

  double *pAry;
  double *pDst;

  pAry = tDblBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = *pAry;
    pDst += 1;
    pAry += 2;
    }

  pAry = tDblBlock;
  pAry++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = *pAry;
    pDst += 1;
    pAry += 2;
    }

  free(tDblBlock);

  //fprintf(stdout, "Just read in double stereo for real\n");
  //fflush(stdout);

  return N;
  }

//_____________________________________________________________________________
void        WaveFiler::WriteAry             (void) {
  NumSys::eType   numType;
  double        **tAry;
  ullong          tCh;
  byte           *mHdr;
  ullong          mSize;

  numType = tns->GetType();
  tAry    = sig->GetAry();
  tCh     = sig->GetCh();

  if(tAry == 0)
    return;
  mHdr  = trf->GetMetaHeader();
  mSize = trf->GetMetaSize();
  taf->Pitch(mHdr, mSize);

  if     ((numType == NumSys::ET_FLOAT)  && (tCh == 2))
    WriteAryIEEE32stereo();
  else if((numType == NumSys::ET_FLOAT)  && (tCh == 1))
    WriteAryIEEE32mono();
  else if((numType == NumSys::ET_DOUBLE) && (tCh == 2))
    WriteAryIEEE64stereo();
  else if((numType == NumSys::ET_DOUBLE) && (tCh == 1))
    WriteAryIEEE64mono();
  else if((numType == NumSys::ET_INT16)  && (tCh == 2))
    WriteAryInt16stereo();
  else if((numType == NumSys::ET_INT16)  && (tCh == 1))
    WriteAryInt16mono();
  else if((numType == NumSys::ET_INT24)  && (tCh == 2))
    WriteAryInt24stereo();
  else if((numType == NumSys::ET_INT24)  && (tCh == 1))
    WriteAryInt24mono();
  else if((numType == NumSys::ET_INT32)  && (tCh == 2))
    WriteAryInt32stereo();
  else if((numType == NumSys::ET_INT32)  && (tCh == 1))
    WriteAryInt32mono();
  else
    return;

  return;
  }
void        WaveFiler::WriteAryInt16mono    (void) {
  fprintf(stdout, "Just wrote out Int-16 mono fake\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryInt16stereo  (void) {
  fprintf(stdout, "Just wrote out Int-16 stereo fake\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryInt24mono    (void) {
  byte     tByte[6144]; // which is 2048 mono samples.
  byte    *pDst;
  double **pAry;
  double  *pSrc;
  llong    byteRmd;
  double   bigNo;
  uint     bn;
  union {
    int    i;
    byte   b[4];
  } tI;

  byteRmd  = trf->GetNumFrms();
  byteRmd *= 3* sizeof(ubyte);

  pAry = sig->GetAry();
  pSrc = pAry[0]; // after all, we're mono right now.

  bn = 0x80000000; // since we're converting to int and only taking the first 3 bytes.
  bn--;
  bigNo = (double)bn;

  while(byteRmd > 0) {
    llong miniRes;
    llong byteCount;
    miniRes   =  6144; // which is 2048 mono samples.
    byteCount =     0;
    pDst      = tByte;
    while((miniRes >= 3) && (byteRmd >= 3)) {
      double tD;
      tD    = *pSrc;
      tD   *=  bigNo;
      tI.i  =  (int)tD;
      *pDst = tI.b[1];  pDst++;
      *pDst = tI.b[2];  pDst++;
      *pDst = tI.b[3];  pDst++;
       pSrc++;
       miniRes   -= 3;
       byteRmd   -= 3;
       byteCount += 3;
    }
    taf->Pitch(tByte, byteCount);
  }
  tByte[0] = 0;
  if((trf->GetNumFrms() % 2) != 0)
    taf->Pitch(tByte, 1);

  fprintf(stdout, "Just wrote out Int-24 mono for real\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryInt24stereo  (void) {
  byte     tByte[12288]; // which is 2048 stereo samples.
  byte    *pDst;
  double **pAry;
  double  *pSrc0;
  double  *pSrc1;
  llong    byteRmd;
  double   bigNo;
  uint     bn;
  union {
    int    i;
    byte   b[4];
  } tI;

  byteRmd  = trf->GetNumFrms();
  byteRmd *= 6* sizeof(ubyte);

  pAry = sig->GetAry();
  pSrc0 = pAry[0];
  pSrc1 = pAry[1];

  bn = 0x80000000;
  bn--;
  bigNo = (double)bn;

  while(byteRmd > 0) {
    llong miniRes;
    llong byteCount;
    miniRes   =  12288; // which is 2048 stereo samples.
    byteCount =     0;
    pDst      = tByte;
    while((miniRes >= 6) && (byteRmd >= 6)) {
      double tD;
      double iD;
       iD    = *pSrc0;
       tD    =  iD * bigNo;
       tI.i  =  (int)tD;
      *pDst  = tI.b[1];  pDst++;
      *pDst  = tI.b[2];  pDst++;
      *pDst  = tI.b[3];  pDst++;
       pSrc0++;
       tD    = *pSrc1;
       tD   *=  bigNo;
       tI.i  =  (int)tD;
      *pDst  = tI.b[1];  pDst++;
      *pDst  = tI.b[2];  pDst++;
      *pDst  = tI.b[3];  pDst++;
       pSrc1++;
       miniRes   -= 6;
       byteRmd   -= 6;
       byteCount += 6;
    }
    taf->Pitch(tByte, byteCount);
  }

  fprintf(stdout, "Just wrote out Int-24 stereo for real\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryInt32mono    (void) {
  int      tInt[32768];
  int     *pDst;
  double **pAry;
  double  *pSrc0;
  llong    byteRmd;
  double   bigNo;
  uint     bn;

  byteRmd = trf->GetNumFrms();
  byteRmd *= sizeof(int);
  byteRmd *= sig->GetCh();

  pAry  = sig->GetAry();
  pSrc0 = pAry[0];

  bn = 0x80000000;
  bn--;
  bigNo = (double)bn;

  while(byteRmd > 0) {
    llong miniRes;
    llong byteCount;
    miniRes   = 32768; // which is 2k mono samples.
    byteCount =     0;
    pDst      = tInt;
    while((miniRes >= 4) && (byteRmd >= 4)) {
      double tD;
      int    tI;
      tD    = *pSrc0;
      tD   *=  bigNo;
      tD    = tD > 0 ? tD + 0.5 : tD - 0.5;
      tI    =  (int)tD;
      *pDst = tI;
       pDst++; pSrc0++;
       miniRes   -= 4;
       byteRmd   -= 4;
       byteCount += 4;    }
    taf->Pitch((byte *)tInt, byteCount);
  }

  fprintf(stdout, "Just wrote out Int-32 mono for real\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryInt32stereo  (void) {
  int      tInt[32768];
  int     *pDst;
  double **pAry;
  double  *pSrc0;
  double  *pSrc1;
  llong    byteRmd;
  double   bigNo;
  uint     bn;

  byteRmd = trf->GetNumFrms();
  byteRmd *= sizeof(int);
  byteRmd *= sig->GetCh();

  pAry  = sig->GetAry();
  pSrc0 = pAry[0];
  pSrc1 = pAry[1];

  bn = 0x80000000;
  bn--;
  bigNo = (double)bn;

  while(byteRmd > 0) {
    llong miniRes;
    llong byteCount;
    miniRes   = 32768; // which is 2k stereo samples.
    byteCount =     0;
    pDst      = tInt;
    while((miniRes >= 8) && (byteRmd >= 8)) {
      double tD;
      int    tI;
      tD    = *pSrc0;
      tD   *=  bigNo;
      tD    = tD > 0.0 ? tD + 0.5 : tD - 0.5;
      tI    =  (int)tD;
      *pDst = tI;
       pDst++; pSrc0++;

       tD    = *pSrc1;
       tD   *=  bigNo;
       tD    = tD > 0.0 ? tD + 0.5 : tD - 0.5;
       tI    =  (int)tD;
       *pDst = tI;
       pDst++; pSrc1++;

       miniRes   -= 8;
       byteRmd   -= 8;
       byteCount += 8;
    }
    taf->Pitch((byte *)tInt, byteCount);
  }

  fprintf(stdout, "Just wrote out Int-32 stereo for real\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryIEEE32mono   (void) {
  fprintf(stdout, "Just wrote out float mono fake\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryIEEE32stereo (void) {
  fprintf(stdout, "Just wrote out float stereo fake\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryIEEE64mono   (void) {
  fprintf(stdout, "Just wrote out double mono fake\n");  fflush(stdout);
  return;
  }
void        WaveFiler::WriteAryIEEE64stereo (void) {
  fprintf(stdout, "Just wrote out double stereo fake\n");  fflush(stdout);
  return;
  }

