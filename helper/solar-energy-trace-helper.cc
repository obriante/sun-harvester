/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Universita' Mediterranea di Reggio Calabria (UNIRC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Orazio Briante <orazio.briante@unirc.it>
 */

#include "solar-energy-trace-helper.h"

#include <iostream>
#include <iterator>

#include <ns3/assert.h>
#include <ns3/nstime.h>
#include <ns3/simulator.h>

namespace ns3 {

void
SolarEnergyTraceHelper::EnableLogComponents (enum LogLevel level)
{
  LogComponentEnableAll (LOG_PREFIX_TIME);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnable ("EnergyHarvester", level);
  LogComponentEnable ("SolarEnergyHarvester", level);
}

//
// Public API
//
void
SolarEnergyTraceHelper::EnableAscii (Ptr<OutputStreamWrapper> stream, EnergyHarvesterContainer n)
{
  for (EnergyHarvesterContainer::Iterator i = n.Begin (); i != n.End (); ++i)
    {
      Ptr<SolarEnergyHarvester> dev = DynamicCast<SolarEnergyHarvester> (*i);
      NS_ASSERT (dev);
      EnableAsciiInternal (stream, dev);
    }
}

void
SolarEnergyTraceHelper::DefaultHarvestedPowerSinkWithContext (Ptr<OutputStreamWrapper> stream, std::string context, double previous, double current)
{
  *stream->GetStream () << "+ " << Simulator::Now ().GetSeconds () << " " << context << " " << current << " [W]" << std::endl;
}

void
SolarEnergyTraceHelper::DefaultTotalEnergyHarvestedSinkWithContext (Ptr<OutputStreamWrapper> stream, std::string context, double previous, double current)
{
  *stream->GetStream () << "+ " << Simulator::Now ().GetSeconds () << " " << context << " " << current << " [J]" << std::endl;
}

} /* namespace ns3 */
