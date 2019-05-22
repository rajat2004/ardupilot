## AirSim Setup

This is a temporary page describing the development setup of AirSim and how to use Ardupilot SITL with it (currently under development)

### Linux

This AirSim page describes how to build Unreal Engine, AirSim
<https://microsoft.github.io/AirSim/docs/build_linux/>

Setting up the Blocks Environment - <https://microsoft.github.io/AirSim/docs/unreal_blocks/>

Development Workflow in AirSim -  

- Updating the repo - Normal Git workflow
- Make any changes required
- Run the `build.sh` script in Airsim which will also copy the changes to the plugin directory
- Launch UnrealEngine Editor, choose Blocks environment and when prompted about missing .so files, press Yes to build it again.

- Troubleshooting - <https://microsoft.github.io/AirSim/docs/build_linux/>

### Windows

Build AirSim on Windows - <https://microsoft.github.io/AirSim/docs/build_windows/>

Setup Blocks Environment - <https://microsoft.github.io/AirSim/docs/unreal_blocks/>

Development Workflow - <https://microsoft.github.io/AirSim/docs/dev_workflow/>

### Using ArduCopterSolo vehicle

Before launching for the first time, go to `Edit->Editor Preferences`, in the 'Search' box type `CPU` and ensure that the `Use Less CPU when in Background` is unchecked.

Press Play after Editor loads.

See <https://github.com/Microsoft/AirSim/blob/master/docs/settings.md> for info about settings

Current `settings.json` file for launching Solo

```
{
  "SettingsVersion": 1.2,
  "LogMessagesVisible": true,
  "SimMode": "Multirotor",
  "OriginGeopoint": {
    "Latitude": -35.363261,
    "Longitude": 149.165230,
    "Altitude": 583
  },
  "Vehicles": {
      "Solo": {
          "VehicleType": "arducoptersolo",
          "UseSerial": false,
          "AllowAPIAlways": false,
          "UdpIp": "127.0.0.1",
          "UdpPort": 9003,
          "SitlPort": 9002 
        }
    }
}
```

Note: Many of the fields have the default values as above only but just specifying them

First launch AirSim, after that launch the ArduPilot SITL using 

```
sim_vehicle.py -v ArduCopter -f airsim-copter --add-param-file=libraries/SITL/examples/Airsim/quadX.parm --console --map
```

For closing, first stop the AirSim simulation by pressing the Stop button, then close Ardupilot
If Ardupilot is closed first, then UE hangs and need to force close it.
You can restart by just pressing the Play button and then start the Ardupilot side, no need to close the Editor completely and then start it again


Run on different machines - Change `Udpip` to the IP address of the machine running Ardupilot, use `--sim-address` to specify Airsim's IP address


Using different ports - 
`UdpPort` assigns the port no. which Ardupilot receives the sensor data from (i.e. the port that Airsim sends the data to)
Therefore, `SitlPort` assigns the motor control port

`--sim-port-in` shouuld be equal to sensor port i.e. `UdpPort`
`--sim-port-out` should be equal to motor control port i.e. `SitlPort`


Current state - Flying nicely with some of the params modified. Currently it's working at bigger timestamps, let's see if we can improve that

You'll probably need to force close AirSim till everything's working properly