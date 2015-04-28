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

#include "ns3/log.h"
#include "ns3/type-id.h"
#include <string.h>
#include <math.h>

#include "sun.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Sun");
NS_OBJECT_ENSURE_REGISTERED (Sun );


TypeId
Sun::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Sun")
    .SetParent<Object> ()
    .AddConstructor<Sun> ()

    .AddAttribute ("Latitude",
                   "The location's latitude",
                   DoubleValue (38.11),
                   MakeDoubleAccessor (&Sun::dLatitude),
                   MakeDoubleChecker<double> ())

    .AddAttribute ("Longitude",
                   "The longitude",
                   DoubleValue (15.661),
                   MakeDoubleAccessor (&Sun::dLongitude),
                   MakeDoubleChecker<double> ())

    .AddAttribute ("avgInsolation",
                   "The avgInsolation parameters",
                   DoubleValue (8.63),
                   MakeDoubleAccessor (&Sun::avgInsolation),
                   MakeDoubleChecker<double> ());

  return tid;

}


Sun::Sun ()
{
  NS_LOG_FUNCTION (this);
  this->dLatitude = 0;
  this->dLongitude = 0;
  this->avgInsolation = 0;
  this->Noon = 0;
  this->Sunrise = 0;
  this->Sunset = 0;
  this->LightHours = 0;
  this->IncidentInsolation = 0;
  this->Noon = 0;
  this->Sunrise = 0;
  this->Sunset = 0;
  this->LightHours = 0;
  this->IncidentInsolation = 0;
}

Sun::Sun ( double latitude, double longitude, double avgInsolation)
{
  this->dLatitude = latitude;
  this->dLongitude = longitude;
  this->avgInsolation = avgInsolation;
  NS_ASSERT (this->avgInsolation > 0);
}

Sun::~Sun ()
{
}

const bool Sun::equals (   const  Sun* Sun)
{

  return (this->dLatitude == Sun->dLatitude)
         &&(this->dLongitude == Sun->dLongitude)
         &&(this->avgInsolation == Sun->avgInsolation);

}
void Sun::Start ()
{
  // here if necessary you can place some controls.
  CalculateSunSource ();
  calculateLightHours ();
}


/***********************************************************************************/

/*
 * Private functions start here.
 */
