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

    .AddAttribute ("HarvestablePower",
                   "The harvestable power [] that the sun energy harvester is allowed to harvest. By default, the value of this variable is set to 0.0 W",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&SunEnergyHarvester::m_harvestablePower),
                   MakeDoubleChecker<double> ())

    .AddTraceSource ("HarvestedPower",
                     "Harvested power by the EnergyHarvester.",
                     MakeTraceSourceAccessor (&SunEnergyHarvester::m_harvestedPower),
                     "ns3::TracedValue::DoubleCallback")

    .AddTraceSource ("TotalEnergyHarvested",
                     "Total energy harvested by the solar harvester.",
                     MakeTraceSourceAccessor (&SunEnergyHarvester::m_totalEnergyHarvestedJ),
                     "ns3::TracedValue::DoubleCallback")

    .AddAttribute ("SolarCellEfficiency",
                   "The Panel Solar Cell efficiency  by default 8 %",
                   DoubleValue (8),
                   MakeDoubleAccessor (&SunEnergyHarvester:: m_p_SolarCellEfficiency ),
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
    .AddAttribute ("Year",
                   "The years  ",
                   IntegerValue (2004),
                   MakeIntegerAccessor  (&SunEnergyHarvester::m_year),
                   MakeIntegerChecker<int> ())

    .AddAttribute ("Month",
                   "The Month by default 1",
                   IntegerValue (1),
                   MakeIntegerAccessor  (&SunEnergyHarvester::m_month),
                   MakeIntegerChecker<int> ())

    .AddAttribute ("Day",
                   "The Day by default 1",
                   IntegerValue (1),
                   MakeIntegerAccessor  (&SunEnergyHarvester::m_day),
                   MakeIntegerChecker<int> ())
    .AddAttribute ("Minutes",
                   "The minutes  by default 0",
                   IntegerValue (0),
                   MakeIntegerAccessor  (&SunEnergyHarvester::m_minutes),
                   MakeIntegerChecker<int> ())

    .AddAttribute ("Hours",
                   "The hours by default 0",
                   IntegerValue (0),
                   MakeIntegerAccessor  (&SunEnergyHarvester::m_hours),
                   MakeIntegerChecker<int> ())

    .AddAttribute ("Sun",
                   "The Pointer to a  Sun-Source object",
                   PointerValue (),
                   MakePointerAccessor (&SunEnergyHarvester::m_sun),
                   MakePointerChecker<Sun> ())

  ;
  return tid;

}


const bool SunEnergyHarvester::equals (   const  SunEnergyHarvester* sunEnergyHarvesterR)
{
  return (
    (m_panelTiltAngle == sunEnergyHarvesterR->m_panelTiltAngle)
    &&(m_p_SolarCellEfficiency == sunEnergyHarvesterR->m_p_SolarCellEfficiency)
    &&(m_DCDCefficiency == sunEnergyHarvesterR->m_DCDCefficiency)
    &&(m_panelAzimuthAngle == sunEnergyHarvesterR->m_panelAzimuthAngle)
    &&(m_diffusePercentage == sunEnergyHarvesterR->m_diffusePercentage)
    &&(m_day == sunEnergyHarvesterR->m_day)
    &&(m_month == sunEnergyHarvesterR->m_month)
    &&(m_year == sunEnergyHarvesterR->m_year)
    &&(m_hours == sunEnergyHarvesterR->m_hours)
    &&(m_minutes == sunEnergyHarvesterR->m_minutes));
}

SunEnergyHarvester::SunEnergyHarvester ()
{
  NS_LOG_FUNCTION (this);
  m_DCDCefficiency = 0;
  m_p_SolarCellEfficiency = 0;
  m_panelAzimuthAngle = 0;
  m_panelTiltAngle = 0;
  m_year = 0;
  m_month = 1;
  m_day = 1;
  m_harvestablePower = 0;
  m_hours = 0;
  m_minutes = 0;
  RefreshTime (0);
}

