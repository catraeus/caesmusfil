
//=================================================================================================
// Original File Name : RawFile.cpp
// Original Author    : duncang
// Creation Date      : Jul 4, 2014 1:54:09 PM
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

#include <malloc.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include <caesbase/CaesString.hpp>

#include <caesmusfil/raw/RawFile.hpp>

//=================================================================================================
             RawFile::RawFile(Signal *i_sig, NumSys *i_tns )
: sig(i_sig)
, tNumSys(i_tns) {

  accessType        =      EA_READ;
  valid             =      false;
  dirty             =      false;

  fd                =     -1;
  fileName          =      new char[SIZE_FILE_NAME];
  fileName[0]       =    '\0';
  dataBlock         =      0;
  resident          =      false;
  fileSize          =      0;


  CbChangeStream    =      0;

  duration          =      0.0;

  }
             RawFile::~RawFile(void) {
  delete fileName;
  fprintf(stdout, "RawFile destructor\n");
  }
bool         RawFile::FileCheck(char *i_fileName) {
  int accessMode;
  int accessAnswer;

  if(accessType == RawFile::EA_READ) {
    accessMode = F_OK;
    accessAnswer = access(i_fileName, accessMode);
    if(accessAnswer == -1) {
      fprintf(stdout, "The requested file - %s - has problems because %s.\n", i_fileName, strerror(errno));
      fflush(stdout);
      return false;
      }
    strcpy(fileName, i_fileName);
    Open();
    if(valid) {
      Close();
      return true;
      }
    fileName[0] = '\0';
    fd          = -1;
    }
  else {
    accessMode = F_OK;
    accessAnswer = access(i_fileName, accessMode);
    if(accessAnswer == -1) {
      if(errno == ENOENT)
        fprintf(stdout, "File doesn't exist ... COOL\n");
      else {
        fprintf(stdout, "The requested file - %s - has problems because .%d. %s.\n", i_fileName, errno, strerror(errno));
        fflush(stdout);
        return false;
        }
      }
    else
      fprintf(stdout, "File DOES EXIST ... Watch Out!\n");
    strcpy(fileName, i_fileName);
    Open();
    if(valid) {
      Close();
      return true;
      }
    fileName[0] = '\0';
    fd          = -1;
    }
  return false;
  }
