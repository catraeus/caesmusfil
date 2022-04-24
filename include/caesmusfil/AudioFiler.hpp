
//=================================================================================================
// Original File Name : AudioFiler.hpp
// Original Author    : duncang
// Creation Date      : Dec 25, 2015
// Copyright          : Copyright © 2015 - 2022 by Catraeus and Duncan Gray
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
    Facilities to do file things.
    Is a manager, and a single file object.
    Is specific to audio world, it will tell you what kind of (audio) file is inside.

    Use Cases:
      A small file is read in completely to a stream image.
      A small file is written completely from a stream image.
      A large file is read in piece-meal.  String-of-pearl like.  and each pearl is discarded after the musician is done.
        An analyzer will do this to provide a summary analysis or shuttle through for views.
      A large file is randomly read, with an arbitrarily sized, located block.
        An analyzer will do this to shuttle through a viewport of it.  SpecAn, Scope, Song Looping.
      All of these are managed by the RiffMgr system.  We don't need residency, start/stop, number types etc.
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
          <callback>Done Working, so the AudioFile object has new information.
        private:
          fd, the ancient posix file descriptor number.
      DOESN't know what the type or organization of the data is.
    Functions:
      Is able to get a contiguous block of num bytes starting at offset and report if it is legal to do so.
    Relationships:
      This object only knows about the header of the audio file formats that it will try to work with.
      It doesn't have any more smarts that.
      Ultimately, the higher levels will need to have their various format types know how to handle files based on headers.

*/
//=================================================================================================

#ifndef __AUDIO_FILER_HPP_
#define __AUDIO_FILER_HPP_

#include <caesbase/CaesTypes.hpp>
#include <caesbase/CaesCallBack.hpp>

#include <caesmusfil/IPC/CtlAudMsg.hpp>

class WaveFiler;

class AudioFiler {
  private:
    enum eConst {
      SIZE_FILE_NAME     =     32768,  //!< ridiculously long file name.
      SIZE_META_IMG      =   1048576   //!< stores everything but the audio data.
    };
    enum eAccess {
      EA_READ,
      EA_WRITE
      };
  public:
    enum eStatus {
      FST_BAD,       // The name on file doesn't name a good file for the purpose at hand.
      FST_IDLE,      // No name, closed, all info is benign
      FST_OPENRD,    // Named, Valid, Readable/Writable as appropriate
      FST_OPENWR,    // Named, Valid, Readable/Writable as appropriate
      FST_WRITEWARN, // Named, Valid, Writable but haven't asked user if it's OK to blow it away
      FST_WRITEOK,   // Named, Valid, Writable but haven't asked user if it's OK to blow it away
      FST_OPEN,      // Named, Valid, Readable/Writable as appropriate
      FST_CLOSED,    // Named, Unknown validity, Unknown readability/writability, known existing TODO make a polling mechanism to check existance etc.
      FST_NOENT,     // The name on file is known to not exist.  This would be true for a file that has the read permission false in its parent folder.
      FST_DIR,       // The name on file is a directory.  Don't do anything with this one.
      FST_NOWR       // The name on file is a file, but you can't write for some reason.
      };
// ====  Administrivia
  private:
                       AudioFiler           ( void             );
                      ~AudioFiler           ( void             );
          void         BuildEnv             ( void             );
          void         BuildSetup           ( void             );
          void         Connect              ( void             );
  public:
// ====  File Level
          eStatus      SetFileNameWrite     ( char *i_f        )  { return SetFileName(i_f, EA_WRITE) ;}; //!< Opens-Closes a file.  Loads the file info into variables for access.  No open.
          eStatus      SetFileNameRead      ( char *i_f        )  { return SetFileName(i_f, EA_READ)  ;}; //!< Opens-Closes a file.  Loads the file info into variables for access.  No open.
          eStatus      GetStatus            ( void             )  {                            return status;   };
    const char        *GetFileName          ( void             )  {                            return fileName; };
    const ullong       GetFileSize          ( void             )  { return fileSize; };
          bool         OnGetFileSize        ( void *io         )  { *((llong *)io) = GetFileSize(); return true;     };
          void         SetAccessRead        ( void             )  { accessType = EA_READ;      return;          };
          void         SetAccessWrite       ( void             )  { accessType = EA_WRITE;     return;          };
// ====  Meta
          void         SetSize              ( long i_arySize   ); //Sets the size of the image on disk, useful for write operations..

          void         Open                 ( void             );
          void         Pitch                ( byte *i_pImg, ullong i_N               );
          bool         Fetch                ( byte *i_pImg, ullong i_N, ullong i_off );
          void         Close                ( void             );
  private:
          void         AcquireFileSize      ( void             );
          eStatus      SetFileName          ( char *i_fileName, eAccess i_a ); //!< Opens-Closes a file.  Loads the file info into variables for access.  No open.
          void         Reset                ( eAccess i_a      );


  public:
  private:
    friend class WaveFiler;

    char         *fileName;
    size_t        fileSize;
    int           fd;
// ====  Machine state
    eStatus       status;
    eAccess       accessType;


    static AudioFiler  *inst;

// ====  Interconn
    CtlAudMsg         *ctAm;
  public:
    CbV               *CbChangeFile;
};
#endif // __AUDIO_FILER_HPP_