SunEnergyHarvester::SunEnergyHarvester (Time updateInterval,double DCDCefficiency,double p_SolarCellEfficiency, double panelAzimuthAngle, double panelTiltAngle,
                                        int year,int month, int day,int hours,int minutes )
{
  NS_LOG_FUNCTION (this << updateInterval);
  m_DCDCefficiency = DCDCefficiency;
  m_p_SolarCellEfficiency = p_SolarCellEfficiency;
  m_panelAzimuthAngle = panelAzimuthAngle;
  m_panelTiltAngle = panelTiltAngle;
  m_year = year;
  m_month = month;
  m_day = day;
  m_hours = hours;
  m_minutes = minutes;
  m_harvestedPowerUpdateInterval = updateInterval;
  NS_ASSERT (m_month > 0);
  NS_ASSERT (m_day > 0);
  NS_ASSERT (m_p_SolarCellEfficiency > 0);
  NS_ASSERT (m_DCDCefficiency > 0);
  RefreshTime (0);
}

SunEnergyHarvester::~SunEnergyHarvester ()
{
  NS_LOG_FUNCTION (this);
}

void
SunEnergyHarvester::SetHarvestedPowerUpdateInterval (Time updateInterval)
{
  NS_LOG_FUNCTION (this << updateInterval);

  m_harvestedPowerUpdateInterval = updateInterval;
}

Time
SunEnergyHarvester::GetHarvestedPowerUpdateInterval (void) const
{
  NS_LOG_FUNCTION (this);

  return m_harvestedPowerUpdateInterval;

}

const Ptr<Sun>&
SunEnergyHarvester::getSun () const
{
  return m_sun;
}

void
SunEnergyHarvester::setSun (const Ptr<Sun>& sun)
{
  m_sun = sun;
}

int
SunEnergyHarvester::getDay () const
{
  return m_day;
}

void
SunEnergyHarvester::setDay (int day)
{
  m_day = day;
}

double
SunEnergyHarvester::getDiffusePercentage () const
{
  return m_diffusePercentage;
}

void
SunEnergyHarvester::setDiffusePercentage (double diffusePercentage)
{
  m_diffusePercentage = diffusePercentage;
}

int
SunEnergyHarvester::getMonth () const
{
  return m_month;
}

void
SunEnergyHarvester::setMonth (int month)
{
  m_month = month;
}

int
SunEnergyHarvester::getYear () const
{
  return m_year;
}

void
SunEnergyHarvester::setYear (int year)
{
  m_year = year;
}

double
SunEnergyHarvester::getPanelDimension () const
{
  return m_panelDimension;
}

void
SunEnergyHarvester::setPanelDimension (double dim)
{
  m_panelDimension = dim;
}


/*
 * Private functions start here.
 */

void
SunEnergyHarvester::UpdateHarvestedPower (void)
{
  NS_LOG_FUNCTION (this);
  RefreshTime (Simulator::Now ().GetSeconds ());
  NS_LOG_DEBUG (Simulator::Now ().GetSeconds ()
                << "s SunEnergyHarvester(" << GetNode ()->GetId () << "): Updating harvesting power.");

  Time duration = Simulator::Now () - m_lastHarvestingUpdateTime;

  NS_ASSERT (duration.GetNanoSeconds () >= 0); // check if duration is valid

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

  m_energyHarvestingUpdateEvent = Simulator::Schedule (m_harvestedPowerUpdateInterval,
                                                       &SunEnergyHarvester::UpdateHarvestedPower,
                                                       this);
}

void
SunEnergyHarvester::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);

  m_lastHarvestingUpdateTime = Simulator::Now ();
  RefreshTime (Simulator::Now ().GetSeconds ());
  UpdateHarvestedPower ();  // start periodic harvesting update
}

void
SunEnergyHarvester::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  RefreshTime (Simulator::Now ().GetSeconds ());
}



