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

#include "sun.h"

#include <math.h>
#include <ns3/assert.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/log-macros-disabled.h>
#include <ns3/object-base.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Sun");
NS_OBJECT_ENSURE_REGISTERED (Sun);


TypeId
Sun::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Sun")
    .SetParent<Object> ()
    .AddConstructor<Sun> ()
    .AddAttribute ("Latitude",
                   "The location's latitude",
                   DoubleValue (38.11),
                   MakeDoubleAccessor (&Sun::m_latitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Longitude",
                   "The longitude",
                   DoubleValue (15.661),
                   MakeDoubleAccessor (&Sun::m_longitude),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("AvgInsolation",
                   "The m_avgInsolation parameters",
                   DoubleValue (8.63),
                   MakeDoubleAccessor (&Sun::m_avgInsolation),
                   MakeDoubleChecker<double> ());

  return tid;

}


Sun::Sun ()
{
  NS_LOG_FUNCTION (this);
  m_latitude = 0;
  m_longitude = 0;
  m_avgInsolation = 0;
  m_noon = 0;
  m_sunrise = 0;
  m_sunset = 0;
  m_lightHours = 0;
  m_incidentInsolation = 0;
}

Sun::Sun ( double latitude, double longitude, double avgInsolation)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_avgInsolation >= 0);

  m_latitude = latitude;
  m_longitude = longitude;
  m_avgInsolation = avgInsolation;
  m_noon = 0;
  m_sunrise = 0;
  m_sunset = 0;
  m_lightHours = 0;
  m_incidentInsolation = 0;
}

Sun::~Sun ()
{
  NS_LOG_FUNCTION (this);
}

bool Sun::isEqual (const  Ptr<Sun> sun) const
{
  NS_LOG_FUNCTION (this);

  return (m_latitude == sun->GetLatitude ()) && (m_longitude == sun->GetLongitude ()) && (m_avgInsolation == sun->GetAvgInsolation ());

}
void Sun::Start ()
{
  NS_LOG_FUNCTION (this);

  // here if necessary you can place some controls.
  CalculateSunSource ();
  CalculateLightHours ();
}


/***********************************************************************************/

/*
 * Private functions start here.
 */
void
Sun:: CalculateSunSource ()
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("(Sun): Latitude is " << m_latitude);
  NS_LOG_DEBUG ("(Sun): Longitude is " << m_longitude);
  NS_ASSERT (m_avgInsolation >= 0);

  ElevationAngle = new double [secDay];
  AzimuthAngle = new double [secDay];
  double count = 0;
  double avgsinelevation;

  for (int i = 0; i < secDay; i++)
    {
      sim_time.tm_sec = i;
      sim_time.tm_hour = 0;
      sim_time.tm_isdst = -1;
      sim_time.tm_min = 0;
      mktime (&sim_time);
      // Main variables
      double dElapsedJulianDays;
      double dDecimalHours;
      double dEclipticLongitude;
      double dEclipticObliquity;
      double dRightAscension;
      double dDeclination;
      double dZenithAngle;
      // Auxiliary variables
      double dY;
      double dX;

      // Calculate difference in days between the current Julian Day and JD 2451545.0,
      {
        double dJulianDate;
        long int liAux1;
        long int liAux2;

        dDecimalHours = sim_time.tm_hour + (sim_time.tm_min  / 60) + (sim_time.tm_sec / 3600);
        liAux1 = (sim_time.tm_mon - 14) / 12;
        liAux2 = (1461 * ( sim_time.tm_year + 4800 + liAux1)) / 4 + (367 * (sim_time.tm_mon - 2 - 12 * liAux1)) / 12 - (3 * ((sim_time.tm_year  + 4900 + liAux1) / 100)) / 4 + sim_time.tm_mday - 32075;
        dJulianDate = (double)(liAux2) - 0.5 + dDecimalHours / 24.0;
        dElapsedJulianDays = dJulianDate - 2451545.0;
      }

      {
        double dMeanLongitude;
        double dMeanAnomaly;
        double dOmega;

        dOmega = 2.1429 - 0.0010394594 * dElapsedJulianDays;
        dMeanLongitude = 4.8950630 + 0.017202791698 * dElapsedJulianDays; // Radians
        dMeanAnomaly = 6.2400600 + 0.0172019699 * dElapsedJulianDays;
        dEclipticLongitude = dMeanLongitude + 0.03341607 * sin ( dMeanAnomaly ) + 0.00034894 * sin ( 2 * dMeanAnomaly ) - 0.0001134 - 0.0000203 * sin (dOmega);
        dEclipticObliquity = 0.4090928 - 6.2140e-9 * dElapsedJulianDays + 0.0000396 * cos (dOmega);
      }

      {
        double dSin_EclipticLongitude;
        dSin_EclipticLongitude = sin ( dEclipticLongitude );
        dY = cos ( dEclipticObliquity ) * dSin_EclipticLongitude;
        dX = cos ( dEclipticLongitude );
        dRightAscension = atan2 ( dY,dX );

        if ( dRightAscension < 0.0 )
          {
            dRightAscension = dRightAscension + twopi;
          }

        dDeclination = asin ( sin ( dEclipticObliquity ) * dSin_EclipticLongitude );
      }

      // Calculate local coordinates ( AzimuthAngle and zenith angle ) in degrees
      {
        double dGreenwichMeanSiderealTime;
        double dLocalMeanSiderealTime;
        double m_latitudeInRadians;
        double dHourAngle;
        double dCos_Latitude;
        double dSin_Latitude;
        double dCos_HourAngle;
        double dParallax;
        dGreenwichMeanSiderealTime = 6.6974243242 +
          0.0657098283 * dElapsedJulianDays
          + dDecimalHours;
        dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15
                                  + m_longitude) * rad;
        dHourAngle = dLocalMeanSiderealTime - dRightAscension;
        m_latitudeInRadians = m_latitude * rad;
        dCos_Latitude = cos ( m_latitudeInRadians );
        dSin_Latitude = sin ( m_latitudeInRadians );
        dCos_HourAngle = cos ( dHourAngle );
        dZenithAngle = (acos ( dCos_Latitude * dCos_HourAngle
                               * cos (dDeclination) + sin ( dDeclination ) * dSin_Latitude));
        dY = -sin ( dHourAngle );
        dX = tan ( dDeclination ) * dCos_Latitude - dSin_Latitude * dCos_HourAngle;
        AzimuthAngle[i] = atan2 ( dY, dX );
        if (   AzimuthAngle[i] < 0.0 )
          {
            AzimuthAngle[i] =   AzimuthAngle[i] + twopi;
          }
        AzimuthAngle[i] =   AzimuthAngle[i] / rad;

        // Parallax Correction
        dParallax = (dEarthMeanRadius / dAstronomicalUnit) * sin ( dZenithAngle);
        dZenithAngle = (dZenithAngle + dParallax) / rad;

        if ((90 - dZenithAngle) > 0)
          {
            ElevationAngle[i] = 90 - dZenithAngle; //DEGREE
            count++;
            avgsinelevation = avgsinelevation + sin (ElevationAngle[i] * rad);
          }
        else
          {
            ElevationAngle[i] = 0; //the elevation angle can not be negative
          }
      }
      /**
       * an alternative formulation to calculate the elevation angle is:
       *
       * ElevationAngle[i] =  asin((sin( dDeclination)*dSin_Latitude)+(cos(dDeclination)*dCos_HourAngle*dCos_Latitude))/rad;
       **/

    }//end for

  //Calculate the average elevation Angle
  avgsinelevation = (avgsinelevation / count);
  m_incidentInsolation = (m_avgInsolation * 360) / avgsinelevation; // TO DO: NON MI TORNA!!

  NS_LOG_DEBUG  ("(Sun): Writing files Sun_info successful!");
  NS_LOG_UNCOND ("(Sun): CalculateSun OK!");


}  //end-function Calculate SunSource


