
//=================================================================================================
// Original File Name : RiffCk.hpp
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
  Factory ...
    There is an array of structures that is maintained in conjunction with the creation of new classes
For a read file, it takes a pointer to an unknown FORM and gives you back a legitimate known
or explicitly unknown derived type.

For a write file, it takes a pointer to a pre-made space that better be big enough (not too hard)
and fills it with the right stuff.

Since the RIFF size is a hierarchical container size and the data size depends on frames and
fmt_ information, the cross-correlation of



Base Class:
  parameters
    fourCC.  That shows how to handle content.
    size     That shows how bit it is.  Watch out, the RIFF chunk is a special case with implied size.  Supplants to the whole encapsulations's size.
    oSrc     The offset, in bytes, in the source file to find the fourCC.
    pImg     The pointer in memory to the beginning (fourCC) of where it was simply transcribed from disk to memory ( or vice-versa for a write situation)
    nBytes   How many bytes were taken from disk.  Includes the fourCC and size.
  methods
    public:
      Ck...(Ck...)  Takes in the parent.  Gets the parent's "Next()" Get() the stuff from disk, auto-parses
      GetParent()
      GetChild(int)
      GetNumChildren()
    private:
      Parse()  Take apart the image and distribute it, typed, into the structure.
      Get()    From its locaion in disk.
      Next()   Offset to the next oSrc.

Manager asks Static Class for PreFetch size.
Factory takes in PreFetch bytes and returns new Ck...
Manager asks Ck... how many bytes to fetch and where to put them.
Manager asks Ck... to parse the bytes it just got.

Top level structure
-------------------
Fairly flat.  There are only three major concepts, the Chunk hierarchy, the Fmt_ as a very special chunk that has hierarchy
and the GUID that is our old friend Microsoft's arogance in believing that they could own something Global.

______
Meta Image:
-----------
This came from the WaveFile above the Chunk.  It has the first 32k of Wave File that better damn have all of the relevant
chunk info in it.  In the read direction, there might be any amount of cruft that they thought might be cool to put into
the wave file, there are the following chunk types that I've been able to find so far:
- .RIFF.
- .WAVE.
- .fmt .
- .fact.
- .data.
- .bext.
- .PAD .
- .cue .
- .plst.
- .list.
- .labl.
- .ltxt.
- .note.
- .smpl.
- .inst.
- .wavl.
- .slnt.

But I will only write out RIFF, WAVE, fmt , fact, and data.  The others are not useful, and most players don't even try to
look at these.  MP3, OGG and AIFF have lots of rich info if you want that.
______
Modality:
----------------
The chunk system will be either a read system or a write system.  The WaveFile system will take care of the whim of the user
in going back and forth.  That way the Chunk system will not have to be too stateful.




______
How to specify a WAVE file:
--------------------------
The WaveFile will take care of the minimum/sufficient info that a user might demand.  The Chunk will expand to the more
gangly interdependent parameters of the Fmt chunk.


*/
//=================================================================================================

#ifndef __RIFF_CK_HPP_
#define __RIFF_CK_HPP_

#include <stdio.h>

#include <caesbase/CaesTypes.hpp>

#include <caesmusfil/Sig/NumSys.hpp>
#include <caesmusfil/RIFF/wav/WaveFmt.hpp>
#include <caesmusfil/RIFF/wav/WaveGUID.hpp>

