
//=================================================================================================
// Original File Name : FileWave.hpp
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
/* Top of the Wave File class group

______
    Meta Image:
    -----------
    There is a block resident in memory that is an image of what disk has.
    I will only deal with PCM and IEEE float types.
    I will only deal with files with a single data chunk.
    The data chunk will sit in the imageBlock but truncated, most likely.
    The data chunk conts will reside in memory as scalar vectors, one per channel, where their lengths
        have to be equal, their types have to be the same, their SRs have to be the same etc.
    I will pull in cue, info, list and afsp chunks and never do anything with them, simply parsed
        and disregard.
    I will write only the bare-bones wave file.  Nothing but data.
    The block will be identical to what will be written.  It may have dumped all of those other
        chunk types, so it ISN'T like the read file.

    ______
    Order of events:
    ----------------
        READING
    <pre>
        do {
          read in min(fileLen - numRead, 256k) as numRead
          do a first-pass chunk walk, bring them into an array of chunk classes
          Divine the format arcana
          if fmt doesn't show the right encoding, then barf.
          } while(weStillDontHaveA_DataChunkSpec)
    </pre>
          If FACT and chunk sizes don't add up, barf.
          Go through the combinatorics of formats, facts, etc. to assure validity.

          Pull in the data chunk, converting to double. (This enacts a potential LSBRMS noise generation on recycling.)

    ______
    Definitions
    -----------

    sample - A single particle of audio data - smp

    frame - A ch-tuple of samples - frm

    channel - A 1 x numFrames vector of samples - ch

    program - a ch x numFrames bundle of channels that need to be associated with words like stereo, mix or surround.

    FS - The most appropriate word is frameRate, but sampleRate means the same thing.

    Byte Rates, Bit Rates, Bird, Turd and Schmurd rates are cruft (only for compression) and will be derived and not discussed.

    numFrames - Is obvious, therefore - N

    numSamples - Is ambiguous and WILL NOT BE USED, ya hear!

    numBytes - is real obvious too.  numBytes of a chunk is "size"  Really only machine-relevant and will only be 8-bit bytes.


    off - The byte location from zero, zero based, of an interesting thing within a block of stuff

    ptr - Is a memory location and can be in int, long, ulong or whatever. - pXyz

    locFile - The byte location from zero, zero based, of an interesting thing within THE FILE

    locMem - The byte location from zero, zero based, of an interesting thing in the META IMAGE


    ______
    How to specify a WAVE file:
    --------------------------
    The full spec allows for so many types, there was obviously a feeding frenzy back in the day.
    We will only use float and int

    sample type as enum in {iInt16, Int24 and Int32, float, double}
     (IEEE/c float/double spec)

    We will only left-justify up to 32-bits when bringing in smaller data types
    We will only be Linux/Windows endian (little.)
    NOTE! Normalization is to (2^width - 1) - 1.  So, e.g., a 16-bit small integer will transform 1.0 in float or double into 32767 in Int16.

    numFrames as integer in bytes

    channels as integer in channels.

    ______
    The four classes
    ----------------
      1.  WaveFiler
      2.  RiffMgr (and a few derived)
      3.  sFmt (and a few derived)
      4.  WaveGUID

      The big question for the day is how to keep track of the cached block memory pointer
        and offset vs. the on-disk offset in the file.

      As usual, just by writing that down, I got it.

        - The meta block is a single uniform 32k of bytes.

        - There will be a pointer to memory for each chunk, in bytes.

        - The WaveFiler will assure that the state of the data will be either fully analyzed or invalid.  Intermediate reading is forbidden

        - The .data. chunk will ALWAYS NO EXCEPTION be NOT in the imageBlock.  A second call will suck in the data.
          Just like the rest of the blocks, the WaveFiler will assure that data is resident or it is called invalid.
*/
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
      Traverse
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

    There are four levels of inspection:
      File   -- This is straight out of the book for a posix (et al.) filesystem
                Resides in WaveFiler
      Chunk  -- This is straight out of the book for a MS definition, to the extent that I can find the breadcrumbs
                Resides in RiffCk
      Meta   -- This is also out of the MS def, but is my check of the non-orthogonal data that all the different formats specify.
                Resides in NumSys
      Audio  -- What we're really here for.  What is the number system, array storage method, length, FS, channels etc.
*/
//=================================================================================================


#ifndef __WAVE_FILER_HPP__
#define __WAVE_FILER_HPP__

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <caesbase/CaesTypes.hpp>
#include <caesbase/CaesCallBack.hpp>
#include <caesmusfil/Sig/Signal.hpp>

#include <caesmusfil/AudioFiler.hpp>
#include <caesmusfil/Sig/NumSys.hpp>
#include <caesmusfil/RIFF/RiffMgr.hpp>


class WaveFiler {
  private:
    private:
    enum eConst {
      SIZE_FILE_NAME     =   32768, // ridiculously long file name
      SIZE_FRAME_FETCH   =   32768, // Size in samples, regardless of channels
      SIZE_MAX_CH        =       4,
      SIZE_INT24         =       3  // Int24 isn't a native size format.
      };
    enum eAccess {
      EA_READ, // File Read Access
      EA_WRITE // File Write Access
      };
    struct sFileSpec {
      uint  len;
      char *match;
      void *cb;
    };
// ====  Administrivia
  public:
                       WaveFiler            ( Signal *i_sig );
    virtual           ~WaveFiler            (void);
  public:
           Signal     *GetSignal            ( void        ) {return sig;};
           RiffMgr    *GetRiffMgr           ( void        ) {return trf;};
           AudioFiler *GetAudioFiler        ( void        ) {return taf;};
           NumSys     *GetNumSys            ( void        ) {return tns;};
           void        OnFileChange         (void);
           void        Reset                (void);
//======
// File Level
//======
// Meta
           double      GetDuration          (void);

           void        ResAry               (void);
           void        WriteAry             (void);

  private:
           ullong      ResAryInt08mono      (void);
           ullong      ResAryInt08stereo    (void);
           ullong      ResAryInt16mono      (void);
           ullong      ResAryInt16stereo    (void);
           ullong      ResAryInt24          (const ullong numCh);
           ullong      ResAryInt32          (const ullong numCh);
           ullong      ResAryIEEE32         (const ullong numCh);
           ullong      ResAryIEEE64mono     (void);
           ullong      ResAryIEEE64stereo   (void);

           void        WriteChunks          (void);
           void        WriteAryInt08mono    (void);
           void        WriteAryInt08stereo  (void);
           void        WriteAryInt16mono    (void);
           void        WriteAryInt16stereo  (void);
           void        WriteAryInt24mono    (void);
           void        WriteAryInt24stereo  (void);
           void        WriteAryInt32mono    (void);
           void        WriteAryInt32stereo  (void);
           void        WriteAryIEEE32mono   (void);
           void        WriteAryIEEE32stereo (void);
           void        WriteAryIEEE64mono   (void);
           void        WriteAryIEEE64stereo (void);



  public:
    //==============================
    // Interface
    //CbV          *cbMsgSendActTwf;
  private:
    static const char   hdTstRiff[];
    static const char   hdTstMp3[];
    static const char   hdTstFlac[];
    static sFileSpec  **fileSpecList;
    static const uint   numFileTypes = 3;
           Signal       *sig;
           NumSys       *tns;
           RiffMgr      *trf;
           AudioFiler   *taf;

    //==============================
    // Machine state
           eAccess       accessType;


  };

#endif // _WAVEFILE_HPP__
