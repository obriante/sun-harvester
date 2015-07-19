# Sun-harvester Module Documentation

We propose an extension to ns-3 to accurately model the harvesting capabilities of a device powered by a photo-voltaic panel.
The proposed model takes into account the latitude where the devices is placed, the size of the photo-voltaic panel as well the orientation and tilt angle of this latter.

### NOTE

<b style='color:red'> The module is under development.</b>

### Citations

If you use this ns-3 model for accademic purpose, authors appreciate if you cite:

<strong>G. Benigno, O. Briante, and G. Ruggeri, “A sun energy harvester model for the network simulator 3 (ns-3),” in SWANSITY 2015 - 2nd Workshop on Smart Wireless Access Networks for Smart cITY (IEEE SECON Workshop) (SWANSITY 2015), Seattle, USA, Jun. 2015.<strong>

## Model Assumption

* The sun energy is available only during the light hours and for the remaining daily hours the harvestable energy is negligible;

* The amount of diffuse radiation is a percentage of current direct radiation;

## Implementation Details

The solar energy harvesting system (S-EHS) model consists of a Sun Source and a set of Sun Energy Harvesters; every Solar Energy Harvester is an implementation of the Energy Harvester contained into the energy framework of ns-3.
Energy Harvester implements all the basic mechanisms to recharge one or more Energy Source objects.
Every element structure is interconnected by a set of interfaces.
The Energy Source Model models the source that powers each node (e.g., rechargeable batteries or super-capacitors).
The implementation of such a model consist in two main classes: "Sun" and "SolarEnergyHarvester".

### Sun Class

The Sun Class implements a set of functions to estimate the sun position and the relative incident insolation.

Implemented methods are:

* GetAirMass: This method returns the Air Mass factor for the selected location;

* GetIncidentInsolation: This method returns the instantaneous solar radiation incident on a surface perpendicular to the sun;

* PSA: This function implements the Solar position algorithm (PSA).

### Solar Energy Harvester Class

The SunEnergyHarvester class represents the kernel of the model that estimates, with high accuracy, the instantaneously power archived by
an arbitrary tilted photo-voltaic panel oriented towards the incident insolation.

The input parameters are:

* Latitude, Longitude, Altitude;

* the time between two consecutive periodic updates;

* the Date to simulate: year, month and day, hours, minutes and seconds;  (24 hours format: YYYY-MM-DD hh:mm:ss; Default: 2005-06-21 09:00:00)

* the DC-DC converter efficiency [%];

* the solar cell efficiency [%];

* the panel azimuth angle [degree];

* the panel tilt angle [degree];

* the panel dimension [m^2];

* the diffuse energy percentage [%];

Implemented methods are:

* DoGetPower: to connect our Solar Energy Harvester with one or more than one Energy Source. It also returns the currently power provided by the Energy Harvester.

* UpdateHarvestedPower: called every refresh time interval.

* CalculateHarvestedPower: to calculate the instantaneously harvestable power.

## Validation

Please refer to the paper reported in "Citations" for more details.

#TODO: Model validation with real hardware.
