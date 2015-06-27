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

/** Constants Declaration*/

#define HOURS_IN_DAY 24
#define SECONDS_IN_HOUR 3600
#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTE MINUTES_IN_HOUR

#define SECONDS_IN_DAY  (HOURS_IN_DAY * SECONDS_IN_HOUR)

#define pi    3.14159265358979323846
#define twopi (2 * pi)
#define rad   (pi / 180)
#define secDay  86400  // second for day
#define dEarthMeanRadius     6371.01    // In km
#define dAstronomicalUnit    149597890  // In km

class Sun : public Object
{

public:
  typedef struct
  {
    double dZenithAngle;
    double dAzimuth;
    double dElevationAngle;
  } Coordinates;

  static TypeId GetTypeId (void);

  Sun ();
  virtual ~Sun ();

  double GetIncidentInsolation (const tm *date);

  /**
   * This function is called for calculate:
   *
   * - The Sun AzimuthAngle as a vector of 86400 seconds
   * - The Sun ElevationAngle as a vector of 86400 seconds
   * - The Horizontal Insolation as a double
   */
  void  CalculateSunSource (const tm *date, Sun::Coordinates* udtSunCoordinates);


  /** Getters */
  double GetAvgInsolation () const;
  double GetLatitude () const;
  double GetLongitude () const;

private:
  /**
   *  Calculate time of the day in UT decimal hours
   *  \return UT decimal hours
   */
  double DecimalHours (const tm *date);

  /** Variables */
  double m_longitude;
  double m_latitude;
  double m_avgInsolation;

}; // end class

/**
 * \param os          output stream
 * \param sun         Sun object
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, Ptr<Sun> sun);

}//end namespace

#endif /* SUN_Energy_H */

