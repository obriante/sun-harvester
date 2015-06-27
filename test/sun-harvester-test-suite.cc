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

#include <ns3/basic-energy-source.h>
#include <ns3/basic-energy-source-helper.h>
#include <ns3/callback.h>
#include <ns3/double.h>
#include <ns3/integer.h>
#include <ns3/log.h>
#include <ns3/log-macros-disabled.h>
#include <ns3/node-container.h>
#include <ns3/nstime.h>
#include <ns3/pointer.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>
#include <ns3/sun.h>
#include <ns3/sun-harvester-module.h>
#include <ns3/sun-helper.h>
#include <ns3/test.h>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("sun-harvester-model");

/* Trace function for remaining energy at node. */
void
RemainingEnergy (double oldValue, double remainingEnergy)
{
  std::cout << Simulator::Now ().GetSeconds ()
            << "s Current remaining energy = " << remainingEnergy << "J" << std::endl;
}

/* Trace function for the power harvested by the energy harvester. */
void
HarvestedPower (double oldValue, double harvestedPower)
{
  std::cout << Simulator::Now ().GetSeconds ()
            << "s Current harvested power = " << harvestedPower << " W" << std::endl;
}

/* Trace function for the total energy harvested by the node. */
void
TotalEnergyHarvested (double oldValue, double TotalEnergyHarvested)
{
  std::cout << Simulator::Now ().GetSeconds ()
            << "s Total energy harvested by harvester = "
            << TotalEnergyHarvested << " J" << std::endl;
}


class SunHarvesterModelTest : public TestCase
{
public:
  SunHarvesterModelTest ();
  virtual ~SunHarvesterModelTest ();

private:
  virtual void DoRun (void);
};

// Add some help text to this case to describe what it is intended to test
SunHarvesterModelTest::SunHarvesterModelTest ()
  : TestCase ("Sun Harvester test case")
{
}

SunHarvesterModelTest::~SunHarvesterModelTest ()
{
}


void
SunHarvesterModelTest::DoRun (void)
{
  LogComponentEnable ("Config", LOG_LEVEL_ALL);

  NS_LOG_UNCOND ("Started..");

  Time stopTime = Days (1);

  NodeContainer c;
  c.Create (1);     // create 1 nodes

  /* energy source */
  BasicEnergySourceHelper basicSourceHelper;

  /* configure energy source */
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (1.0));

  /* install source */
  EnergySourceContainer sources = basicSourceHelper.Install (c);

  /*********** Install Sun *************/
  SunHelper SunHelper;
  SunHelper.Set ("AvgInsolation", DoubleValue (8.63));   //From the radiation table
  SunHelper.Set ("Longitude", DoubleValue (15.661));   //degree
  SunHelper.Set ("Latitude", DoubleValue (38.11));     //degree

  Ptr<Sun> SunP = SunHelper.Install ();

  NS_LOG_UNCOND (SunP);

  /*********** Install a Sun Harvester *************/
  SunEnergyHarvesterHelper SunHarvesterHelper;
  SunHarvesterHelper.Set ("StartAt", StringValue ("2004-06-21 00:00:00"));
  SunHarvesterHelper.Set ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (1)));
  SunHarvesterHelper.Set ("PanelTiltAngle",DoubleValue (0));  //degree
  SunHarvesterHelper.Set ("PanelAzimuthAngle",DoubleValue (0));   //degree
  SunHarvesterHelper.Set ("PanelDimension",DoubleValue (1e-4));   // 1 cm^2
  SunHarvesterHelper.Set ("SolarCellEfficiency",DoubleValue (8));   //8%
  SunHarvesterHelper.Set ("DCDCEfficiency",DoubleValue (90));     //90%
  SunHarvesterHelper.Set ("DiffusePercentage",DoubleValue (10));   //10%
  SunHarvesterHelper.Set ("Sun",PointerValue (SunP));


  EnergyHarvesterContainer harvesters = SunHarvesterHelper.Install (sources);

  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (0));
  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));

  Ptr<SunEnergyHarvester> basicHarvesterPtr = DynamicCast<SunEnergyHarvester> (harvesters.Get (0));
  basicHarvesterPtr->TraceConnectWithoutContext ("HarvestedPower", MakeCallback (&HarvestedPower));
  basicHarvesterPtr->TraceConnectWithoutContext ("TotalEnergyHarvested", MakeCallback (&TotalEnergyHarvested));

  Simulator::Stop (stopTime);
  Simulator::Run ();
  Simulator::Destroy ();
}

class SunHarvesterModelTestSuite : public TestSuite
{
public:
  SunHarvesterModelTestSuite ();
};

// ==============================================================================
SunHarvesterModelTestSuite::SunHarvesterModelTestSuite ()
  : TestSuite ("sun-harvester-model", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new SunHarvesterModelTest, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static SunHarvesterModelTestSuite sunHarvesterModelTestSuite;

