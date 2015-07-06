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
 * Author: Giovanni Benigno <giovanni.benigno.954@studenti.unirc.it>
 *         Orazio Briante <orazio.briante@unirc.it>
 */

#ifndef SOLAR_ENERGY_HARVESTER_HELPER_H
#define SOLAR_ENERGY_HARVESTER_HELPER_H

#include "ns3/energy-harvester-helper.h"
#include "ns3/solar-energy-trace-helper.h"
#include "ns3/energy-source.h"
#include "ns3/node.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Creates a SolarEnergyHarvester object.
 */
class SolarEnergyHarvesterHelper : public EnergyHarvesterHelper,
                                   public SolarEnergyTraceHelper
{
public:
  SolarEnergyHarvesterHelper (void);
  ~SolarEnergyHarvesterHelper (void);

  void Set (std::string name, const AttributeValue &v);

  virtual void EnableAsciiInternal (Ptr<OutputStreamWrapper> stream, Ptr<SolarEnergyHarvester> nd);

private:
  virtual Ptr<EnergyHarvester> DoInstall (Ptr<EnergySource> source) const;

private:
  ObjectFactory m_solarEnergyHarvester;

};

} // namespace ns3

#endif /* defined(SOLAR_ENERGY_HARVESTER_HELPER_H) */