class RiffCk {
  public:
    enum eForm { // MAGIC The order of this must be kept strict to the list ck4CCSpec in the static instance.
      ERF_RIFF, // .RIFF. -- and this is the marker that starts it all ... Is Hierarchical, I only know WAVE
      ERF_WAVE, // .WAVE. -- but this is the RIFF container's special contents ... Is Hierarchy Type, I only know RIFF::WAVE
      ERF_FLAC, // .WAVE. -- but this is the RIFF container's special contents ... Is Hierarchy Type, I only know RIFF::WAVE
      ERF_bext, // .bext. -- Broadcast Audio Extension
      ERF_fmt_, // .fmt . -- Mandatory Format Specifier
      ERF_fact, // .fact. -- Helper for compressed formats.  How many samples will there be after reconstitution
      ERF_PAD_, // .PAD . -- How to make the data chunk always be the last and not need to move on-disk.the hierarchy type.
      ERF_JUNK, // .PAD . -- How to make the data chunk always be the last and not need to move on-disk.
      ERF_cue_, // .cue . -- A variable length (24-bytes per cue point) chunk of significant moments
      ERF_wavl, // .wavl. -- Wrapper to tell that there will be an array of .data.+.slnt. chunks
      ERF_data, // .data. -- The reason we're here
      ERF_afsp, // .slnt. -- Silence to allow segmented data to be reduced in size.
      ERF_PEAK, // .PEAK. -- Offset to and value of highest peak in each channel.
      ERF_slnt, // .slnt. -- Silence to allow segmented data to be reduced in size.
      ERF_list, // .list. -- To talk about cue points (Can be upper()!) ... Is Hierarchical, I only knw INFO
      ERF_info, // .info. --
      ERF_labl, // .labl. -- Mandatory sub of list.  For the cue points
      ERF_note, // .note. -- Mandatory sub of list.  For the cue points
      ERF_ltxt, // .ltxt. -- Mandatory sub of list.  For the cue points
      ERF_plst, // .plst. -- Replay order of a series of cue points
      ERF_smpl, // .smpl. --
      ERF_inst, // .inst.
      ERF_Unk_, // .xxxx. -- not a clue
      ERF_COUNT_OF
      };
    enum eConst{
      FORM_SIZE      =  4,
      SIZE_SIZE      =  4,
      RIFF_MIN_SIZE  = 46, // If there is a single 8 bit number (it must be padded to 2-byte) and a minimum format spec. RIFF : 12 + fmt : 24 + data : 10
      CHUNK_MIN_SIZE = FORM_SIZE + SIZE_SIZE + SIZE_SIZE
      };
    enum eDir{ED_READ, ED_WRITE};
    typedef RiffCk* (*tCreateRiffCkFn)(byte *, eDir, NumSys *);
  protected:
           struct sCk4CCSpec {
      eForm             formNo;  // To begin to undo the MAGIC of the enum order.
      uint              tag;     // The 4 bytes straight from the file, Microsoft calls them FOURCC
      bool              valid;   // I have compared the tag to known tags and found a match
      char             *form;    // A c_str() of the tag.  FOURCC are supposed to be all human-readable, printing characters.
      char             *name;    // Future expansion.  Nice florid prose.
      tCreateRiffCkFn   creator; // Function pointer to the thing that will make it.
      };
    static       sCk4CCSpec *ck4CCSpec; // The secret decoder ring to find which sub-type we're dealing with.
  public:
                            RiffCk       ( byte  *i_pHdr, eDir i_dir, NumSys *i_tns  )                                             ;
    virtual                ~RiffCk       (                   )                                             ;
    static  const llong     PreFetchSize ( void              ) { return        FORM_SIZE + SIZE_SIZE;     };
    static        RiffCk   *RiffFactory  ( byte  *i_pHdr, NumSys *i_ctns  )                                       ;
    virtual const llong     FetchSize    ( void              ) { return size + FORM_SIZE + SIZE_SIZE;     }; // A very special function that only gets what's needed.
            const llong     ChunkSize    ( void              ) { return size +             SIZE_SIZE;     }; // The whole enchilada.  Would pull out gigs if it was the data chunk!
    virtual const llong     LeafSize     ( void              ) { return size + FORM_SIZE + SIZE_SIZE;     }; // Special function.  A container reports its size as it's own stuff plus all of the conts sizes added together
            const byte     *GetImageLoc  ( void              ) { return img;                              };
    virtual       void      ParseBody    ( void              )                                             ;
    virtual       void      BlankBody    ( void          )                                                ;
                  void      SetFileOff   ( llong i_fileOff   ) { fileOff = i_fileOff;       return;       };

                  void      SetOrder     ( llong i_order     ) { order = i_order;           return;          };
                  llong     GetOrder     ( void              ) {                            return order;    };
            const char     *GetHdr       ( void              ) {                            return form;      };
            const llong     GetFileOff   ( void              ) {                            return fileOff;  };
            const llong     GetSize      ( void              ) {                            return (const llong)size;   };
            const llong     GetSubSize   ( void              ) {                            return subSize;  };
    virtual       void      SetRes       ( llong i_res       ) { res = i_res - LeafSize(); return;          }; // The non-hier version is the standard version
            const llong     GetRes       ( void              ) {                            return res;      };

                  bool      IsHier       ( void              ) {                     return isHier;   };
                  void      SetParent    ( RiffCk *i_pParent ) {pParent = i_pParent; return;          };
                  void      SetPred      ( RiffCk *i_pPred   ) {pPred   = i_pPred;   return;          };
                  void      SetSucc      ( RiffCk *i_pSucc   ) {pSucc   = i_pSucc;   return;          };
                  void      SetChild     ( RiffCk *i_pChild  ) {pChild  = i_pChild;  return;          };
                  RiffCk   *GetParent    ( void              ) {                     return pParent;  };
                  RiffCk   *GetPred      ( void              ) {                     return pPred;    };
                  RiffCk   *GetSucc      ( void              ) {                     return pSucc;    };
                  RiffCk   *GetChild     ( void              ) {                     return pChild;  };

