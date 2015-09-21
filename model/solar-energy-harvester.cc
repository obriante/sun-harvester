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

#include "solar-energy-harvester.h"

#include "ns3/sun.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/abort.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/energy-harvester.h"
#include "ns3/traced-value.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/device-energy-model.h"

#include <math.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SolarEnergyHarvester");

NS_OBJECT_ENSURE_REGISTERED (SolarEnergyHarvester);

TypeId
SolarEnergyHarvester::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SolarEnergyHarvester")
    .SetParent<EnergyHarvester> ()
    .AddConstructor<SolarEnergyHarvester> ()
    .AddAttribute ("PeriodicHarvestedPowerUpdateInterval",
                   "Time between two consecutive periodic updates of the harvested power. By default, the value is updated every 1 s",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&SolarEnergyHarvester::SetHarvestedPowerUpdateInterval,
                                     &SolarEnergyHarvester::GetHarvestedPowerUpdateInterval),
                   MakeTimeChecker ())
    .AddAttribute ("Latitude",
                   "The location's latitude",
                   DoubleValue (38.11),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "The longitude",
                   DoubleValue (15.661),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_longitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Altitude",
                   "The location's altitude from the sea level [m]",
                   DoubleValue (31),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_altitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SolarCellEfficiency",
                   "The Panel Solar Cell efficiency  by default 8 %",
                   DoubleValue (8),
                   MakeDoubleAccessor (&SolarEnergyHarvester:: m_solarCellEfficiency ),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DCDCEfficiency",
                   "The DC Converter efficiency  by default 90 %",
                   DoubleValue (90),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_DCDCefficiency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PanelTiltAngle",
                   "The Panel Tilt Angle  by default 0",
                   DoubleValue (0),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_panelTiltAngle),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PanelAzimuthAngle",
                   "The Panel Azimuth Angle  by default 0",
                   DoubleValue (0),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_panelAzimuthAngle),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PanelDimension",
                   "The Panel area dimension in cm^2  by default 1 ",
                   DoubleValue (1),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_panelDimension),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DiffusePercentage",
                   "The percentage of the energy diffuse by default 10 %",
                   DoubleValue (10),
                   MakeDoubleAccessor (&SolarEnergyHarvester::m_diffusePercentage),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("StartAt", "The starting date for panel simulation in format (24 hours): YYYY-MM-DD hh:mm:ss; Default: 01/01/2015 09:00:00 ",
                   StringValue ("2015-01-01 09:00:00"),
                   MakeStringAccessor  (&SolarEnergyHarvester::SetDate),
                   MakeStringChecker ())
    .AddTraceSource ("HarvestedPower",
                     "Harvested power by the EnergyHarvester.",
                     MakeTraceSourceAccessor (&SolarEnergyHarvester::m_harvestedPower),
                     "ns3::TracedValue::DoubleCallback")
    .AddTraceSource ("TotalEnergyHarvested",
                     "Total energy harvested by the solar harvester.",
                     MakeTraceSourceAccessor (&SolarEnergyHarvester::m_totalEnergyHarvestedJ),
                     "ns3::TracedValue::DoubleCallback")
  ;
  return tid;

}

SolarEnergyHarvester::SolarEnergyHarvester (void)
{
  NS_LOG_FUNCTION (this);
}

SolarEnergyHarvester::~SolarEnergyHarvester (void)
{
  NS_LOG_FUNCTION (this);
}


const tm SolarEnergyHarvester::GetDate (void) const
{
  NS_LOG_FUNCTION (this);
  return m_date;
}

void SolarEnergyHarvester::SetDate (const std::string& s)
{
  NS_LOG_FUNCTION (this << s);
  struct tm tm;

  NS_ABORT_MSG_UNLESS (strptime (s.c_str (), "%Y-%m-%d %H:%M:%S", &tm), "Date Format (24 hours): YYYY-MM-DD hh:mm:ss");

  // normalization: e.g. 29/02/2013 would become 01/03/2013
  time_t when = mktime (&tm);
  m_startDate = *localtime (&when);
  m_date = m_startDate;
}

void
SolarEnergyHarvester::SetHarvestedPowerUpdateInterval (const Time harvestedPowerUpdateInterval)
{

  NS_LOG_FUNCTION (this << harvestedPowerUpdateInterval);
  m_harvestedPowerUpdateInterval = harvestedPowerUpdateInterval;
}


const Time
SolarEnergyHarvester::GetHarvestedPowerUpdateInterval (void) const
{
  NS_LOG_FUNCTION (this);

  return m_harvestedPowerUpdateInterval;
}

double
SolarEnergyHarvester::GetDcdCefficiency (void) const
{
  NS_LOG_FUNCTION (this);
  return m_DCDCefficiency;
}

double
SolarEnergyHarvester::GetDiffusePercentage (void) const
{
  NS_LOG_FUNCTION (this);
  return m_diffusePercentage;
}

double
SolarEnergyHarvester::GetHarvestablePower (void) const
{
  NS_LOG_FUNCTION (this);
  return m_harvestablePower;
}

double
SolarEnergyHarvester::GetLatitude (void) const
{
  NS_LOG_FUNCTION (this);
  return m_latitude;
}

double
SolarEnergyHarvester::GetLongitude (void) const
{
  NS_LOG_FUNCTION (this);
  return m_longitude;
}

double
SolarEnergyHarvester::GetSolarCellEfficiency (void) const
{
  NS_LOG_FUNCTION (this);
  return m_solarCellEfficiency;
}

