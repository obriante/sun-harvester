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
 * Author: Giovanni Benigno <giovanni.benigno.954@studenti.unirc.it>,
 *         Orazio Briante <orazio.briante@unirc.it>
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/energy-module.h"
#include "ns3/internet-module.h"
#include "ns3/sun-harvester-module.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include "ns3/ptr.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;

std::ofstream powerCsv;
std::ofstream energyCsv;
std::ofstream batteryCsv;

Ptr<SolarEnergyHarvester> solarHarvesterPtr;

NS_LOG_COMPONENT_DEFINE ("SolarHarvesterExample");

/* Trace function for remaining energy at node. */
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
	tm date = solarHarvesterPtr->GetDate ();

	  char buffer[100];
	  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S;", &date);

	  std::cout << buffer << " Current remaining energy = " << remainingEnergy << " [J]" << std::endl;

	  strftime (buffer, 80,"%Y;%m;%d;%H;%M;%S;", &date);
	  batteryCsv << buffer << remainingEnergy << ";" << std::endl;
}

/* Trace function for the power harvested by the energy harvester. */
void
HarvestedPower (double oldValue, double harvestedPower)
{
	tm date = solarHarvesterPtr->GetDate ();

	  char buffer[100];
	  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S;", &date);
	  std::cout << buffer << " Current harvested power = " << harvestedPower << " [W]" << std::endl;

	  strftime (buffer, 80,"%Y;%m;%d;%H;%M;%S;", &date);
	  powerCsv << buffer << harvestedPower << ";" << std::endl;
}

/* Trace function for the total energy harvested by the node. */
void
TotalEnergyHarvested (double oldValue, double TotalEnergyHarvested)
{
	tm date = solarHarvesterPtr->GetDate ();

	  char buffer[100];
	  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S;", &date);
	  std::cout << buffer << " Total energy harvested by harvester = " << TotalEnergyHarvested << " [J]" << std::endl;

	  strftime (buffer, 80,"%Y;%m;%d;%H;%M;%S;", &date);
	  energyCsv << buffer << TotalEnergyHarvested << ";" << std::endl;
}

int
main (int argc, char *argv[])
{
  /* LogComponentEnable ("Config", LOG_LEVEL_DEBUG);
   LogComponentEnable ("BasicEnergySource", LOG_LEVEL_DEBUG);
   LogComponentEnable ("SolarEnergyHarvester", LOG_LEVEL_DEBUG);
   LogComponentEnable ("SolarEnergyHarvesterTestSuite", LOG_LEVEL_DEBUG);*/

	powerCsv.open ("HarvestedPower.csv");
	  powerCsv << "year;month;day;hour;min;sec;harvestedPower;" << ";" << std::endl;

	  batteryCsv.open ("TotalEnergyHarvested.csv");
	  batteryCsv << "year;month;day;hour;min;sec;RemainingEnergy;" << ";" << std::endl;

	  energyCsv.open ("RemainingEnergy.csv");
	  energyCsv << "year;month;day;hour;min;sec;TotalEnergyHarvested;" << ";" << std::endl;

  double startTime = 0.0;
  CommandLine cmd;
  cmd.AddValue ("startTime", "Simulation start time", startTime);
  cmd.Parse (argc, argv);
  NodeContainer c;
  c.Create (1);     // create 1 nodes

  /* Energy Harvester variables */
  double harvestingUpdateInterval = 1;  // seconds

  /* energy source */
  BasicEnergySourceHelper basicSourceHelper;

  /* configure energy source */
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (1.0));

  /* install source */
  EnergySourceContainer sources = basicSourceHelper.Install (c);


  /*********** Install a Sun Harvester *************/
  SolarEnergyHarvesterHelper solarHarvesterHelper;
  solarHarvesterHelper.Set ("StartAt", StringValue ("2004-06-21 00:00:00"));
  solarHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
  solarHarvesterHelper.Set ("PanelTiltAngle",DoubleValue (0));//degree
  solarHarvesterHelper.Set ("PanelAzimuthAngle",DoubleValue (0)); //degree
  solarHarvesterHelper.Set ("PanelDimension",DoubleValue (1e-4)); // 1 cm^2
  solarHarvesterHelper.Set ("SolarCellEfficiency",DoubleValue (8)); //8%
  solarHarvesterHelper.Set ("DCDCEfficiency",DoubleValue (90));   //90%
  solarHarvesterHelper.Set ("DiffusePercentage",DoubleValue (10)); //10%
  solarHarvesterHelper.Set ("Longitude", DoubleValue (15.661)); //degree
  solarHarvesterHelper.Set ("Latitude", DoubleValue (38.11));   //degree

  EnergyHarvesterContainer harvesters = solarHarvesterHelper.Install (sources);
  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (0));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

  solarHarvesterPtr = DynamicCast<SolarEnergyHarvester> (harvesters.Get (0));
  solarHarvesterPtr->TraceConnectWithoutContext ("HarvestedPower", MakeCallback (&HarvestedPower));
  solarHarvesterPtr->TraceConnectWithoutContext ("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));


  std::cout << solarHarvesterPtr << std::endl;

  Simulator::Stop (Days (1));
  Simulator::Run ();
  Simulator::Destroy ();

  powerCsv.close ();
  energyCsv.close ();
  batteryCsv.close ();

  return 0;
}
