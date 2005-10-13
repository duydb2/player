/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2000-2003
 *     Brian Gerkey, Kasper Stoy, Richard Vaughan, & Andrew Howard
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * $Id$
 */

#include "playerc++.h"

using namespace PlayerCc;

DioProxy::DioProxy(PlayerClient *aPc, uint aIndex)
  : ClientProxy(aPc, aIndex),
  mDevice(NULL)
{
  Subscribe(aIndex);
  // how can I get this into the clientproxy.cc?
  // right now, we're dependent on knowing its device type
  mInfo = &(mDevice->info);
}

DioProxy::~DioProxy()
{
  Unsubscribe();
}

void
DioProxy::Subscribe(uint aIndex)
{
  boost::mutex::scoped_lock lock(mPc->mMutex);
  mDevice = playerc_dio_create(mClient, aIndex);
  if (NULL==mDevice)
    throw PlayerError("DioProxy::DioProxy()", "could not create");

  if (0 != playerc_dio_subscribe(mDevice, PLAYER_OPEN_MODE))
    throw PlayerError("DioProxy::DioProxy()", "could not subscribe");
}

void
DioProxy::Unsubscribe()
{
  assert(NULL!=mDevice);
  boost::mutex::scoped_lock lock(mPc->mMutex);
  playerc_dio_unsubscribe(mDevice);
  playerc_dio_destroy(mDevice);
  mDevice = NULL;
}

std::ostream& std::operator << (std::ostream &os, const PlayerCc::DioProxy &c)
{
  os << "#DIO (" << c.GetInterface() << ":" << c.GetIndex() << ")" << std::endl;
  for (unsigned int i = 0; i < c.GetCount(); ++i)
    os << i << ": " << ((c.GetDigin() << i) & 0x80000000 ? "1" : "0");
  return os;
}

/// Set the output
void DioProxy::SetOutput(uint aCount, uint32_t aDigout)
{
  boost::mutex::scoped_lock lock(mPc->mMutex);
  if (0 != playerc_dio_set_output(mDevice, aCount, aDigout))
    throw PlayerError("DioProxy::SetOutput()", "error setting output");
  return;
}