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
#ifndef HELPER_CAPILLARY_ENERGY_TRACE_HELPER_H_
#define HELPER_CAPILLARY_ENERGY_TRACE_HELPER_H_

#include <string>

#include <ns3/energy-harvester-container.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/ptr.h>
#include <ns3/solar-energy-harvester.h>

namespace ns3 {

/*
 *
 */
class SolarEnergyTraceHelper
{
public:
  SolarEnergyTraceHelper ()
  {
  }
  virtual ~SolarEnergyTraceHelper ()
  {
  }

  /**
   * Helper to enable all Sun Harvester log components with one statement
   */
  void EnableLogComponents (enum LogLevel level);


  /**
   * @brief Enable ascii trace output on each device (which is of the
   * appropriate type) in the nodes provided in the container.
   *
   * @param stream An OutputStreamWrapper representing an existing file to use
   *               when writing trace data.
   * \param n container of DeviceEnergyModel.
   */
  void EnableAscii (Ptr<OutputStreamWrapper> stream, EnergyHarvesterContainer n);

  virtual void EnableAsciiInternal (Ptr<OutputStreamWrapper> stream, Ptr<SolarEnergyHarvester> nd) = 0;

  static void DefaultHarvestedPowerSinkWithContext (Ptr<OutputStreamWrapper> file, std::string context, double previous, double current);
  static void DefaultTotalEnergyHarvestedSinkWithContext (Ptr<OutputStreamWrapper> file, std::string context, double previous, double current);

};

} /* namespace ns3 */

#endif /* HELPER_CAPILLARY_ENERGY_TRACE_HELPER_H_ */