void         RawFile::Open() {
  struct stat fs;

  if(accessType == EA_READ) {
    fd = open(fileName, O_RDONLY);
    if(fd < 0) { // check based on fd number.
      fprintf(stdout, "The input file has problems being opened.\n");
      valid = false;
      fflush(stdout);
      return;
      }
    else {
      valid = true;
      }

    fstat(fd, &fs);
    if(fs.st_mode & S_IFDIR) { // check if it's a directory
      fprintf(stdout, "Sorry, but directories can't be used for content.\n");
      close(fd);
      fd = -1;
      valid = false;
      }
    fileSize = fs.st_size; // HOORAY, we can open the file.
    }
  else if(accessType == EA_WRITE) {
    fd = open(fileName, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if(fd < 0) {
      fprintf(stdout, "The output file has problems being opened: %s\n", strerror(errno));
      valid = false;
      fflush(stdout);
      return;
      }
    else {
      valid = true;
      }

    fstat(fd, &fs);
    if(fs.st_mode & S_IFDIR) {
      fprintf(stdout, "Sorry, but directories can't be used for content.\n");
      valid = false;
      close(fd);
      fd = -1;
      }
    fileSize = fs.st_size;
    }
  else { // if(accessType == EA_READ) -- Bad enum, deep in c++ Guts we have a problem!
    valid = false;
    }
  return;
  }

void         RawFile::SetAccessWrite     (void) {
  accessType = EA_WRITE;
  SetDataType(NumSys::ET_INT32);
  SetBitDepth(32);
  ReCalc();
  return;
  }

void         RawFile::Write(void) {

  dataBlock = new byte[fileSize];

  WriteAry();
  write(fd, dataBlock, fileSize);
  delete dataBlock;
  dataBlock = 0;
  return;
  }
void         RawFile::Close(void) {
  close(fd);
  fd = -1;
  return;
  }
void         RawFile::PrintHeaders(void) {

  if(!valid) {
    fprintf(stdout, "File format is invalid\n");
    return;
    }
  fprintf(stdout, "file size:                 %lld\n",    fileSize                );
  fprintf(stdout, "Channel Count:             %lld\n",    sig->GetCh()        );
  fprintf(stdout, "Sample Rate:               %lf\n",     sig->GetFS()        );
  fprintf(stdout, "Bits Per Sample:           %lld\n",    tNumSys->GetBitDepth()  );
  fprintf(stdout, "sample count:              %lld\n",    sig->GetN()         );
  return;
  }
//_____________________________________________________________________________
void     RawFile::SetDataType(NumSys::eType i_type) {
  tNumSys->SetNumType(i_type);
  return;
  }
//_____________________________________________________________________________

//_____________________________________________________________________________
double    **RawFile::ResAry            (void) {

  if     ((tNumSys->GetType() == NumSys::ET_FLOAT) && (sig->GetCh() == 2))
    ResAryIEEE32stereo();
  else if((tNumSys->GetType() == NumSys::ET_FLOAT) && (sig->GetCh() == 1))
    ResAryIEEE32mono();
  else if((tNumSys->GetType() == NumSys::ET_DOUBLE) && (sig->GetCh() == 2))
    ResAryIEEE64stereo();
  else if((tNumSys->GetType() == NumSys::ET_DOUBLE) && (sig->GetCh() == 1))
    ResAryIEEE64mono();
  else if((tNumSys->GetType() == NumSys::ET_INT16) && (sig->GetCh() == 2))
    ResAryInt16stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT16) && (sig->GetCh() == 1))
    ResAryInt16mono();
  else if((tNumSys->GetType() == NumSys::ET_INT24) && (sig->GetCh() == 2))
    ResAryInt24stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT24) && (sig->GetCh() == 1))
    ResAryInt24mono();
  else if((tNumSys->GetType() == NumSys::ET_INT32) && (sig->GetCh() == 2))
    ResAryInt32stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT32) && (sig->GetCh() == 1))
    ResAryInt32mono();
  else if((tNumSys->GetType() == NumSys::ET_INT64) && (sig->GetCh() == 2))
    ResAryInt64stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT64) && (sig->GetCh() == 1))
    ResAryInt64mono();
  else {
    fprintf(stdout, "############# Unhandled Format from WAV file #############\n");
    //sig->VoidAry();
    }

  return sig->GetAry();
  }
