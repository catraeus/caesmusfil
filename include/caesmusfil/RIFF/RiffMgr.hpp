
//=================================================================================================
// Original File Name : RiffMgr.hpp
// Original Author    : duncang
// Creation Date      : Sep 28, 2013
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
  Is a machine/interface for RIFF files.  Since the RIFF has lots and lots of stuff, it provides
    other interfaces.  Some of the tricksiest stuff this does is the number systems that RIFF
    uses to pack data.  At Catraeus, the Stream is ALWAYS double (IEEE 64-bit float)
  Is an interface to a Stream, hiding all of the RIFF stuff.
  Drives an interface to an AudioFile object.


    Definitions:
      Stream       The musician's currency.  Has FS, Length, Channels.  Contiguous and complete
      Store        The file system's currency.  Has a file descriptor and an encapsulation spec (RIFF::WAVE, fLaC, MP3...
    Use Cases:
      A small file is read in completely to a stream image.
      A small file is written completely from a stream image.
      A large file is read in piece-meal.  String-of-pearl like.  and each pearl is discarded after the musician is done.
        An analyzer will do this to provide a summary analysis or shuttle through for views.
      A large file is randomly read, with an arbitrarily sized, located block.
        An analyzer will do this to shuttle through a viewport of it.  SpecAn, Scope, Song Looping.
    Attributes:
      File Control I/F -
        public:
          File Name
          File Size (in bytes)
          File Type (r, w, rw, b, t)
          File State (enum) GetState() {idle, open, closed, non-existant, isDir, inaccessible} some conflation
          Open (locking, no-locking)
          Close()
          Fetch (n bytes from p location)
          Overwrite (n bytes to p location)
          Append (n bytes)
          <callback>changed out from under us
        private:
          lock, unlock
      Stream I/F - Has info about the RIFF.  In that sense, it is duplicated info between here and a stream.
        public:
          FS
          Len (in samples)
          Len (in time)
          channels
          Start (of in-mem resident sub-stream) in samples
          Len (of in-mem resident sub-stream) in samples
      RIFF I/F
        The RIFF structure on file is flat organized.  RIFF, however, is Hierarchical.  At each GetNext moment, the RiffMgr will simply make a straight-line array of Riff... objects.
          But it also has to figure out whether to push children or pop to a parent to add another child. Hierarcical Tree kind of stuff.
        public:
          Count Number of RIFF sub-sections
          Array of RiffChunk objects There should never be a base class only derived.
          And now is the hard part because, e.g. the user of the RIFF info has to intimately know RIFF.  I'm not going to make
            the RIFF chunk interface expose all of the bits, so the GUI of RIFF has to be really intelligent.
        Methods:
          public:
            Traverse()  Starts at 0 in the file and does a bunch of GetNext().  Tells whether there is residue, leaves a pointer to anything that might come after.
          private:
            GetNext(RiffCk) given the RiffCk, find the next pointer, dig out the fourCC, return a Ck... that is the right kind.
              Since the Ck... got "new"'d it will have auto-parsed.  Ready for doing a GetNext() if there is residue.
      During Traversal, RiffMgr doesn't keep track of the File locations.  Each RiffChunk has knowledge of the file offset and size.
        Assumes that any single RIFF Chunk is contiguous.
        RiffMgr, however, does keep track of RiffCk residue vs. AudioFile residue.
      The Stream Interface interacts in a special way with a RiffChunkData object to get and put data.
    Functions:
      Set File - to associate an AudioFile instance with a RiffFile instance.
      Traverse - to get the stuff on disk into the h tree that is a RIFF smart object.
*/
//=================================================================================================

#ifndef __RIFF_MGR_HPP_
#define __RIFF_MGR_HPP_

#include <stdlib.h>

#include <caesbase/CaesTypes.hpp>
#include <caesbase/CaesCallBack.hpp>

#include <caesmusfil/AudioFiler.hpp>
#include <caesmusfil/Sig/NumSys.hpp>
#include <caesmusfil/IPC/CtlAudMsg.hpp>

