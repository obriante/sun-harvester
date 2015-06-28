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

#include "solar-energy-harvester-helper.h"
#include "ns3/pointer.h"


#include "ns3/energy-harvester.h"

namespace ns3 {

SolarEnergyHarvesterHelper::SolarEnergyHarvesterHelper (void)
{
  m_solarEnergyHarvester.SetTypeId ("ns3::SolarEnergyHarvester");
}

SolarEnergyHarvesterHelper::~SolarEnergyHarvesterHelper (void)
{
}

void
SolarEnergyHarvesterHelper::Set (std::string name, const AttributeValue &v)
{
  m_solarEnergyHarvester.Set (name, v);
}

Ptr<EnergyHarvester>
SolarEnergyHarvesterHelper::DoInstall (Ptr<EnergySource> source) const
{
  NS_ASSERT (source != 0);
  Ptr<Node> node = source->GetNode ();

  // Create a new Basic Energy Harvester
  Ptr<EnergyHarvester> harvester = m_solarEnergyHarvester.Create<EnergyHarvester> ();
  NS_ASSERT (harvester != 0);

  // Connect the Basic Energy Harvester to the Energy Source
  source->ConnectEnergyHarvester (harvester);
  harvester->SetNode (node);
  harvester->SetEnergySource (source);
  return harvester;
}

} // namespace ns3
