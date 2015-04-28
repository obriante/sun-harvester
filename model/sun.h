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

#ifndef SUN_ENERGY_H
#define SUN_ENERGY_H

#include <time.h>
#include "ns3/core-module.h"
#include "ns3/energy-module.h"
#include "ns3/type-id.h"
#include "ns3/object.h"
#include "ns3/node.h"

// Declaration of some constants
#define pi    3.14159265358979323846
#define twopi (2 * pi)
#define rad   (pi / 180)
#define secDay  86400  // second for day
#define dEarthMeanRadius     6371.01    // In km
#define dAstronomicalUnit    149597890  // In km

namespace ns3 {

class Sun : public Object
{
private:
  double dLongitude;
  double dLatitude;
  double avgInsolation;
  tm sim_time;
  double *ElevationAngle;
  double *AzimuthAngle;
  double  Noon;
  double  Sunrise;
  double  Sunset;
  double  LightHours;
  double IncidentInsolation;

  /**
   * PRIVATE
   *
   * This function is called for calculate some parameters of the sun. This function compute:
   *- The Sun AzimuthAngle as a vector of 86400 seconds
   *- The Sun ElevationAngle as a vector of 86400 seconds
   *- The Horizzontal Insolation as a double
   */
  void  CalculateSunSource ();

  /**
   * PRIVATE
   * This function compute:;
   *- The hours of Sunrise;
   *- The hours of dSunset;
   *- The Light Hours.
   */
  void calculateLightHours ();

public:
  static TypeId GetTypeId (void);

  /**
   * Default constructor
   */
  Sun ();

  /**
   * Parameterized constructor
   */
  Sun (double latitude, double longitude, double avgInsolation);

  /**
   *  Destroyer
   */
  virtual ~Sun ();

  /**
   * \param sunEnergyHarvesterR is an SunEnergyHarvester objet .
   *
   * This function return true if two SunEnergyHarvester object are equals.
   * Two SunEnergyHarvester objet are consider equals if Year,day,
   * avgInsolation, latitude and longitude are equals.
   */
  const bool
  equals (const  Sun*sun);


  /**
   *  This function is used to start calculateSunSource e CalculateLightHours;
   */
  void Start ();

  /**
   *  return dLatitude: the location's latitude.
   */
  double
  getLatitude () const
  {
    return dLatitude;
  }

  /**
   * \param Latitude
   *  Set  the location's latitude.
   */
  void
  setLatitude (double Latitude)
  {
    dLatitude = Latitude;
  }

  /**
   *  return Longitude
   *   the location's longitude.
   */
  double
  getLongitude () const
  {
    return dLongitude;
  }

  /**
   * \param Longitude
   *  Set  the location's longitude.
   */
  void
  setLongitude (double  Longitude)
  {
    dLongitude = Longitude;
  }


  /**
   *  return Azimuth : the vector of the sun's azimuth angle
   */
  double*
  getAzimuthAngle () const
  {
    return AzimuthAngle;
  }

  /**
   * \param azimuth
   *  Set the vector of the sun's azimuth angle
   */
  void
  setAzimuthAngle (double* azimuthangle)
  {
    AzimuthAngle = azimuthangle;
  }

  /**
   *  return ElevationAngle
   *   the vector of the sun's elevation angle.
   */
  double*
  getElevationAngle () const
  {
    return ElevationAngle;
  }

  /**
   * \param *elevationAngle
   *  Set  the vector of elevationAngle longitude.
   */
  void
  setElevationAngle (double* elevationAngle)
  {
    ElevationAngle = elevationAngle;
  }

  /**
   *  return IncidentInsolation
   *   the insolation in a horizzontal surface
   */
  double
  getIncidentInsolation () const
  {
    return IncidentInsolation;
  }

  /**
   * \param horizzonatalInsolation
   *  Set the horizzonatalInsolation.
   */
  void
  setIncidentInsolation (double IncidentInsolation)
  {
    this->IncidentInsolation = IncidentInsolation;
  }

  /**
   *  return LightHours
   *   the daytimes hours
   */
  double
  getLightHours () const
  {
    return LightHours;
  }

  /**
   * \param lightHours
   *  Set the lightHours.
   */
  void
  setLightHours (double lightHours)
  {
    LightHours = lightHours;
  }

  /**
   *  return avgInsolation : the current avgInsolation
   */
  double
  getavgInsolation () const
  {
    return avgInsolation;
  }


  void
  setavgInsolation (double avgInsolation)
  {
    this->avgInsolation = avgInsolation;
  }

  /**
   *  return Noon
   *   the noon hours.
   */
  double
  getNoon () const
  {
    return Noon;
  }

  /**
   *  \param noon
   *   set the noon hours.
   */
  void
  setNoon (double noon)
  {
    Noon = noon;
  }

  /**
   *  return SimTime
   */
  const tm&
  getSimTime () const
  {
    return sim_time;
  }


  void
  setSimTime (const tm& simTime)
  {
    sim_time = simTime;
  }

  /**
   *  return Sunrise
   *  the Sunrise hours.
   */
  double
  getSunrise () const
  {
    return Sunrise;
  }


  void
  setSunrise (double sunrise)
  {
    Sunrise = sunrise;
  }


  /**
   *  return Sunset
   *   the Sunset hour.
   */
  double
  getSunset () const
  {
    return Sunset;
  }

  /**
    *  return Sunset
    *   the Sunset hour.
    */
  void
  setSunset (double sunset)
  {
    Sunset = sunset;
  }

};
// end class

/**
 * \param os          output stream
 * \param sun         Sun object
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, Sun sun);

}//end namespace

#endif /* SUN_Energy_H */