#include <caesmusfil/RIFF/RiffCk.hpp>
#include <caesmusfil/RIFF/wav/WaveFmt.hpp>
#include <caesmusfil/RIFF/wav/WaveGUID.hpp>

class RiffMgr {
  public:
    struct sFetch{
      byte  *pImg;
      llong  nImg;
      llong  off;
    };
  private:
    typedef int  tFourCc;
    typedef int  tChLen;
            enum eConst {
              EC_MAX_RIFFS = 1024,
              EC_MAX_FILES =   32
            };
  private:
  public:
                        RiffMgr          ( AudioFiler *i_taf, NumSys *i_tns )                           ;
    virtual            ~RiffMgr          ( void                       )                                 ;
           void         OnFileUpdateSrc  ( void                       )                                 ;
           void         OnParse          ( void                       )                                 ;
           void         OnCreate         ( llong i_ch, double i_FS    )                                 ;
           byte        *GetMetaHeader    ( void                       ) { return metaBlock;            };
           ullong       GetMetaSize      ( void                       ) { return metaSize;             };

    const  llong        GetNumRiffs      ( void                       ) { return numRiffs;             };
    const  char        *RiffNGetHdr      ( llong   i_n                ) { return riffs[i_n] == 0 ? "" : riffs[i_n]->GetHdr();};
    const  llong        RiffNGetFileOff  ( llong   i_n                ) { return riffs[i_n] == 0 ? 0  : riffs[i_n]->GetFileOff();};
    const  llong        RiffNGetSize     ( llong   i_n                ) { return riffs[i_n] == 0 ? 0  : riffs[i_n]->GetSize();};
    const  llong        RiffNGetRes      ( llong   i_n                ) { return riffs[i_n] == 0 ? 0  : riffs[i_n]->GetRes();};

           llong        RiffGetOrder     ( void                       ) { return tcRiff != 0 ? tcRiff->GetOrder() : 0 ;   };
    const  char        *RiffGetHdr       ( void                       ) { return tcRiff != 0 ? tcRiff->GetHdr()   : "";   };
    const  llong        RiffGetSize      ( void                       ) { return tcRiff->GetSize();    };
    const  char        *RiffGetForm      ( void                       ) { return tcRiff->GetForm();    };
    const  llong        GetSubSize       ( void                       ) { return tcRiff->GetSubSize(); };
    const  llong        RiffGetRes       ( void                       ) { return tcRiff->GetRes();     };

    const  llong        FmtGetOrder      ( void                       ) { return tcFmt->GetOrder();    };
    const  char        *FmtGetHdr        ( void                       ) { return tcFmt->GetHdr();      };
    const  llong        FmtGetSize       ( void                       ) { return tcFmt->GetSize();     };
    const  llong        FmtGetRes        ( void                       ) { return tcFmt->GetRes();      };
    const  uint         FmtGetTagCode    ( void                       ) { return tcFmt->GetTagCode();  };
    const  ullong       FmtGetTagCode    ( ullong i_tagDex            ) { return (ullong)WaveFmt::GetTagCode(i_tagDex);  };
    const  WaveFmt::eFmtTag FmtGetTag    ( void                       ) { return tcFmt->GetTagType();};
    const  char        *FmtGetTagName    ( void                       ) { return tcFmt->GetTagName();  };
    const  char        *FmtGetTagName    ( ullong   i_tagDex          ) { return WaveFmt::GetTagName(i_tagDex);  };
    const  llong        FmtGetCh         ( void                       ) { return tcFmt->GetCh();       };
    const  double       FmtGetFS         ( void                       ) { return tcFmt->GetFS();       };
    const  llong        FmtGetBitDepth   ( void                       ) { return tcFmt->GetBitDepth(); };
    const  llong        FmtGetByteRate   ( void                       ) { return tcFmt->GetByteRate(); };
    const  llong        FmtGetBlkAlign   ( void                       ) { return tcFmt->GetBlkAlign(); };
    const  llong        FmtGetExtSize    ( void                       ) { return tcFmt->GetExtSize();  };
    const  llong        FmtGetValBits    ( void                       ) { return tcFmt->GetValBits();  };
    const  ullong       FmtGetChMask     ( void                       ) { return tcFmt->GetChMask();   };
    const  char        *FmtGetWaveGUID   ( void                       ) { return tcFmt->GetWaveGUID(); };
    const  char        *FmtGetGUIDname   ( void                       ) { return tcFmt->GetGUIDname(); };