    static  const char     *GetForm4CC   ( eForm i_f         ) {            return ck4CCSpec[i_f].form;   };
            const uint      GetTag       ( void              ) {            return tag;                   };
            const bool      IsValid      ( void              ) {            return isValid;               };
  protected:
                  void      ResetCk      ( void              )                                             ;

          static  void      FormExtrct   ( char   *o_c, byte *i_b    )                         ;
          static  void      FormPlace    ( byte *o_b, eForm i_f); //!< The UINT target
          static  void      FormWrite    ( char *o_c, eForm i_f); //!< The UINT target
          static  eForm     FormIntrp    ( byte *i_b );
  private:
          static  void      BuildDecoder ( void      );
  public:

  protected:
    byte           *img;         // pointer to start of chunk in the file image in memory
    eDir            dir;         //!< Is it a read file, a write file ... inquiring minds want to know.
    char            form[5];     // Keyword from original spec - copied from image so that de-residing doesn't lose this
    eForm           formNo;      // For fast comparisons.
    uint            tag;         // The UINT_32 of the fourCC.
    ullong          size;        // Keyword from original spec - will be synthesized as 4 in a RIFF chunk - else straight from file
    ullong          imgSize;     // Keep track of the dirty little secret that the image gets padded by a byte to make it word-granular.
    llong           res;         // residue - For its position in the hierarchical chunk, how much chunk is left
    bool            isValid;     // The chunk has everything in order.  Formatted, length not past end ...
    byte           *pConts;      // pointer to the stuff after the FourCC and the chunk size.
    uint           *pSize;       // All chunks have the second quartet of bytes as the uint size of the chunk.
    off_t           fileOff;     // offset from the beginning, therefore applies to both memory and file
    byte            padByte;     // Just to be nice and complete.

    bool            isHier;      // If the chunk type is hierarchical, this will say so. Can it have sub-chunks
    ullong          subSize;     // The total of the size of the child chunks.  Lets traversal easily check sizing and residue

