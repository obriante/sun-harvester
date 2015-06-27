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

#include "sun-harvester.h"

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

NS_LOG_COMPONENT_DEFINE ("SunEnergyHarvester");

NS_OBJECT_ENSURE_REGISTERED (SunEnergyHarvester);

TypeId
SunEnergyHarvester::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SunEnergyHarvester")
    .SetParent<EnergyHarvester> ()
    .AddConstructor<SunEnergyHarvester> ()
    .AddAttribute ("PeriodicHarvestedPowerUpdateInterval",
                   "Time between two consecutive periodic updates of the harvested power. By default, the value is updated every 1 s",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&SunEnergyHarvester::SetHarvestedPowerUpdateInterval,
                                     &SunEnergyHarvester::GetHarvestedPowerUpdateInterval),
                   MakeTimeChecker ())
    .AddAttribute ("SolarCellEfficiency",
                   "The Panel Solar Cell efficiency  by default 8 %",
                   DoubleValue (8),
                   MakeDoubleAccessor (&SunEnergyHarvester:: m_solarCellEfficiency ),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DCDCEfficiency",
                   "The DC Converter efficiency  by default 90 %",
                   DoubleValue (90),
                   MakeDoubleAccessor (&SunEnergyHarvester::m_DCDCefficiency),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PanelTiltAngle",
                   "The Panel Tilt Angle  by default 0",
                   DoubleValue (0),
                   MakeDoubleAccessor (&SunEnergyHarvester::m_panelTiltAngle),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PanelAzimuthAngle",
                   "The Panel Azimuth Angle  by default 0",
                   DoubleValue (0),
                   MakeDoubleAccessor (&SunEnergyHarvester::m_panelAzimuthAngle),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("PanelDimension",
                   "The Panel area dimension in cm^2  by default 1 ",
                   DoubleValue (1),
                   MakeDoubleAccessor (&SunEnergyHarvester::m_panelDimension),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("DiffusePercentage",
                   "The percentage of the energy diffuse by default 10 %",
                   DoubleValue (10),
                   MakeDoubleAccessor (&SunEnergyHarvester::m_diffusePercentage),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("StartAt", "The starting date for panel simulation in format (24 hours): YYYY-MM-DD hh:mm:ss; Default: 2005-06-21 09:00:00.",
                   StringValue ("2005-06-21 09:00:00"),
                   MakeStringAccessor  (&SunEnergyHarvester::SetDate),
                   MakeStringChecker ())
    .AddTraceSource ("HarvestedPower",
                     "Harvested power by the EnergyHarvester.",
                     MakeTraceSourceAccessor (&SunEnergyHarvester::m_harvestedPower),
                     "ns3::TracedValue::DoubleCallback")
    .AddTraceSource ("TotalEnergyHarvested",
                     "Total energy harvested by the solar harvester.",
                     MakeTraceSourceAccessor (&SunEnergyHarvester::m_totalEnergyHarvestedJ),
                     "ns3::TracedValue::DoubleCallback")

    .AddAttribute ("Sun",
                   "The Pointer to a  Sun-Source object",
                   PointerValue (),
                   MakePointerAccessor (&SunEnergyHarvester::m_sun),
                   MakePointerChecker<Sun> ())

  ;
  return tid;

}

SunEnergyHarvester::SunEnergyHarvester ()
{
  NS_LOG_FUNCTION (this);
}

SunEnergyHarvester::~SunEnergyHarvester ()
{
  NS_LOG_FUNCTION (this);
}

const tm SunEnergyHarvester::GetDate () const
{
  NS_LOG_FUNCTION (this);
  return m_date;
}

void SunEnergyHarvester::SetDate (const std::string& s)
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
SunEnergyHarvester::SetHarvestedPowerUpdateInterval (const Time harvestedPowerUpdateInterval)
{

  NS_LOG_FUNCTION (this << harvestedPowerUpdateInterval);
  m_harvestedPowerUpdateInterval = harvestedPowerUpdateInterval;
}


const Time
SunEnergyHarvester::GetHarvestedPowerUpdateInterval (void) const
{
  NS_LOG_FUNCTION (this);

  return m_harvestedPowerUpdateInterval;

}

double
SunEnergyHarvester::GetDcdCefficiency () const
{
  NS_LOG_FUNCTION (this);
  return m_DCDCefficiency;
}

double
SunEnergyHarvester::GetDiffusePercentage () const
{
  NS_LOG_FUNCTION (this);
  return m_diffusePercentage;
}

double
SunEnergyHarvester::GetHarvestablePower () const
{
  NS_LOG_FUNCTION (this);
  return m_harvestablePower;
}

double
SunEnergyHarvester::GetSolarCellEfficiency () const
{
  NS_LOG_FUNCTION (this);
  return m_solarCellEfficiency;
}

double
SunEnergyHarvester::GetPanelAzimuthAngle () const
{
  NS_LOG_FUNCTION (this);
  return m_panelAzimuthAngle;
}

double
SunEnergyHarvester::GetPanelDimension () const
{
  NS_LOG_FUNCTION (this);
  return m_panelDimension;
}

double
SunEnergyHarvester::GetPanelTiltAngle () const
{
  NS_LOG_FUNCTION (this);
  return m_panelTiltAngle;
}

Ptr<Sun> SunEnergyHarvester::GetSun () const
{
  return m_sun;
}

void
SunEnergyHarvester::UpdateHarvestedPower (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG (Simulator::Now ().GetSeconds ()
                << "s SunEnergyHarvester(" << GetNode ()->GetId () << "): Updating harvesting power.");

  Time duration = Simulator::Now () - m_lastHarvestingUpdateTime;

  NS_ASSERT (duration.GetNanoSeconds () >= 0);       // check if duration is valid

  double energyHarvested = 0.0;

  // do not update if simulation has finished
  if (Simulator::IsFinished ())
    {
      NS_LOG_DEBUG ("SunEnergyHarvester: Simulation Finished.");
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
                                                       &SunEnergyHarvester::UpdateHarvestedPower,
                                                       this);
}

void
SunEnergyHarvester::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);

  m_lastHarvestingUpdateTime = Simulator::Now ();
  UpdateHarvestedPower ();        // start periodic harvesting update
}

