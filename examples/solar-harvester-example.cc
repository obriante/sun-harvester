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
#include "ns3/config-store-module.h"
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

std::string savePath = "src/sun-harvester/examples/";

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
  bool debug = false;

  std::string configFile = savePath + "sun-harvester-example.xml";

  CommandLine cmd;
  cmd.AddValue ("debug", "Flag to enable/disable debug", debug);
  cmd.AddValue ("config", "The configuration file name", configFile);
  cmd.Parse (argc, argv);

  // input config store: txt format
  Config::SetDefault ("ns3::ConfigStore::Filename", StringValue (configFile));
  Config::SetDefault ("ns3::ConfigStore::FileFormat", StringValue ("Xml"));
  Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Load"));

  ConfigStore config;
  config.ConfigureDefaults ();



  SolarEnergyHarvesterHelper solarHarvesterHelper;

  if (debug)
    {
      solarHarvesterHelper.EnableLogComponents (LOG_LEVEL_DEBUG);
      LogComponentEnable ("Config", LOG_LEVEL_DEBUG);
      LogComponentEnable ("BasicEnergySource", LOG_LEVEL_DEBUG);
      LogComponentEnable ("SolarHarvesterExample", LOG_LEVEL_DEBUG);
    }

  powerCsv.open (std::string (savePath + "HarvestedPower.csv").c_str ());
  powerCsv << "year;month;day;hour;min;sec;harvestedPower;" << ";" << std::endl;

  batteryCsv.open (std::string (savePath + "TotalEnergyHarvested.csv").c_str ());
  batteryCsv << "year;month;day;hour;min;sec;RemainingEnergy;" << ";" << std::endl;

  energyCsv.open (std::string (savePath + "RemainingEnergy.csv").c_str ());
  energyCsv << "year;month;day;hour;min;sec;TotalEnergyHarvested;" << ";" << std::endl;

  NodeContainer c;
  c.Create (1);       // create 1 nodes

  /* energy source */
  BasicEnergySourceHelper basicSourceHelper;
  EnergySourceContainer sources = basicSourceHelper.Install (c);


  /*********** Solar Harvester *************/

  EnergyHarvesterContainer harvesters = solarHarvesterHelper.Install (sources);
  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (0));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

  solarHarvesterPtr = DynamicCast<SolarEnergyHarvester> (harvesters.Get (0));
  solarHarvesterPtr->TraceConnectWithoutContext ("HarvestedPower", MakeCallback (&HarvestedPower));
  solarHarvesterPtr->TraceConnectWithoutContext ("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));

  Simulator::Stop (Days (1));

  std::cout << solarHarvesterPtr << std::endl;
  Simulator::Run ();
  Simulator::Destroy ();

  powerCsv.close ();
  energyCsv.close ();
  batteryCsv.close ();

  return 0;
}