void
Sun::CalculateLightHours ()
{
  double sunrise = 0;
  m_sunset = secDay;
  int i = 0;
  int j = secDay;

  while (ElevationAngle[i] <= 0)
    {
      i++;
      m_sunrise = i;
    }
  NS_LOG_DEBUG  ("(Sun): m_sunrise " << m_sunrise / 3600); //hours

  while (ElevationAngle[j] == 0)
    {
      j--;
      m_sunset = j;
    }
  NS_LOG_DEBUG  ("(Sun): m_sunset  " << m_sunset / 3600);  //hours

  m_noon =  m_sunrise;
  for (int i = sunrise; i < m_sunset; i++)
    {
      if (ElevationAngle[i] > ElevationAngle[i - 1])
        {
          m_noon++;
        }
    }
  m_lightHours = m_sunset - m_sunrise;

  NS_LOG_DEBUG   ("(Sun): Noon  " << m_noon / 3600);//hours
}


std::ostream&
operator << (std::ostream& os, Ptr<Sun> sun)
{
  os << "Latitude: " << sun->GetLatitude () << " [' N], ";
  os << "Longitude: " << sun->GetLongitude () << " [' N], ";
  os << "Average Insolation: " << sun->GetAvgInsolation () << " [J]";

  return os;
}

double* Sun::GetAzimuthAngle () const
{
  return AzimuthAngle;
}

void Sun::SetAzimuthAngle (double* azimuthAngle)
{
  AzimuthAngle = azimuthAngle;
}

double* Sun::GetElevationAngle () const
{
  return ElevationAngle;
}

void Sun::SetElevationAngle (double* elevationAngle)
{
  ElevationAngle = elevationAngle;
}

double Sun::GetAvgInsolation () const
{
  return m_avgInsolation;
}

void Sun::SetAvgInsolation (double avgInsolation)
{
  m_avgInsolation = avgInsolation;
}

double Sun::GetIncidentInsolation () const
{
  return m_incidentInsolation;
}

void Sun::SetIncidentInsolation (double incidentInsolation)
{
  m_incidentInsolation = incidentInsolation;
}

double Sun::GetLatitude () const
{
  return m_latitude;
}

void Sun::SetLatitude (double latitude)
{
  m_latitude = latitude;
}

double Sun::GetLightHours () const
{
  return m_lightHours;
}

void Sun::SetLightHours (double lightHours)
{
  m_lightHours = lightHours;
}

double Sun::GetLongitude () const
{
  return m_longitude;
}

void Sun::SetLongitude (double longitude)
{
  m_longitude = longitude;
}

double Sun::GetNoon () const
{
  return m_noon;
}

void Sun::SetNoon (double noon)
{
  m_noon = noon;
}

double Sun::GetSunrise () const
{
  return m_sunrise;
}

void Sun::SetSunrise (double sunrise)
{
  m_sunrise = sunrise;
}

double Sun::GetSunset () const
{
  return m_sunset;
}

void Sun::SetSunset (double sunset)
{
  m_sunset = sunset;
}

const tm& Sun::GetSimTime () const
{
  return sim_time;
}

void Sun::SetSimTime (const tm& simTime)
{
  sim_time = simTime;
}

}  /* namespace ns3 */