void
Sun:: CalculateSunSource ()
{
  NS_LOG_DEBUG ("(Sun): Latitude is " << this->dLatitude);
  NS_LOG_DEBUG ("(Sun): Longitude is " << this->dLongitude);
  NS_ASSERT (this->avgInsolation > 0);

  this->ElevationAngle = new double [secDay];
  this->AzimuthAngle = new double [secDay];
  double count = 0;
  double avgsinelevation;
  for (int i = 0; i < secDay; i++)
    {
      this->sim_time.tm_sec = i;
      this->sim_time.tm_hour = 0;
      this->sim_time.tm_isdst = -1;
      this->sim_time.tm_min = 0;
      mktime (&this->sim_time);
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
        dDecimalHours = this->sim_time.tm_hour + (this->sim_time.tm_min
                                                  + +this->sim_time.tm_sec / 60.0 ) / 60.0;
        liAux1 = (this->sim_time.tm_mon - 14) / 12;
        liAux2 = (1461 * ( this->sim_time.tm_year + 4800 + liAux1)) / 4 + (367 * (this->sim_time.tm_mon
                                                                                  - 2 - 12 * liAux1)) / 12 - (3 * ((this->sim_time.tm_year  + 4900
                                                                                                                    + liAux1) / 100)) / 4 + this->sim_time.tm_mday - 32075;
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
        dEclipticLongitude = dMeanLongitude + 0.03341607 * sin ( dMeanAnomaly )
          + 0.00034894 * sin ( 2 * dMeanAnomaly ) - 0.0001134
          - 0.0000203 * sin (dOmega);
        dEclipticObliquity = 0.4090928 - 6.2140e-9 * dElapsedJulianDays
          + 0.0000396 * cos (dOmega);
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
        double dLatitudeInRadians;
        double dHourAngle;
        double dCos_Latitude;
        double dSin_Latitude;
        double dCos_HourAngle;
        double dParallax;
        dGreenwichMeanSiderealTime = 6.6974243242 +
          0.0657098283 * dElapsedJulianDays
          + dDecimalHours;
        dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime * 15
                                  + this->dLongitude) * rad;
        dHourAngle = dLocalMeanSiderealTime - dRightAscension;
        dLatitudeInRadians = this->dLatitude * rad;
        dCos_Latitude = cos ( dLatitudeInRadians );
        dSin_Latitude = sin ( dLatitudeInRadians );
        dCos_HourAngle = cos ( dHourAngle );
        dZenithAngle = (acos ( dCos_Latitude * dCos_HourAngle
                               * cos (dDeclination) + sin ( dDeclination ) * dSin_Latitude));
        dY = -sin ( dHourAngle );
        dX = tan ( dDeclination ) * dCos_Latitude - dSin_Latitude * dCos_HourAngle;
        this->AzimuthAngle[i] = atan2 ( dY, dX );
        if (   this->AzimuthAngle[i] < 0.0 )
          {
            this->AzimuthAngle[i] =   this->AzimuthAngle[i] + twopi;
          }
        this->AzimuthAngle[i] =   this->AzimuthAngle[i] / rad;
        // Parallax Correction
        dParallax = (dEarthMeanRadius / dAstronomicalUnit)
          * sin ( dZenithAngle);
        dZenithAngle = (dZenithAngle
                        + dParallax) / rad;
        if ((90 - dZenithAngle) > 0)

          {
            this->ElevationAngle[i] = 90 - dZenithAngle; //DEGREE
            count++;
            avgsinelevation = avgsinelevation + sin (this->ElevationAngle[i] * rad);
          }
        else
          {
            this->ElevationAngle[i] = 0; //the elevation angle can not be negative
          }
      }
      //an alternative formulation to calculate the elevation angle is:
      // this->ElevationAngle[i] =  asin((sin( dDeclination)*dSin_Latitude)+(cos(dDeclination)*dCos_HourAngle*dCos_Latitude))/rad;

    }//end for

  //Calculate the average elevation Angle
  avgsinelevation = (avgsinelevation / count);
  this->IncidentInsolation = (avgInsolation * 360) / avgsinelevation;

  NS_LOG_DEBUG  ("(Sun): Writing files Sun_info successful!");
  NS_LOG_UNCOND ("(Sun): CalculateSun OK!");


}  //end-function Calculate SunSource


void
Sun::calculateLightHours ()
{
  double sunrise = 0;
  this->Sunset = secDay;
  int i = 0;
  int j = secDay;
  while (this->ElevationAngle[i] <= 0)
    {
      i++;
      this->Sunrise = i;
    }
  NS_LOG_DEBUG  ("(Sun): Sunrise " << this->Sunrise / 3600); //hours
  while (this->ElevationAngle[j] == 0)
    {
      j--;
      this->Sunset = j;
    }
  NS_LOG_DEBUG  ("(Sun): Sunset  " << this->Sunset / 3600);  //hours
  this->Noon =  this->Sunrise;
  for (int i = sunrise; i < this->Sunset; i++)
    {
      if (this->ElevationAngle[i] > this->ElevationAngle[i - 1])
        {
          this->Noon++;
        }
    }
  this->LightHours = this->Sunset - this->Sunrise;
  NS_LOG_DEBUG   ("(Sun): Noon  " << this->Noon / 3600);//hours
}  //End Function calculateLightHours


std::ostream&
operator << (std::ostream& os, Sun sun)
{
  os << "Date: " << sun.getSimTime ().tm_mday << "-" << sun.getSimTime ().tm_mon << "-" <<  sun.getSimTime ().tm_year;
  os << ", Latitude: " << sun.getLatitude () << " [' N]" << " Longitude: " << sun.getLongitude () << " [' N]";
  os << ", Average Insolation: " << sun.getavgInsolation () << " [J]";

  return os;
}

}  // end namespace
