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
  virtual ~SunEnergyHarvester ();

  void SetDate (const std::string& s);
  void SetHarvestedPowerUpdateInterval (const Time harvestedPowerUpdateInterval);

  const tm GetDate (void) const;

  double GetDcdCefficiency () const;
  double GetDiffusePercentage () const;
  double GetHarvestablePower () const;
  const Time GetHarvestedPowerUpdateInterval () const;
  Ptr<Sun> GetSun() const;
  double GetSolarCellEfficiency () const;
  double GetPanelAzimuthAngle () const;
  double GetPanelDimension () const;
  double GetPanelTiltAngle () const;

private:
  /// Defined in ns3::Object
  virtual void DoInitialize (void);

  /// Defined in ns3::Object
  virtual void DoDispose (void);

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
  void CalculateHarvestedPower (void);

  /**
   * \returns  m_harvestablePower
   */
  virtual double DoGetPower (void) const;

  /**
   * This function is called every m_energyHarvestingUpdateInterval in order to
   * update the amount of power that will be provided by the harvester in the
   * next interval.
   */
  void UpdateHarvestedPower (void);


private:

  /** Input Parameter */
  double m_DCDCefficiency; // <- This is the Panel DC-DC converter  efficiency.
  double m_solarCellEfficiency; // <- The Panel Solar Cell efficiency
  double m_panelAzimuthAngle; // <- The Panel Solar Azimuth angle.
  double m_panelTiltAngle; // <- The Panel Solar Tilt angle.
  double m_panelDimension; // <- The panel Dimension in m.
  double m_harvestablePower; // <- This is the harvestable power from the sun.
  double m_diffusePercentage; // <- The diffused energy percentage

  tm m_startDate;
  tm m_date;

  Ptr<Sun> m_sun; // <- A pointer to a Sun Source Object

  /** Traced Parameter */
  TracedValue<double> m_harvestedPower; // <-The current harvested power, in Watt
  TracedValue<double> m_totalEnergyHarvestedJ; //<- the total harvested energy, in Joule

  /** Internal Parameter */
  EventId m_energyHarvestingUpdateEvent; // <- Energy harvesting event
  Time m_lastHarvestingUpdateTime; // <- This is last harvesting time
  Time m_harvestedPowerUpdateInterval; // <- This is  the harvestable energy update interval

};  //end class

/**
 * \param os                    output stream
 * \param sunHarvester		SunEnergyHarvester object
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, Ptr<SunEnergyHarvester> sunHarvester);

} // namespace ns3



#endif /* SUN_HARVESTER_H */