    NumSys         *tns;
    llong           order;       // Which chunk is it in the original file?
    RiffCk         *pParent;     // The parent chunk, so it will only ever be a RIFF or a LIST chunk
    RiffCk         *pChild;      // This is the first child chunk found in a container chunk.  Go there to traverse the list.
    RiffCk         *pPred;       // The chunk before me in the list of siblings
    RiffCk         *pSucc;       // The chunk after me in the list of siblings
  private:
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkRIFF    : public RiffCk { // very special chunk, not a chunk strictly speaking, that announces the form of the RIFF chunk.
                              // It is possible that the RIFF size makes any given RIFF chunk smaller than the file size.
                              // So there might be multiple roots.  But it might be just garbage at the end
                              // (proprietary, bad coding, unknown next-rev)
  public:
                         CkRIFF        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  )                                                ;
    virtual             ~CkRIFF        (               )                                                ;
           const llong   FetchSize     ( void          ) {                                          return FORM_SIZE + SIZE_SIZE + FORM_SIZE;   };
                 void    ParseBody     ( void          )                                                ;
                 void    BlankBody     ( void          )                                                ;
    static       RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) { return new CkRIFF(i_pHdr, i_dir, i_tns);       };
           const char   *GetForm       ( void          ) {                                          return (const char *)form;                  };
           const llong   ChunkSize     ( void          ) {                                          return FORM_SIZE + size + GetSubSize();     };
           const llong   LeafSize      ( void          ) {                                          return size + FORM_SIZE + SIZE_SIZE;        };
                 void    SetRes        ( llong i_res   ) { res = i_res - GetSubSize() - LeafSize(); return;  };
                 void    SetSubSize    ( uint  i_b     ) { *pSize = i_b;                            return;  };

  protected:
  private:
                 char    formWave[5];
  private: // Specific to the RIFF chunk, which is a root chunk.
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkBext    : public RiffCk {
  public:
                          CkBext    ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  )   ;
                 void     ParseBody ( void          )   ;
          static RiffCk  *Create    ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) { return new CkBext(i_pHdr, i_dir, i_tns);            };
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkFmt   : public RiffCk {
  public:
                   CkFmt         ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  )                                  ;
           void    ParseBody     ( void                 )                                       ;
           void    BlankBody     ( llong i_ch, double i_FS )                                    ;
    static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) { return new CkFmt(i_pHdr, i_dir, i_tns);};
           void    SetNS         ( NumSys *i_tns        ) { tns = i_tns; return               ;};
           uint    GetTagCode    ( void                 ) { return wf->GetTagCode   (        );};
    const  WaveFmt::eFmtTag GetTagType(void             ) { return wf->GetTag       (        );};
    const  char   *GetTagName    ( void                 ) { return wf->GetTagName   (        );};
    const  char   *GetTagName    ( ullong i_tagDex      ) { return wf->GetTagName   (i_tagDex);};
    const  llong   GetCh         ( void                 ) { return wf->GetNumCh     (        );};
    const  double  GetFS         ( void                 ) { return wf->GetFS        (        );};
    const  llong   GetBitDepth   ( void                 ) { return wf->GetBitVSmp   (        );};
    const  llong   GetByteRate   ( void                 ) { return wf->GetByteVSec  (        );};
    const  llong   GetBlkAlign   ( void                 ) { return wf->GetBlkAlign  (        );};
    const  llong   GetExtSize    ( void                 ) { return wf->GetExtSize   (        );};
    const  llong   GetValBits    ( void                 ) { return wf->GetVBitVSmp  (        );};
    const  ulong   GetChMask     ( void                 ) { return wf->GetChMask    (        );};
    const  char   *GetWaveGUID   ( void                 ) { return wf->GetGUIDPreso (        );};
    const  char   *GetGUIDname   ( void                 ) { return wf->GetGUIDname  (        );};
           ullong  SetCh         ( ullong        i_ch   );
           void    SetType       ( NumSys::eType i_type );
           double  SetFS         ( double        i_FS   );
  protected:
  private:
  public:
    WaveFmt *wf;
  protected:
  private:
    WaveFmt::sWaveFmt *pFmtStruct;
    WaveFmt::sWaveFmt *pFmt16Struct;
    WaveFmt::sWaveFmt *pFmt18Struct;
    WaveFmt::sWaveFmt *pFmt40Struct;

    WaveGUID          *pGUID;

    WaveFmt::eFmtTag    fmtCode;
    char    *fmtCodeStr;
    ullong   fmtCh;
    double   fmtFS;      // This works because double will always be able to let an int fit without roundoff
    ullong   fmtByteRate;
    ullong   fmtBlkAlign;
    ullong   fmtBitDepth;
    ullong   fmtExtSize;
    ullong   fmtExtBitDepth;
    ullong   fmtExtChMask;
    byte    *fmtGUID;
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkFact    : public RiffCk {
  public:
                  CkFact    ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  )                 ;
          void    ParseBody ( void )                 ;
   static RiffCk *Create    ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) {            return new CkFact(i_pHdr, i_dir, i_tns);            };
          llong   GetNFrm   ( void ) {return nFrm;}  ;
  private:
    llong          nFrm;
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkData    : public RiffCk {
  public:
                  CkData        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  );
   static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) { return new CkData(i_pHdr, i_dir, i_tns);              };
   const llong    FetchSize     ( void ) { return FORM_SIZE + SIZE_SIZE;     }; // A very special function that only gets what's needed.
          void    ParseBody     ( void          );
          void    BlankBody     ( void          );
          void    SetSize       ( uint i_b      );
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkAfsp    : public RiffCk {
  public:
                   CkAfsp        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  );
    static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) { return new CkAfsp(i_pHdr, i_dir, i_tns);              };
           void    ParseBody     ( void )                                               ;
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkPEAK    : public RiffCk {
  private:
    struct sPeak {
      float          pL;
      uint           offL;
      float          pR;
      uint           offR;
    };
  public:
                   CkPEAK        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  );
    static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) { return new CkPEAK(i_pHdr, i_dir, i_tns);              };
           void    ParseBody     ( void )                                               ;
  private:
           sPeak  *peakInfo;
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkPAD    : public RiffCk {
  public:
                  CkPAD         ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  );
   static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) {            return new CkPAD(i_pHdr, i_dir, i_tns);            };
          void    ParseBody     ( void )                                               ;
  };




//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkInfo    : public RiffCk {
  public:
                  CkInfo        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  );
   static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) {            return new CkInfo(i_pHdr, i_dir, i_tns);            };
          void    ParseBody     ( void )                                               ;
  };







//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
class CkUnk     : public RiffCk {
  public:
                  CkUnk         ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  );
   static RiffCk *Create        ( byte *i_pHdr, eDir i_dir, NumSys *i_tns  ) {            return new CkUnk(i_pHdr, i_dir, i_tns);            };
   const llong    FetchSize     ( void ) { return FORM_SIZE + SIZE_SIZE;     }; // A very special function that only gets what's needed.
          void    ParseBody     ( void )                                               ;
  public:
  };




#endif // __RIFF_CK_HPP_
