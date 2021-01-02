[![alt text](https://i.imgur.com/peAF6rc.png)](https://neilkhera.com)

## Installation
### Download Executable
The Windows and Linux executables can be found under the [Releases](https://github.com/Nekhera/FauxRanger/releases) section.
### Build Yourself
#### Prereqs
* Unreal Engine v4.26
* [Brushify.io Moon Pack](https://www.unrealengine.com/marketplace/en-US/product/brushify-moon-pack)
> Ensure these are installed before opening the project
#### Build
1. Clone the repository with submodules
`git clone --recursive https://github.com/Nekhera/FauxRanger.git`
2. Open FauxRanger.uproject.
3. `File > Package Project > Windows/Linux`
4. That's it! :)
## How to run
### ROS setup
To enable ROS communication, a ROSbridge server must be started on a compatible system. The following configuration has been tested:
* Ubuntu 18.04
* ROS Melodic
* ROSbridge Suite v0.11.10

> NOTE! ROS Noetic running on Ubuntu 20.04 or newer versions of ROSbridge suite are NOT currently supported. This is dies to an incompatibility between the ROSintegration plugin and Python 3. This will be fixed in the future.

To build and install ROSbridge Suite v0.11.10, checkout [this](https://github.com/RobotWebTools/rosbridge_suite/commit/b04345fab6b2394adb74ec29297356d8ec552be9) commit into your catkin workspace and build as normal. The newest version of the apt package (ros-melodic-rosbridge-suite) is NOT supported.

Run the TCP server with the following command

`roslaunch rosbridge_server rosbridge_tcp.launch bson_only_mode:=True`

### Simulator setup
> Windows version: run `FauxRanger.exe`
> Linux version: run `./FauxRanger.sh`

The rover model may be changed in the Settings menu. Select an Environment to spawn the rover. ROS connection is disabled by default. To enable ROS go to the Pause Menu, enter the Network IP of ROS machine and toggle ROS on. If a connection is succesfully established, the client connection will be displayed in the roslaunch window. The simulator is now ready for use.

### Driving the rover

#### Keyboard Controls
* WASD to drive the rover
* X to stop
* Left/Right arrow keys to cycle between camera views
* C for third-person camera
* P to toggle panel
* ESC to Pause Menu
* O for solar panel camera view (PitRanger only)

#### ROS Controls
The rover may be driven through ROS `geometry_msgs/Twist` messages published on the `/moon_ranger_velocity_controller/cmd_vel` ROStopic.

## ROStopics
The following ROStopics are published and subscribed to by the simulator
### Publishers
* /stereo/left/image_raw
* /stereo/left/camera_info
* /stereo/left/tf
* /stereo/right/image_raw
* /stereo/right/camera_info
* /stereo/right/tf
* /wheels
* /moon_ranger_velocity_controller/odom

### Subscribers
* /moon_ranger_velocity_controller/cmd_vel
## License
MIT License. Copyright (c) 2020 Neil Khera

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
___
_fin._
