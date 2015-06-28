/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Wireless Communications and Networking Group (WCNG),
 * University of Rochester, Rochester, NY, USA.
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
 * Author: Cristiano Tapparello <cristiano.tapparello@rochester.edu>
 */

#include <ns3/log.h>
#include <ns3/test.h>
#include <ns3/node.h>
#include <ns3/simulator.h>
#include <ns3/double.h>
#include <ns3/config.h>
#include <ns3/string.h>
#include <ns3/solar-energy-harvester.h>
#include <ns3/basic-energy-source.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SolarEnergyHarvesterTestSuite");

class SolarEnergyHarvesterTestCase : public TestCase
{
public:
  SolarEnergyHarvesterTestCase ();
  ~SolarEnergyHarvesterTestCase ();

  void DoRun (void);

  double m_timeS;     // in seconds
  double m_tolerance; // tolerance for energy estimation

  ObjectFactory m_energySource;
  ObjectFactory m_energyHarvester;

};

SolarEnergyHarvesterTestCase::SolarEnergyHarvesterTestCase ()
  : TestCase ("Sun Energy Harvester test case")
{
  m_timeS = 15;       // harvest energy for 15 seconds
  m_tolerance = 1.0e-13;        //
}

SolarEnergyHarvesterTestCase::~SolarEnergyHarvesterTestCase ()
{
}

void
SolarEnergyHarvesterTestCase::DoRun ()
{
  LogComponentEnable ("Config", LOG_LEVEL_DEBUG);
  LogComponentEnable ("BasicEnergySource", LOG_LEVEL_DEBUG);
  LogComponentEnable ("SolarEnergyHarvester", LOG_LEVEL_DEBUG);
  LogComponentEnable ("SolarEnergyHarvesterTestSuite", LOG_LEVEL_DEBUG);

  // set types
  m_energySource.SetTypeId ("ns3::BasicEnergySource");
  m_energyHarvester.SetTypeId ("ns3::SolarEnergyHarvester");
  // create node
  Ptr<Node> node = CreateObject<Node> ();

  // create Energy Source
  Ptr<BasicEnergySource> source = m_energySource.Create<BasicEnergySource> ();
  // aggregate Energy Source to the node
  node->AggregateObject (source);

  //create energy harvester
  Ptr<SolarEnergyHarvester> harvester = m_energyHarvester.Create<SolarEnergyHarvester> ();
  // Set the Energy Harvesting update interval to a value grater than the
  // simulation duration, so that the power provided by the harvester is constant
  harvester->SetAttribute ("PeriodicHarvestedPowerUpdateInterval", TimeValue (Seconds (m_timeS + 1.0)));
  // Connect the Basic Energy Harvester to the Energy Source
  source->ConnectEnergyHarvester (harvester);
  harvester->SetNode (node);
  harvester->SetEnergySource (source);

  NS_LOG_DEBUG (harvester);

  Time now = Simulator::Now ();

  /*
         * The energy harvester will recharge the energy source for m_timeS seconds.
         */

  // Calculate remaining energy at simulation stop time
  Simulator::Schedule (Seconds (m_timeS),
                       &BasicEnergySource::UpdateEnergySource, source);

  double timeDelta = 0.000000001; // 1 nanosecond
  // run simulation; stop just after last scheduled event
  Simulator::Stop (Seconds (m_timeS + timeDelta));
  Simulator::Run ();

  // calculate energy harvested
  double estRemainingEnergy = source->GetInitialEnergy ();
  // energy = power * time
  estRemainingEnergy += harvester->GetPower () * m_timeS;

  // obtain remaining energy from source
  double remainingEnergy = source->GetRemainingEnergy ();
  NS_LOG_DEBUG ("Remaining energy is " << remainingEnergy);
  NS_LOG_DEBUG ("Estimated remaining energy is " << estRemainingEnergy);
  NS_LOG_DEBUG ("Difference is " << estRemainingEnergy - remainingEnergy);

  Simulator::Destroy ();

  // check remaining energy
  NS_TEST_ASSERT_MSG_EQ_TOL (remainingEnergy, estRemainingEnergy, m_tolerance,
                             "Incorrect Remaining energy!");

}

class SolarEnergyHarvesterTestSuite : public TestSuite
{
public:
  SolarEnergyHarvesterTestSuite ();
};

SolarEnergyHarvesterTestSuite::SolarEnergyHarvesterTestSuite ()
  : TestSuite ("solar-energy-harvester-test", UNIT)
{
  AddTestCase (new SolarEnergyHarvesterTestCase, TestCase::QUICK);
}

// create an instance of the test suite
static SolarEnergyHarvesterTestSuite g_solarEnergyHarvesterTestSuite;