llong       RawFile::ResAryInt16mono   (void) {
  short    *tShortBlock; // because pread only gathers short data.
  llong     blockSize;
  llong     readBytes;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tShortBlock = (short *)malloc(N * sizeof(short));
  if(tShortBlock == 0)
    return 0;

  blockSize = N * sizeof(short);

  readBytes  = (llong)pread(fd, tShortBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  short  *pSrc;
  double *pDst;

  pSrc = tShortBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 1;
    }

  free(tShortBlock);

  llong kk     = 0x8000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;

  //fprintf(stdout, "Just read in Int 16-bit mono for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt16stereo (void) {
  short    *tShortBlock;
  llong     blockSize;
  llong     readBytes;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tShortBlock = (short *)malloc(2 * N * sizeof(short));
  if(tShortBlock == 0)
    return 0;

  blockSize = N * 2 * sizeof(short);

  readBytes  = (llong)pread(fd, tShortBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  short  *pSrc;
  double *pDst;

  pSrc = tShortBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  pSrc = tShortBlock;
  pSrc++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  free(tShortBlock);

  llong kk     = 0x8000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  aa = ary[1];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;

  //fprintf(stdout, "Just read in Int 16-bit stereo for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt24mono   (void) {
  byte          *pSrcB;
  byte          *pDstB;
  byte          *pBlockIn;
  int           *pBlockL;
  llong         readBytes;
  llong         blockSize;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  blockSize = 3 * N;

  pBlockIn = (byte *)malloc(blockSize);
  if(pBlockIn == 0)
    return 0;
  pBlockL  = (int *)malloc(N * sizeof(int));
  if(pBlockL == 0)
    return 0;

  readBytes  = (llong)pread(fd, pBlockIn, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  for(llong i=0; i<N; i++) {
    pBlockL[i] = 0;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL;
  pDstB += 1; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 3;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL;
  pSrcB += 1;
  pDstB += 2; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 3;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL;
  pSrcB += 2;
  pDstB += 3; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 3;
    }

  free(pBlockIn);

  for(llong i=0; i<N; i++)
    ary[0][i] = pBlockL[i];

  free(pBlockL);

  llong kk     = 0x80000000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;

  //fprintf(stdout, "Just read in Int 24-bit mono for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt24stereo (void) {
  byte         *pSrcB;
  byte         *pDstB;
  byte         *pBlockIn;
  int          *pBlockL0;
  int          *pBlockL1;
  llong         readBytes;
  llong         blockSize;
  double      **ary = sig->GetAry();;
  llong         N   = sig->GetN();

  blockSize = 2 * 3 * N;

  pBlockIn = (byte *)malloc(blockSize);
  if(pBlockIn == 0)
    return 0;
  pBlockL0  = (int *)malloc(2 * N * sizeof(int));
  if(pBlockL0 == 0)
    return 0;
  pBlockL1  = (int *)malloc(2 * N * sizeof(int));
  if(pBlockL1 == 0)
    return 0;

  readBytes  = (llong)pread(fd, pBlockIn, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  for(llong i=0; i<N; i++) {
    pBlockL0[i] = 0;
    }
  for(llong i=0; i<N; i++) {
    pBlockL1[i] = 0;
    }
  //________________________________________________________
  //Left Channel Block
  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL0;
  pSrcB += 0;
  pDstB += 1; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 6;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL0;
  pSrcB += 1;
  pDstB += 2; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 6;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL0;
  pSrcB += 2;
  pDstB += 3; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 6;
    }

  //________________________________________________________
  //Right Channel Block
  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL1;
  pSrcB += 3;
  pDstB += 1; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 6;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL1;
  pSrcB += 4;
  pDstB += 2; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 6;
    }

  pSrcB    =         pBlockIn;
  pDstB    = (byte *)pBlockL1;
  pSrcB += 5;
  pDstB += 3; // point to what will be the next to LSB.
  for(llong i=0; i<N; i++) {
    *pDstB = *pSrcB;
    pDstB += 4;
    pSrcB += 6;
    }

  free(pBlockIn);

  for(llong i=0; i<N; i++)
    ary[0][i] = pBlockL0[i];
  for(llong i=0; i<N; i++)
    ary[1][i] = pBlockL1[i];

  free(pBlockL0);
  free(pBlockL1);

  llong kk     = 0x80000000;
  kk--;
  double bigNo = (double)kk;
  bigNo = 1.0 / bigNo;
  for(llong i=0; i<N; i++)
    ary[0][i] *= bigNo;
  for(llong i=0; i<N; i++)
    ary[1][i] *= bigNo;

  //fprintf(stdout, "Just read in Int 24-bit stereo\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt32mono   (void) {
  int      *tIntBlock;
  llong     blockSize;
  llong     readBytes;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tIntBlock = (int *)malloc(sig->GetN() * sizeof(int));
  if(tIntBlock == 0)
    return 0;

  blockSize = N * sizeof(int);

  readBytes  = (llong)pread(fd, tIntBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  int  *pSrc;
  double *pDst;

  pSrc = tIntBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 1;
    }

  free(tIntBlock);

  llong kk     = 0x80000000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  //fprintf(stdout, "Just read in Int 32-bit mono for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt32stereo (void) {
  int    *tIntBlock;
  llong     blockSize;
  llong     readBytes;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tIntBlock = (int *)malloc(2 * N * sizeof(int));
  if(tIntBlock == 0)
    return 0;

  blockSize = N * 2 * sizeof(int);

  readBytes  = (llong)pread(fd, tIntBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  int  *pSrc;
  double *pDst;

  pSrc = tIntBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  pSrc = tIntBlock;
  pSrc++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  free(tIntBlock);

  llong kk     = 0x80000000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  aa = ary[1];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  //fprintf(stdout, "Just read in Int 32-bit stereo for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt64mono   (void) {
  llong    *tIntBlock;
  llong     blockSize;
  llong     readBytes;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tIntBlock = (llong *)malloc(N * sizeof(llong));
  if(tIntBlock == 0)
    return 0;

  blockSize = N * sizeof(llong);

  readBytes  = (llong)pread(fd, tIntBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  llong  *pSrc;
  double *pDst;

  pSrc = tIntBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 1;
    }

  free(tIntBlock);

  llong kk     = 0x8000000000000000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  //fprintf(stdout, "Just read in Int 32-bit mono for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryInt64stereo (void) {
  llong  *tIntBlock;
  llong     blockSize;
  llong     readBytes;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tIntBlock = (llong *)malloc(2 * N * sizeof(llong));
  if(tIntBlock == 0)
    return 0;

  blockSize = N * 2 * sizeof(llong);

  readBytes  = (llong)pread(fd, tIntBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  llong  *pSrc;
  double *pDst;

  pSrc = tIntBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  pSrc = tIntBlock;
  pSrc++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  free(tIntBlock);

  llong kk     = 0x8000000000000000;
  kk--;
  double bigNo = (double)kk;
  double *aa;
  bigNo = 1.0 / bigNo;
  aa = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  aa = ary[1];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;
  //fprintf(stdout, "Just read in Int 32-bit stereo for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryIEEE32mono  (void) {
  llong         readBytes;
  llong         blockSize;
  float         *tFloatBlock;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tFloatBlock = (float *)malloc(1 * N * sizeof(float));
  if(tFloatBlock == 0)
    return 0;

  blockSize = N * sizeof(float) * 1;

  readBytes  = (llong)pread(fd, tFloatBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  float  *pSrc;
  double *pDst;

  pSrc = tFloatBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 1;
    }

  free(tFloatBlock);

  //fprintf(stdout, "Just read in IEEE 32-bit mono for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryIEEE32stereo(void) {
  llong         readBytes;
  llong         blockSize;
  float         *tFloatBlock;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tFloatBlock = (float *)malloc(2 * N * sizeof(float));
  if(tFloatBlock == 0)
    return 0;

  blockSize = N * sizeof(float) * 2;

  readBytes  = (llong)pread(fd, tFloatBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  float  *pSrc;
  double *pDst;

  pSrc = tFloatBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  pSrc = tFloatBlock;
  pSrc++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = (double)(*pSrc);
    pDst += 1;
    pSrc += 2;
    }

  free(tFloatBlock);

  //fprintf(stdout, "Just read in IEEE 32-bit stereo for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryIEEE64mono  (void) {
  llong         readBytes;
  llong         blockSize;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  blockSize = N * sizeof(double);

  readBytes  = (llong)pread(fd, ary[0], blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  //fprintf(stdout, "Just read in IEEE 64-bit mono for real\n");
  //fflush(stdout);

  return N;
  }
llong       RawFile::ResAryIEEE64stereo(void) {
  llong         readBytes;
  llong         blockSize;
  double        *tDblBlock;
  double       **ary = sig->GetAry();;
  llong          N   = sig->GetN();

  tDblBlock = (double *)malloc(2 * N * sizeof(double));
  if(tDblBlock == 0)
    return 0;

  blockSize = N * sizeof(double) * 2;

  readBytes  = (llong)pread(fd, tDblBlock, blockSize, 0);
  if(readBytes != blockSize)
    return 0;

  double *pSrc;
  double *pDst;

  pSrc = tDblBlock;
  pDst = ary[0];
  for(uint i=0; i<N; i++){
    *pDst = *pSrc;
    pDst += 1;
    pSrc += 2;
    }

  pSrc = tDblBlock;
  pSrc++;
  pDst = ary[1];
  for(uint i=0; i<N; i++){
    *pDst = *pSrc;
    pDst += 1;
    pSrc += 2;
    }

  free(tDblBlock);

  //fprintf(stdout, "Just read in IEEE 64-bit stereo for real\n");
  //fflush(stdout);

  return N;
  }

//_____________________________________________________________________________
void        RawFile::WriteAry             (void) {

  if(sig->GetAry() == 0)
    return;
  if(dataBlock == 0)
    return;

  if     ((tNumSys->GetType() == NumSys::ET_FLOAT) && (sig->GetCh() == 2))
    WriteAryIEEE32stereo();
  else if((tNumSys->GetType() == NumSys::ET_FLOAT) && (sig->GetCh() == 1))
    WriteAryIEEE32mono();
  else if((tNumSys->GetType() == NumSys::ET_DOUBLE) && (sig->GetCh() == 2))
    WriteAryIEEE64stereo();
  else if((tNumSys->GetType() == NumSys::ET_DOUBLE) && (sig->GetCh() == 1))
    WriteAryIEEE64mono();
  else if((tNumSys->GetType() == NumSys::ET_INT16) && (sig->GetCh() == 2))
    WriteAryInt16stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT16) && (sig->GetCh() == 1))
    WriteAryInt16mono();
  else if((tNumSys->GetType() == NumSys::ET_INT24) && (sig->GetCh() == 2))
    WriteAryInt24stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT24) && (sig->GetCh() == 1))
    WriteAryInt24mono();
  else if((tNumSys->GetType() == NumSys::ET_INT32) && (sig->GetCh() == 2))
    WriteAryInt32stereo();
  else if((tNumSys->GetType() == NumSys::ET_INT32) && (sig->GetCh() == 1))
    WriteAryInt32mono();
  else
    return;

  return;
  }
