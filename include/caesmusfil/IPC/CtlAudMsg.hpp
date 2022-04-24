
//=================================================================================================
// Original File Name : CtlAudMsg.hpp
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
/*    Messaging, using callbacks, very specific to Audio Files.
        Avoids dbus delays, needs thread hardening.
*/
//=================================================================================================


#ifndef ___CTL_AUD_MSG_HPP_
#define ___CTL_AUD_MSG_HPP_

#include <caesbase/CaesCallBack.hpp>
#include <caesbase/CaesTypes.hpp>

class CtlAudMsg {
  private:
                      CtlAudMsg(void);
    virtual          ~CtlAudMsg(void);
  public:
    static CtlAudMsg *GetInstance(void);
    void              MRD_SizeChange(ullong i_N);
  private:
  public:
           CbV       *MSU_SizeChange;
  private:
    static CtlAudMsg *ctAm;
};
#endif // ___CTL_AUD_MSG_HPP_