void
SunEnergyHarvester::CalculateHarvestedPower (void)
{
  NS_LOG_FUNCTION (this);
  double idiffuse;
  double idirect;
  double moduleinsolation;
  RefreshTime (Simulator::Now ().GetSeconds ());

  if ((m_sun->GetSimTime ().tm_mday != m_time.tm_mday)
      ||(m_sun->GetSimTime ().tm_mon != m_time.tm_mon)
      ||(m_sun->GetSimTime ().tm_year != m_time.tm_year))
    {
      m_sun->SetSimTime (m_time );
      m_sun->Start ();
    }
  int index = (m_time.tm_hour * 3600) + (m_time.tm_min * 60) + m_time.tm_sec;
  idiffuse =   m_sun->GetIncidentInsolation () * (m_diffusePercentage / 100) * sin ((double)(m_sun->GetElevationAngle ()[index] * rad));
  if (m_sun->GetElevationAngle ()[index] > 0)
    {
      idirect = cos ((double)((m_panelAzimuthAngle - m_sun->GetAzimuthAngle ()[index]) * rad));
      idirect = idirect * (cos ((double)(m_sun->GetElevationAngle ()[index] * rad)) * sin ((double)(m_panelTiltAngle * rad)));
      idirect = idirect + (sin ((double)(m_sun->GetElevationAngle ()[index] * rad)) * cos ((double)(m_panelTiltAngle * rad)));
      idirect = (m_sun->GetIncidentInsolation () * idirect * (1 - (m_diffusePercentage / 100)));
      if (idirect < 0)
        {
          idirect = 0;
        }
      moduleinsolation = (idirect + idiffuse);
      m_harvestedPower = (moduleinsolation * m_panelDimension * m_DCDCefficiency * m_p_SolarCellEfficiency / 10000) / m_sun->GetLightHours ();
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

void SunEnergyHarvester::RefreshTime (int seconds)
{
  NS_ASSERT (m_month > 0);
  NS_ASSERT (m_day > 0);
  m_time.tm_sec = seconds;
  m_time.tm_min = m_minutes;
  m_time.tm_hour = m_hours;
  m_time.tm_mday = m_day;
  m_time.tm_mon = m_month - 1;
  m_time.tm_year = m_year;
  m_time.tm_isdst = -1;
  mktime (&m_time);
  m_time.tm_mon = m_time.tm_mon + 1;
  m_time.tm_yday = m_time.tm_yday + 1;

}
double
SunEnergyHarvester::getDCDCefficiency () const
{
  return m_DCDCefficiency;
}

void
SunEnergyHarvester:: setDCDCefficiency (double dCefficiency)
{
  m_DCDCefficiency = dCefficiency;
}

double
SunEnergyHarvester:: getPanelAzimuthAngle () const
{
  return m_panelAzimuthAngle;
}

void
SunEnergyHarvester::setPanelAzimuthAngle (double panelAzimuthAngle)
{
  m_panelAzimuthAngle = panelAzimuthAngle;
}

double
SunEnergyHarvester:: getSolarCellEfficiency  () const
{
  return m_p_SolarCellEfficiency;
}

void
SunEnergyHarvester:: setSolarCellEfficiency (double panelSolCellEff)
{
  m_p_SolarCellEfficiency = panelSolCellEff;
}

double
SunEnergyHarvester:: getPanelTiltAngle () const
{
  return m_panelTiltAngle;
}

void
SunEnergyHarvester::setPanelTiltAngle (double panelTiltAngle)
{
  m_panelTiltAngle = panelTiltAngle;
}

std::ostream&
operator << (std::ostream& os, SunEnergyHarvester sunHarvester)
{
  os << "Date: " << sunHarvester.getSun ()->GetSimTime ().tm_mday << "-" << sunHarvester.getSun ()->GetSimTime ().tm_mon << "-" <<  sunHarvester.getSun ()->GetSimTime ().tm_year;
  os << "DC-DC efficiency: " << sunHarvester.getDCDCefficiency () << "%, ";
  os << "Solar cell efficiency" << sunHarvester.getSolarCellEfficiency () << "%, ";
  os << "Panel azimuth: " << sunHarvester.getPanelAzimuthAngle () << "[' ], ";
  os << "Panel Tilt angle: " << sunHarvester.getPanelTiltAngle () << "[' ], ";
  os << "Panel Dimension: " << sunHarvester.getPanelDimension () << "[cm^2]";
  os << "Diffuse insolation: " << sunHarvester.getDiffusePercentage () << "%, ";
  os << "Harvesting time intervall: " << sunHarvester.GetHarvestedPowerUpdateInterval () << " [sec],";

  return os;
}

}// namespace ns3
