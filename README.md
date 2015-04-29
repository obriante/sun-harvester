# Sun-harvester Module Documentation

We propose an extension to ns-3 to accurately model the harvesting capabilities of a device powered by a photo-voltaic panel.
The proposed model takes into account the latitude where the devices is placed, the size of the photo-voltaic panel as well the orientation and tilt angle of this latter.

### Citations

If you use this ns-3 model for accademic purpose, authors appreciate if you cite:

"A Sun Energy Harvester Model for the Network Simulator 3 (ns-3)", G. Benigno, O. Briante, G. Ruggeri, (it is going to appear in proc of swansity 2015)



## Model Assumption

* The sun energy is available only during the light hours and for the remaining daily hours the harvestable energy is negligible;

* The amount of diffuse radiation is a percentage of current direct radiation;

* For each sun energy harvester into the network the harvestable energy is function of the same Insolation value.
This assumption implies that all the devices in a simulation are located at the same latitude;

* A Daily Averaged Solar Radiation data, related to a specific geographic place, is used as an input to the simulation.
From that input the amount of Instantaneously insolation on the photo-voltaic panel of each node in the simulation is computed depending on the specific size, orientation and tilting.


## Implementation Details

The solar energy harvesting system (S-EHS) model consists of a Sun Source and a set of Sun Energy Harvesters;
every Sun Energy Harvester is an implementation of the Energy Harvester contained into the energy framework
of ns-3. Energy Harvester implements all the basic mechanisms to recharge one or more Energy Source objects.
Every element structure is interconnected by a set of interfaces.
The Energy Source Model models the source that powers each node (e.g., rechargeable batteries or super-capacitors).
The implementation of such a model consist in two main classes: "Sun" and "SunEnergyHarvester".

### Sun Class

The Sun Class implements a set of functions to estimate the sun position and the relative insolation.
The solar harvestable energy depends on a few input parameters:

* Geographic location, designated by latitude and longitude;

* Date: year, month and day;

* Time: hours, minutes and seconds;

* Average Daily Perpendicular Insolation;

Implemented methods are:

* GetAzimuthAngle: This method returns the values of the sun Azimuth Angle;

* GetZenithAngle: This method returns the values of the sun Zenith Angle;

* GetElevationAngle: This method returns the values of the sun Elevation Angle;

* GetPerpendicularInsolation: This method returns the solar irradiation incident on a surface perpendicular to the sun;

* CalculateLightHours: This private method is called by CalculateSunSource to compute: the sunset time, the sunrise time and the daily hours of light;

* CalculateSunSource: a private method used to determine all the sun position parameters at ones. This function implements the Solar position algorithm (PSA).

### Sun Energy Harvester Class

The SunEnergyHarvester class represents the kernel of the model that estimates, with high accuracy, the instantaneously power archived by
an arbitrary tilted photo-voltaic panel oriented towards the incident insolation.

The input parameters are:

* the DC-DC converter efficiency;

* the solar cell efficiency;

* the panel azimuth angle;

* the panel tilt angle;

* the panel dimensions;

* the diffuse energy percentage;

Implemented methods are:

* DoGetPower: to connect our Sun Energy Harvester with one or more than one Energy Source. It also returns the currently power provided by the Energy Harvester.

* UpdateHarvestedPower: called every refresh time interval.

* CalculateHarvestedPower: to calculate the instantaneously harvestable power.

## Validation

Please refer to the paper reported in "Citations" for more details.