void
SunEnergyHarvester::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
}

void
SunEnergyHarvester::CalculateHarvestedPower (void)
{
  NS_LOG_FUNCTION (this);

  double incidentInsolation = m_sun->GetIncidentInsolation (&m_date);

  Sun::Coordinates coordinates;
  m_sun->CalculateSunSource (&m_date, &coordinates);

  NS_LOG_DEBUG ("Zenith Angle =" << coordinates.dZenithAngle);
  NS_LOG_DEBUG ("Elevation Angle =" << coordinates.dElevationAngle);

  if (coordinates.dElevationAngle > 0)
    {
      double directInsolation = cos (coordinates.dElevationAngle * rad) * sin (m_panelTiltAngle * rad) * cos ((m_panelAzimuthAngle - coordinates.dZenithAngle) * rad) + sin (coordinates.dElevationAngle * rad) * cos (m_panelTiltAngle * rad);

      directInsolation = incidentInsolation * directInsolation * 1000 / 3600;   // KWh to W:  1 KWh = 1000 Wh and 1 Wh= 3600 W.

      double insolation = ((double)m_diffusePercentage / 100) * directInsolation + directInsolation;

      m_harvestedPower = insolation * (m_solarCellEfficiency / 100) * (m_DCDCefficiency / 100) * m_panelDimension;
    }
  else
    {
      m_harvestedPower = 0;
    }

  NS_LOG_DEBUG (Simulator::Now ().GetSeconds ()
                << "s SunEnergyHarvester:Harvested energy = " << m_harvestedPower);

}

double
SunEnergyHarvester::DoGetPower (void) const
{
  NS_LOG_FUNCTION (this);
  return m_harvestedPower;
}

std::ostream&
operator << (std::ostream& os, Ptr<SunEnergyHarvester> sunHarvester)
{
  char buffer[100];
  tm date = sunHarvester->GetDate ();
  strftime (buffer, 80,"%Y-%m-%d %H:%M:%S", &date);

  os << "SunEnergyHarvester= [";
  os << "Date: " << buffer << ", ";
  os << "Sun: " << sunHarvester->GetSun () << ", ";
  os << "DC-DC efficiency: " << sunHarvester->GetDcdCefficiency () << "%, ";
  os << "Solar cell efficiency: " << sunHarvester->GetSolarCellEfficiency () << "%, ";
  os << "Panel azimuth: " << sunHarvester->GetPanelAzimuthAngle () << "', ";
  os << "Panel Tilt angle: " << sunHarvester->GetPanelTiltAngle () << "', ";
  os << "Panel Dimension: " << sunHarvester->GetPanelDimension () << " [m^2], ";
  os << "Diffuse insolation: " << sunHarvester->GetDiffusePercentage () << "%, ";
  os << "Harvesting time intervall: " << sunHarvester->GetHarvestedPowerUpdateInterval ().GetSeconds () << " [sec] ]";

  return os;
}

}// namespace ns3