           ullong       FmtSetCh         ( ullong        i_ch         )                                 ;
           void         FmtSetType       ( NumSys::eType i_type       )                                 ;
           double       FmtSetFS         ( double       i_FS          )                                 ;

    const  bool         HasFact          ( void                       ) { return tcFact != 0;          };
    const  llong        FactGetOrder     ( void                       ) { return tcFact->GetOrder();   };
    const  char        *FactGetHdr       ( void                       ) { return tcFact->GetHdr();     };
    const  llong        FactGetSize      ( void                       ) { return tcFact->GetSize();    };
    const  llong        FactGetNFrm      ( void                       ) { return tcFact->GetNFrm();    };
    const  llong        FactGetRes       ( void                       ) { return tcFact->GetRes();     };

    const  llong        DataGetOrder     ( void                       ) { return tcData->GetOrder();   };
    const  char        *DataGetHdr       ( void                       ) { return tcData->GetHdr();     };
    const  llong        DataGetSize      ( void                       ) { return tcData->GetSize();    };
    const  llong        DataGetBlockOff  ( void                       ) { return tcData->GetFileOff() + 8;};
    const  llong        DataGetRes       ( void                       ) { return tcData->GetRes();     };

           llong        GetRemainder     ( void                       ) { return remainder;            };
           bool         IsValid          ( void                       ) { return valid;                };
           bool         RiffTreeReRoot   ( void                       );
    const  char        *RiffTreeGetHdr   ( void                       ) { if(rtsCurr == 0) return ""; else return rtsCurr->GetHdr();    };
    const  llong        GetNumFrms       ( void                       ) { return N;              };
           void         Parse            ( void                       )                                 ;

           void         SetDataType      ( NumSys::eField  i_dataType )                                 ;
           llong        SetN             ( llong           i_numFrms  )                                 ;
           llong        SetBitDepth      ( llong           i_bitdepth )                                 ;
           double       SetFS            ( double          i_FS       )                                 ;

           void         Build            ( void                       );
           llong        GetFileSize      ( void                       );
  private:
           void         Reset            ( void                       );
           void         PlaceCanonical   ( RiffCk        *i_currRiff  );



  private:
          //. - Initialized by creation  at and only at instantiation
          //= - Initialized by reference at and only at instantiation
          //+ - Initialized by delete/create/reset at all reset events
  public:
  private:

    byte         *metaBlock;
    ullong        metaSize;
    RiffCk      **riffs;          //.
    uint         *offsets;        //+
    uint          numRiffs;       //+
    CkRIFF       *tcRiff;         //+
    CkFmt        *tcFmt;          //+
    CkFact       *tcFact;         //+
    CkBext       *tcBext;         //+
    CkPAD        *tcPad;          //+
    CkData       *tcData;         //+
    CkInfo       *tcInfo;         //+
    CkPEAK       *tcPeak;         //+
    CkUnk       **tcUnk;          //.
    llong         numUnk;

    RiffCk       *rtsCurr;
    AudioFiler   *taf;
    NumSys       *tns;

    ///The whole file size that the RiffMgr will manage
    llong         remainder;      //+
    llong         N;

    bool          valid;          //+
    bool          dirty;          //+
    bool          stillGood;      //+


    //==============================
    // Interconn
  public:
    CbV          *CbChangeRiff;   //.

    CtlAudMsg    *ctAm;
  };

#endif // __RIFF_MGR_HPP_
