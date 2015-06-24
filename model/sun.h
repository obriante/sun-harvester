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

#include <ns3/object.h>
#include <ns3/ptr.h>
#include <time.h>
#include <iostream>

namespace ns3 {

// Constants Declaration
#define pi    3.14159265358979323846
#define twopi (2 * pi)
#define rad   (pi / 180)
#define secDay  86400  // second for day
#define dEarthMeanRadius     6371.01    // In km
#define dAstronomicalUnit    149597890  // In km

class Sun : public Object
{

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
  bool isEqual (const  Ptr<Sun> sun) const;

  /**
   *  This function is used to start calculateSunSource e CalculateLightHours;
   */
  void Start ();

  /** Getters and Setters */
  double* GetAzimuthAngle () const;
  void SetAzimuthAngle (double* azimuthAngle);
  double* GetElevationAngle () const;
  void SetElevationAngle (double* elevationAngle);
  double GetAvgInsolation () const;
  void SetAvgInsolation (double avgInsolation);
  double GetIncidentInsolation () const;
  void SetIncidentInsolation (double incidentInsolation);
  double GetLatitude () const;
  void SetLatitude (double latitude);
  double GetLightHours () const;
  void SetLightHours (double lightHours);
  double GetLongitude () const;
  void SetLongitude (double longitude);
  double GetNoon () const;
  void SetNoon (double noon);
  double GetSunrise () const;
  void SetSunrise (double sunrise);
  double GetSunset () const;
  void SetSunset (double sunset);
  const tm& GetSimTime () const;
  void SetSimTime (const tm& simTime);

private:
  /**
   * This function is called for calculate:
   *
   * - The Sun AzimuthAngle as a vector of 86400 seconds
   * - The Sun ElevationAngle as a vector of 86400 seconds
   * - The Horizontal Insolation as a double
   */
  void  CalculateSunSource ();

  /**
   * This function compute:
   *
   * - The Sunrise time;
   * - The Sunset time;
   * - The Light Hours.
   */
  void CalculateLightHours ();

  /** Variables */
  double m_longitude;
  double m_latitude;
  double m_avgInsolation;
  tm sim_time;
  double *ElevationAngle;
  double *AzimuthAngle;
  double  m_noon;
  double  m_sunrise;
  double  m_sunset;
  double  m_lightHours;
  double  m_incidentInsolation;

}; // end class

/**
 * \param os          output stream
 * \param sun         Sun object
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, Ptr<Sun> sun);

}//end namespace

#endif /* SUN_Energy_H */

