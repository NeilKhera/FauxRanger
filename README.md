[![alt text](https://i.imgur.com/peAF6rc.png)](https://neilkhera.com)

## Installation
### Download Executable
The Windows and Linux executables can be found under the [Releases](https://github.com/Nekhera/FauxRanger/releases) section.
### Build Yourself
#### Prereqs
* Unreal Engine v4.26
* [Brushify.io Moon Pack](https://www.unrealengine.com/marketplace/en-US/product/brushify-moon-pack)
* [Substance in Unreal Engine Plugin](https://www.unrealengine.com/marketplace/en-US/product/substance-plugin)
> Ensure these are installed before opening the project
#### Build
1. Clone the repository with submodules
`git clone --recursive https://github.com/Nekhera/FauxRanger.git`
2. Open FauxRanger.uproject.
3. `File > Package Project > Windows/Linux`
4. That's it! :)
## How to run
### Pre-run ROS configuration
ROS connection is disabled by default. To enable ROS go to the Pause Menu, enter Network IP of ROS machine and toggle ROS on.

> Windows version: run `FauxRanger.exe`

> Linux version: run `./FauxRanger.sh`
### Running the rover

#### Keyboard Controls
* WASD to move the rover
* X to stop the rover
* Left/Right arrow keys to cycle between camera views
* C for third-person camera
* P to toggle panel
* ESC to Pause Menu

#### ROS Controls
The rover may be driven around through ROS `geometry_msgs/Twist` messages published on the `/cmd_vel` ROStopic.

## ROStopics
The following ROStopics are published and subscribed to by the simulator
### Publishers
* /stereo/left/image_raw
* /stereo/left/camera_info
* /stereo/left/tf
* /stereo/right/image_raw
* /stereo/right/camera_info
* /stereo/right/tf
* /imu/data
* /odom

> TODO: Replace these with a single topic

* /wheels/front/right/velocity
* /wheels/front/left/velocity
* /wheels/rear/right/velocity
* /wheels/rear/left/velocity
### Subscribers
* /cmd_vel
## License
MIT License. Copyright (c) 2020 Neil Khera

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
___
_fin._
