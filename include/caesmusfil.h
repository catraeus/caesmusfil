
//=================================================================================================
// Original File Name : caesmusfile.h
// Original Author    : duncang
// Creation Date      : March 17, 2002
// Copyright          : Copyright © 2002 - 2015 by Catraeus and Duncan Gray
//
//=================================================================================================
/*
This file is part of the caesmusfil library.

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

    do {
      read in min(fileLen - numRead, 256k) as numRead
      do a first-pass chunk walk, bring them into an array of chunk classes
      Divine the format arcana
      if fmt doesn't show the right encoding, then barf.
      } while(weStillDontHaveA_DataChunkSpec)

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
//=================================================================================================


#ifndef __WAVE_FILE_HPP__
#define __WAVE_FILE_HPP__


#include <caes/CaesTypes.hpp>
#include <caes/CaesCallBack.hpp>

#include <caesmusfil/AudioFiler.hpp>

#include <caesmusfil/IPC/CtlAudMsg.cpp>
#include <caesmusfil/Sig/NumSys.hpp>
#include <caesmusfil/Sig/Signal.hpp">

#include <caesmusfil/RIFF/RiffMgr.hpp>
#include <caesmusfil/RIFF/RiffCk.hpp>
#include <caesmusfil/RIFF/wav/WaveFiler.hpp>
#include <caesmusfil/RIFF/wav/WaveFmt.hpp>
#include <caesmusfil/RIFF/wav/WaveGUID.hpp>

#include <caesmusfil/raw/RawFile.hpp>



#endif // _WAVEFILE_HPP__
