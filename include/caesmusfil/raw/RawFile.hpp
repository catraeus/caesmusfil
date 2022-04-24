
//=================================================================================================
// Original File Name : RawFile.hpp
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
    The wave file is read or write.  It can change between each dynamically.
    It will acquire a lock on access.
    There are several magic moments:
      Creation
        initializes variables.
      Validation
        Must have a file name.
        Finds file existence
        Finds file accessibility
        Finds file size
      Open
        Gives us an int fd and a FILE *filePtr
        Always opens for rw
        Locks
      Scan
        descends and validates the chunk structures
        fills the non-data chunk block
      Read
        Gets the data into double arrays
      ReArray
        Points to a new set of double vectors, re-does size calculation
        Needs SR, Sample Format, Channels, numFrames
        Re-Builds fmt and fact block areas.
      Write
        Duh
      Close
        Duh
*/
//=================================================================================================

#ifndef __RAW_FILE_HPP_
#define __RAW_FILE_HPP_

#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <caesbase/CaesTypes.hpp>
#include <caesbase/CaesCallBack.hpp>

#include <caesmusfil/Sig/NumSys.hpp>
#include <caesmusfil/Sig/Signal.hpp>
#include <caesmusfil/RIFF/RiffCk.hpp>
#include <caesmusfil/RIFF/wav/WaveGUID.hpp>


class RawFile {
  public:
  private:
    enum eConst {
      SIZE_FILE_NAME    = 32768  /* 32768, ridiculously long file name   */
      };
    enum eAccess {
      EA_READ, // File Read Access
      EA_WRITE // File Write Access
      };
  public:
                 RawFile            (Signal *i_sig, NumSys *i_tns );
                ~RawFile            (void);
    bool         FileCheck          (char *i_fileName);
    void         Open               (void);
    void         Write              (void);
    void         Close              (void);
    void         PrintHeaders       (void);

    double       SetFS              (double i_FS);
    llong        SetCh              (llong i_ch);
    void         SetDataType        (NumSys::eType i_type);
    void         SetBitDepth        (llong i_bitDepth);
    void         SetNumFrms         (llong i_numFrms);
    llong        GetNumFrms         (void) {return sig->GetN();                    };
    double       GetDuration        (void) {return duration;                      };
    char        *GetTypeName        (void) {return NumSys::typeNames[tNumSys->GetType()];                      };

    void         SetSize            (long theSize);
    void         SetAccessRead      (void)  {accessType = EA_READ;  return;       };
    void         SetAccessWrite     (void);

    ullong       GetFileSize        (void)  {return fileSize;                     };
    llong        GetFmtNumCh        (void)  {return sig->GetCh();      };
    double       GetFmtFS           (void)  {return sig->GetFS();         };
    llong        GetBitDepth        (void)  {return tNumSys->GetBitDepth();   };

    bool         GetValid           (void)  {return valid;};

    double     **ResAry             (void);
    double     **GetAry             (void)  {return sig->GetAry();};
    void         WriteAry           (void);

  private:
    llong        ResAryInt16mono    (void);
    llong        ResAryInt16stereo  (void);
    llong        ResAryInt24mono    (void);
    llong        ResAryInt24stereo  (void);
    llong        ResAryInt32mono    (void);
    llong        ResAryInt32stereo  (void);
    llong        ResAryInt64mono    (void);
    llong        ResAryInt64stereo  (void);
    llong        ResAryIEEE32mono   (void);
    llong        ResAryIEEE32stereo (void);
    llong        ResAryIEEE64mono   (void);
    llong        ResAryIEEE64stereo (void);

    void         WriteChunks          (void);
    void         WriteAryInt16mono    (void);
    void         WriteAryInt16stereo  (void);
    void         WriteAryInt24mono    (void);
    void         WriteAryInt24stereo  (void);
    void         WriteAryInt32mono    (void);
    void         WriteAryInt32stereo  (void);
    void         WriteAryInt64mono    (void);
    void         WriteAryInt64stereo  (void);
    void         WriteAryIEEE32mono   (void);
    void         WriteAryIEEE32stereo (void);
    void         WriteAryIEEE64mono   (void);
    void         WriteAryIEEE64stereo (void);

    void         ReCalc             (void);


  public:
    //==============================
    Signal       *sig;
    NumSys       *tNumSys;
    CbV          *CbChangeStream;
  private:
    eAccess       accessType;
    char         *fileName;
    llong         fd;
    llong         fileSize;
    byte         *dataBlock;


    bool          valid;
    bool          resident;
    bool          dirty;

    double        duration;

  };

#endif /* __RAW_FILE_HPP_ */