void        RawFile::WriteAryInt16mono    (void) {
  return;
  }
void        RawFile::WriteAryInt16stereo  (void) {
  return;
  }
void        RawFile::WriteAryInt24mono    (void) {
  return;
  }
void        RawFile::WriteAryInt24stereo  (void) {
  return;
  }
void        RawFile::WriteAryInt32mono    (void) {
  int      *tIntBlock;
  double  **ary    = sig->GetAry();;
  llong     N      = sig->GetN();
  double   *pSrc;
  int      *pDst;
  llong     kk;
  double    bigNo;
  double   *aa;

  tIntBlock = (int *)dataBlock;
  if(tIntBlock == 0)
    return;

  kk    = 0x80000000;
  kk--;
  bigNo = (double)kk;
  aa    = ary[0];
  for(llong i=0; i<N; i++)
    aa[i] *= bigNo;

  pSrc = ary[0];
  pDst = tIntBlock;
  for(uint i=0; i<N; i++){
   *pDst  = (int)(*pSrc);
    pDst += 1;
    pSrc += 1;
    }


  //fprintf(stdout, "Just wrote Int 32-bit mono for real\n");
  //fflush(stdout);

  return;
  }
void        RawFile::WriteAryInt32stereo  (void) {
  return;
  }
void        RawFile::WriteAryIEEE32mono   (void) {
  return;
  }
void        RawFile::WriteAryIEEE32stereo (void) {
  return;
  }
void        RawFile::WriteAryIEEE64mono   (void) {
  double   *tDblBlock;
  double  **ary    = sig->GetAry();;
  llong     N      = sig->GetN();
  double   *pSrc;
  double   *pDst;

  tDblBlock = (double *)dataBlock;
  if(tDblBlock == 0)
    return;

  pSrc = ary[0];
  pDst = tDblBlock;
  for(uint i=0; i<N; i++){
   *pDst  = *pSrc;
    pDst += 1;
    pSrc += 1;
    }


  //fprintf(stdout, "Just wrote float 64 bit mono for real\n");
  //fflush(stdout);

  return;
  }
void        RawFile::WriteAryIEEE64stereo (void) {
  return;
  }


//_____________________________________________________________________________
void        RawFile::ReCalc            (void) {
  duration = (double)GetNumFrms() / GetFmtFS();
  fileSize = sig->GetCh() * sig->GetN() * tNumSys->GetBitDepth() / 8;
  return;
  }
void        RawFile::SetBitDepth(llong i_bitDepth) {
  tNumSys->SetBitDepth(i_bitDepth);
  return;
  }
