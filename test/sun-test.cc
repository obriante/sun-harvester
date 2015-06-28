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
#include <ns3/sun.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SunTestSuite");

class SunTestCase : public TestCase
{
public:
  SunTestCase ();
  ~SunTestCase ();

  void DoRun (void);

};

SunTestCase::SunTestCase ()
  : TestCase ("Sun test case")
{

}

SunTestCase::~SunTestCase ()
{

}

void
SunTestCase::DoRun ()
{
  LogComponentEnable ("Config", LOG_LEVEL_DEBUG);
  LogComponentEnable ("SunTestSuite", LOG_LEVEL_DEBUG);

  double latitude = 38.11;
  double longitude = 15.661;

  time_t t = time (0);        // get time now
  struct tm now = *localtime ( &t );

  char buffer[100];
  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S", &now);

  Sun::Coordinates coordinates;
  Sun::PSA (&now, latitude, longitude, &coordinates);

  std::cout << "Latitude: " << latitude << ", Air Mass=" << Sun::GetAirMass (latitude) << ", sin elevation: " << sin (coordinates.dElevationAngle * rad) / rad << std::endl;
  std::cout << buffer << " - Azimuth=" << coordinates.dAzimuth << ", Elevation=" << coordinates.dElevationAngle << ", Zenith=" << coordinates.dZenithAngle << std::endl;
  std::cout << buffer << " - Incident insolation=" << Sun::GetIncidentInsolation (&now, latitude, longitude) << std::endl;

}

class SunTestSuite : public TestSuite
{
public:
  SunTestSuite ();
};

SunTestSuite::SunTestSuite ()
  : TestSuite ("sun-test", UNIT)
{
  AddTestCase (new SunTestCase, TestCase::QUICK);
}

// create an instance of the test suite
static SunTestSuite g_sunTestSuite;
