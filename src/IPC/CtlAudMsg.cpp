
//=================================================================================================
// Original File Name : CtlAudMsg.cpp
// Original Author    : duncang
// Creation Date      : 2017-02-11T23:05:19,630339093+00:00
// Copyright          : Copyright © 2017 - 2022 by Catraeus and Duncan Gray
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

#include <caesmusfil/IPC/CtlAudMsg.hpp>

CtlAudMsg *CtlAudMsg::ctAm = NULL;

            CtlAudMsg::CtlAudMsg(void) {
  MSU_SizeChange = NULL;
  return;
}
            CtlAudMsg::~CtlAudMsg(void) {
  return;
}

CtlAudMsg  *CtlAudMsg::GetInstance(void) {
  if(ctAm == NULL)
    ctAm = new CtlAudMsg();
  return ctAm;
}
void        CtlAudMsg::MRD_SizeChange(ullong i_N) {
  if(MSU_SizeChange != NULL) MSU_SizeChange->Execute((void *)(&i_N));
  return;
}

