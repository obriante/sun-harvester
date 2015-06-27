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
#include "ns3/sun-harvester.h"
#include "ns3/sun-helper.h"
#include "ns3/sun-harvester-helper.h"
#include "ns3/sun.h"
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

Ptr<SunEnergyHarvester> sunHarvesterPtr;

NS_LOG_COMPONENT_DEFINE ("SunHarvesterExample");

/* Trace function for remaining energy at node. */
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  tm date = sunHarvesterPtr->GetDate ();

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
  tm date = sunHarvesterPtr->GetDate ();

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
  tm date = sunHarvesterPtr->GetDate ();

  char buffer[100];
  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S;", &date);
  std::cout << buffer << " Total energy harvested by harvester = " << TotalEnergyHarvested << " [J]" << std::endl;

  strftime (buffer, 80,"%Y;%m;%d;%H;%M;%S;", &date);
  energyCsv << buffer << TotalEnergyHarvested << ";" << std::endl;
}


int
main (int argc, char *argv[])
{

  powerCsv.open ("HarvestedPower.csv");
  powerCsv << "year;month;day;hour;min;sec;harvestedPower;" << ";" << std::endl;

  batteryCsv.open ("TotalEnergyHarvested.csv");
  batteryCsv << "year;month;day;hour;min;sec;RemainingEnergy;" << ";" << std::endl;

  energyCsv.open ("RemainingEnergy.csv");
  energyCsv << "year;month;day;hour;min;sec;TotalEnergyHarvested;" << ";" << std::endl;

  double startTime = 0.0;
  CommandLine cmd;
  cmd.AddValue ("start", "Simulation start time", startTime);
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

  /*********** Install Sun *************/
  SunHelper SunHelper;
  SunHelper.Set ("AvgInsolation", DoubleValue (8.63)); //From the radiation table
  SunHelper.Set ("Longitude", DoubleValue (15.661)); //degree
  SunHelper.Set ("Latitude", DoubleValue (38.11));   //degree

  Ptr<Sun> SunP = SunHelper.Install ();

  /*********** Install a Sun Harvester *************/
  SunEnergyHarvesterHelper SunHarvesterHelper;
  SunHarvesterHelper.Set ("StartAt", StringValue ("2004-06-21 00:00:00"));
  SunHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (harvestingUpdateInterval)));
  SunHarvesterHelper.Set ("PanelTiltAngle",DoubleValue (0));    //degree
  SunHarvesterHelper.Set ("PanelAzimuthAngle",DoubleValue (0));     //degree
  SunHarvesterHelper.Set ("PanelDimension",DoubleValue (1e-4));     // 1 cm^2
  SunHarvesterHelper.Set ("SolarCellEfficiency",DoubleValue (8));     //8%
  SunHarvesterHelper.Set ("DCDCEfficiency",DoubleValue (90));       //90%
  SunHarvesterHelper.Set ("DiffusePercentage",DoubleValue (10));     //10%
  SunHarvesterHelper.Set ("Sun",PointerValue (SunP));


  EnergyHarvesterContainer harvesters = SunHarvesterHelper.Install (sources);
  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (0));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

  sunHarvesterPtr = DynamicCast<SunEnergyHarvester> (harvesters.Get (0));
  sunHarvesterPtr->TraceConnectWithoutContext ("HarvestedPower", MakeCallback (&HarvestedPower));
  sunHarvesterPtr->TraceConnectWithoutContext ("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));


  Simulator::Stop (Days (1));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
