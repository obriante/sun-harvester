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

#ifndef SUN_HARVESTER_H
#define SUN_HARVESTER_H

#include "ns3/sun.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/pointer.h"
#include "ns3/string.h"
#include "ns3/ptr.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/energy-harvester.h"
#include "ns3/traced-value.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"
#include "ns3/device-energy-model.h"


namespace ns3 {

/**
 * \ingroup SunHarvestModel
 * SunEnergyHarvester increases remaining energy stored in an associated
 * Energy Source. The SunEnergyHarvester implements a  model in which
 * the amount of power provided by the harvester varies according to years, month day and time
 * Unit of power is chosen as Watt since energy models typically calculate
 * energy as (time in seconds * power in Watt).
 *
 */

class SunEnergyHarvester : public EnergyHarvester
{
public:
  static TypeId GetTypeId (void);

  SunEnergyHarvester ();

  /**
   * \param updateInterval Energy harvesting update interval.
   *
   * SunEnergyHarvester constructor function that sets the interval
   * between each update of the value of the power harvested by this
   * energy harvester.
   */
  SunEnergyHarvester (Time updateInterval, double DCDCefficiency,
                      double solarCellEfficiency, double panelAzimuthAngle, double panelTiltAngle,
                      int year, int month, int day, int hours, int minutes);

  virtual ~SunEnergyHarvester ();

  void
  SetHarvestedPowerUpdateInterval (Time updateInterval);


  /**
   *  \param updateInterval Sun Energy harvesting update interval.
   * \returns The interval between each update of the harvested power.
   * This function returns the interval between each update of the value of the
   * power harvested by this Sun energy harvester.
   */
  Time
  GetHarvestedPowerUpdateInterval (void) const;


  /**
   * \param sunEnergyHarvesterR is an SunEnergyHarvester objet .
   *
   * This function return true if two SunEnergyHarvester object are equals.
   * Two SunEnergyHarvester objet are consider equals if panelTiltAngle, panelSolCellEff,
   * DCefficiency, diffusepercentuage and panelAzimuthAngle are equals.
   */
  const bool
  equals (const  SunEnergyHarvester*sunEnergyHarvesterR);

  /**
   *\return DCDCefficiency  as a double.
   * It is the DC-DC Efficiency of solar panel converter.
   */
  double
  getDCDCefficiency () const;

  /**
   * \param DCDCefficiency
   *  is the DC-DC efficiency
   */
  void
  setDCDCefficiency (double DCDCefficiency);

  /**
   *\return PanelAzimuthAngle
   *\ as a double. It's the panel azitmuth angle
   */
  double
  getPanelAzimuthAngle () const;

  /**
   * \param panelAzimuthAngle
   * It's the azimuth angle of the solar panel
   */
  void
  setPanelAzimuthAngle (double panelAzimuthAngle);

  double
  getPanelDimension () const;

  /**
   * \param dim the panel dimension
   *
   */
  void
  setPanelDimension (double dim);

  /**
   *\return PanelSolCellEff as a double.
   *It is the solar cell efficiency as a double
   */
  double
  getSolarCellEfficiency () const;


  /**
   * \param panelSolCellEff
   * It's the Solar Cell efficiency of the solar panel
   */
  void
  setSolarCellEfficiency  (double panelSolCellEff);

  /**
   *\return PanelTiltAngle as a double.
   * It is the tilt angle of the solar panel
   */
  double
  getPanelTiltAngle () const;

  /**
   * \param panelTiltAngle
   * the tilt angle of the solar panel
   */
  void
  setPanelTiltAngle (double panelTiltAngle);

  /**
   * \param seconds
   */
  void RefreshTime ( int seconds);

  /**
   * \param m_sun
   *  the Sun pointer
   */
  const Ptr<Sun>&
  getSun () const;

  /**
   * \return m_sun
   *  the Sun pointer
   */
  void
  setSun (const Ptr<Sun>& ptrSun);

  /**
   *\return m_day .
   * It is the day
   */
  int
  getDay () const;

  /**
   * \param  day
   */
  void
  setDay (int day);

  /**
   *\return m_diffusePercentage .
   * It is the percentage of diffuse radiation that impact on panel surface.
   */
  double
  getDiffusePercentage () const;

  /**
   * \param diffusePercentage
   * It is the percentage of diffuse radiation that impact on panel surface.
   */
  void
  setDiffusePercentage (double diffusePercentage);

  /**
   *\return m_month .
   * It is the month
   */
  int
  getMonth () const;

  /**
   * \param  month
   */
  void
  setMonth (int month);

  /**
   *\return m_year .
   * It is the  year
   */
  int
  getYear () const;

  /**
   * \param  year
   */
  void
  setYear (int year);

private:
  /// Defined in ns3::Object
  void
  DoInitialize (void);

  /// Defined in ns3::Object
  void
  DoDispose (void);

  /**
   * Calculates the sun harvested Power.
   *This function:
   * - Setting the time when calculate the energy harvest;
   * - Setting the location in term of latitude and longitude;
   * - Setting the module parameters;
   * - Calculate the sun position;
   * - Calculate the sun irradiation;
   * - CalculateEnergyHarvester ;
   */
  void
  CalculateHarvestedPower (void);

  /**
   * \returns  m_harvestablePower
   */
  virtual double
  DoGetPower (void) const;

  /**
   * This function is called every m_energyHarvestingUpdateInterval in order to
   * update the amount of power that will be provided by the harvester in the
   * next interval.
   */
  void
  UpdateHarvestedPower (void);


private:
  /**
   * This is the Panel DC-DC converter  efficiency
   */
  double m_DCDCefficiency;

  /**
   * This is the Panel Solar Cell efficiency
   */
  double m_p_SolarCellEfficiency;

  /**
   * This is the Panel Solar Azimuth angle.
   */
  double m_panelAzimuthAngle;

  /**
   * This is the Panel Solar Tilt angle.m_panelDimension
   */
  double m_panelTiltAngle;

  /**
       * This is the panel Dimension in cm.
       */
  double m_panelDimension;

  /**
   * This is the harvestable power from the sun.
   */
  double m_harvestablePower;

  /**
   *The year of simulation.
   */
  int m_year;

  /**
   *The month of simulation.
   */
  int m_month;

  /**
   *The day of simulation.
   */
  int m_day;

  /**
   *The minutes of simulation.
   */
  int m_minutes;


  /**
   *The HOurs of simulation.
   */
  int m_hours;

  /**
   *The percentage of the diffuse energy
   */
  double m_diffusePercentage;

  /**
   * The current harvested power, in Watt
   */
  TracedValue<double> m_harvestedPower;

  /**
   *  total harvested energy, in Joule
   */
  TracedValue<double> m_totalEnergyHarvestedJ;
  tm m_time;

  /**
   * This is a pointer to a Sun Source Object
   */
  Ptr<Sun> m_sun;

  /**
   * This is an energy harvesting event
   */
  EventId m_energyHarvestingUpdateEvent;

  /**
   * This is last harvesting time
   */
  Time m_lastHarvestingUpdateTime;

  /**
   * This is  the harvestable energy update interval
   */
  Time m_harvestedPowerUpdateInterval;

};  //end class

/**
 * \param os                    output stream
 * \param sunHarvester		SunEnergyHarvester object
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, SunEnergyHarvester sunHarvester);

} // namespace ns3



#endif /* SUN_HARVESTER_H */