double
SolarEnergyHarvester::GetPanelAzimuthAngle (void) const
{
  NS_LOG_FUNCTION (this);
  return m_panelAzimuthAngle;
}

double
SolarEnergyHarvester::GetPanelDimension (void) const
{
  NS_LOG_FUNCTION (this);
  return m_panelDimension;
}

double
SolarEnergyHarvester::GetPanelTiltAngle (void) const
{
  NS_LOG_FUNCTION (this);
  return m_panelTiltAngle;
}

double
SolarEnergyHarvester::GetAltitude (void) const
{
  NS_LOG_FUNCTION (this);
  return m_altitude;
}

/*
 * Private functions start here.
 */

void
SolarEnergyHarvester::UpdateHarvestedPower (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG (Simulator::Now ().GetSeconds ()
                << "s SolarEnergyHarvester(" << GetNode ()->GetId () << "): Updating harvesting power.");

  Time duration = Simulator::Now () - m_lastHarvestingUpdateTime;

  NS_ASSERT (duration.GetNanoSeconds () >= 0);       // check if duration is valid

  double energyHarvested = 0.0;

  // do not update if simulation has finished
  if (Simulator::IsFinished ())
    {
      NS_LOG_DEBUG ("SolarEnergyHarvester: Simulation Finished.");
      return;
    }

  m_energyHarvestingUpdateEvent.Cancel ();

  CalculateHarvestedPower ();

  energyHarvested = duration.GetSeconds () * m_harvestedPower;

  // update total energy harvested
  m_totalEnergyHarvestedJ += energyHarvested;

  // notify energy source
  GetEnergySource ()->UpdateEnergySource ();

  // update last harvesting time stamp
  m_lastHarvestingUpdateTime = Simulator::Now ();

  m_date.tm_sec = m_date.tm_sec + m_harvestedPowerUpdateInterval.GetSeconds ();
  time_t when = mktime (&m_date);
  m_date = *localtime (&when);

  m_energyHarvestingUpdateEvent = Simulator::Schedule (m_harvestedPowerUpdateInterval,
                                                       &SolarEnergyHarvester::UpdateHarvestedPower,
                                                       this);
}

void
SolarEnergyHarvester::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);

  m_lastHarvestingUpdateTime = Simulator::Now ();
  UpdateHarvestedPower ();        // start periodic harvesting update
}

void
SolarEnergyHarvester::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_energyHarvestingUpdateEvent.Cancel ();
}

void
SolarEnergyHarvester::CalculateHarvestedPower (void)
{
  NS_LOG_FUNCTION (this);

  Sun::Coordinates coordinates;
  Sun::PSA (&m_date, m_latitude, m_longitude, &coordinates);

  NS_LOG_DEBUG ("Zenith Angle =" << coordinates.dZenithAngle);
  NS_LOG_DEBUG ("Elevation Angle =" << coordinates.dElevationAngle);

  if (coordinates.dElevationAngle > 0)
    {
      double incidentInsolation = 2 * Sun::GetIncidentInsolation (&m_date, m_latitude, m_longitude, m_altitude);

      double directInsolation = incidentInsolation * (cos (coordinates.dElevationAngle * rad) * sin (m_panelTiltAngle * rad) * cos ((m_panelAzimuthAngle - coordinates.dZenithAngle) * rad) + sin (coordinates.dElevationAngle * rad) * cos (m_panelTiltAngle * rad));

      double insolation = ((double)m_diffusePercentage / 100) * incidentInsolation + directInsolation;

      m_harvestedPower = insolation * (m_solarCellEfficiency / 100) * (m_DCDCefficiency / 100) * m_panelDimension;
    }
  else
    {
      m_harvestedPower = 0;
    }

  NS_LOG_DEBUG (Simulator::Now ().GetSeconds () << "s SolarEnergyHarvester:Harvested energy = " << m_harvestedPower);

}

double
SolarEnergyHarvester::DoGetPower (void) const
{
  NS_LOG_FUNCTION (this);
  return m_harvestedPower;
}

std::ostream&
operator << (std::ostream& os, Ptr<SolarEnergyHarvester> sunHarvester)
{
  char buffer[100];
  tm date = sunHarvester->GetDate ();
  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S", &date);

  os << "SolarEnergyHarvester= [";
  os << "Date: " << buffer << ", ";
  os << "Latitude: " << sunHarvester->GetLatitude () << ", ";
  os << "Longitude: " << sunHarvester->GetLongitude () << ", ";
  os << "Altitude: " << sunHarvester->GetAltitude () << ", ";
  os << "DC-DC efficiency: " << sunHarvester->GetDcdCefficiency () << "%, ";
  os << "Solar cell efficiency: " << sunHarvester->GetSolarCellEfficiency () << "%, ";
  os << "Panel azimuth: " << sunHarvester->GetPanelAzimuthAngle () << "', ";
  os << "Panel Tilt angle: " << sunHarvester->GetPanelTiltAngle () << "', ";
  os << "Panel Dimension: " << sunHarvester->GetPanelDimension () << " [m^2], ";
  os << "Diffuse insolation: " << sunHarvester->GetDiffusePercentage () << "%, ";
  os << "Harvesting time intervall: " << sunHarvester->GetHarvestedPowerUpdateInterval ().GetSeconds () << " [sec] ]";

  return os;
}

}  // namespace ns3
