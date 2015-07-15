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

#ifndef SUN_H
#define SUN_H

#include <ctime>

namespace ns3 {

/** Constants Declaration*/

#define HOURS_IN_DAY 24
#define SECONDS_IN_HOUR 3600
#define MINUTES_IN_HOUR 60
#define SECONDS_IN_MINUTE MINUTES_IN_HOUR

#define SECONDS_IN_DAY  (HOURS_IN_DAY * SECONDS_IN_HOUR)

#define pi 3.14159265358979323846
#define twopi (2 * pi)
#define rad (pi / 180)
#define dEarthMeanRadius 6371.01    // In km
#define dAstronomicalUnit 149597890 // In km


class Sun
{

public:
  typedef struct
  {
    double dZenithAngle;
    double dAzimuth;
    double dElevationAngle;
  } Coordinates;

  /**
   *  Calculate local coordinates ( azimuth and zenith angle ) in degrees
   *  \return SunCoordinates
   */
  static void PSA (const tm *date, const double &latitude, const double &longitude, Sun::Coordinates* udtSunCoordinates);

  static double GetIncidentInsolation (const tm *date, const double &latitude, const double &longitude, const double &altitude);

  static double GetAirMass (const double &latitude, const double &altitude);

private:
  /**
   *  Calculate time of the day in UT decimal hours
   *  \return UT decimal hours
   */
  static double DecimalHours (const tm *date);

}; // end class

} /* namespace ns3 */

#endif /* SUN_H */
