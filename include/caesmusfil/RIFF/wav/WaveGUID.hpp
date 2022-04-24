
//=================================================================================================
// Original File Name : FileWave.h
// Original Author    : duncang
// Creation Date      : March 17, 2002
// Copyright          : Copyright © 2002 - 2022 by Catraeus and Duncan Gray
//
// Description:
/*
    That nasty old GUID that Microsoft (or somebody) invented to get ages of universe birthday
      crash problems to go away.  HERE we are only laying down the GUIDs that were allocated
      for the RIFF content data types in the extended format spec from Microsoft.
*/
//=================================================================================================


#ifndef __WAVE_GUID_HPP_
#define __WAVE_GUID_HPP_

#include <caesbase/CaesTypes.hpp>

class WaveGUID {
  public:
    enum eType {
      EG_UNK        ,
      EG_PCM        ,
      EG_ADPCM_MS   ,
      EG_IEEE_FLOAT ,
      EG_CVSD       ,
      EG_ALAW       ,
      EG_ULAW       ,
      EG_ADPCM_IMA  ,
      EG_MP3        ,
      EG_EXTENSIBLE ,
      EG_EXPMTL     ,
      EG_COUNT_OF
      };
    union GUID {
      struct __attribute__((__packed__)) lGUID {
        public:
          uint    dd;
          ushort  d1;
          ushort  d2;
          byte    da[2];
          byte    db[6];
        } s;
      byte    bytes[16];
      };
    struct GRec {
      eType  type;
      GUID   id;
      char  *preso;
      char  *name;
    };
  private:
    enum eConst {
      EC_ID_SIZE   =  16,
      EC_NAME_SIZE = 256
    };
  public:
                              WaveGUID        (                                   )                       ;
    virtual                  ~WaveGUID        (                                   )                       ;
                  void        SetID           ( byte   i_bytes[]                  )                       ;
                  void        SetType         ( eType  i_type                     )                       ;
                  bool        Equals          ( byte  *i_id                       )                       ;
            const char       *GetPreso        ( void                              ) { return gRec.preso; };
            const char       *GetName         ( void                              ) { return gRec.name;  };
            const eType       GetType         ( void                              ) { return gRec.type;  };
  private:
                  void        BuildGRec       ( GRec  *io_gRec, eType i_type, byte *i_id, char *i_name)    ;
                  void        ToString        ( GUID  *i_id,   char *s            )                       ;
          //        void        ByteToHexString ( char  *t,      byte i, int dig    )                       ;
  public:
    static        GRec       *gList;
  private:
                  GRec        gRec;
    };
#endif //__WAVEGUID_HPP_
