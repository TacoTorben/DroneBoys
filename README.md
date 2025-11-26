
### Before use:
1. Make sure you have ros2 jazzy installed
    https://docs.ros.org/en/jazzy/Installation/Ubuntu-Install-Debs.html

2. Install missing dependencies:
```bash 
    sudo apt install python3-rosdep
    sudo rosdep init || true  
    rosdep update
    rosdep install --from-paths src --ignore-src -r -y
```
3. Setup image folder
In home folder (or where ever your ros2 workspace folder currently is)
Run:
```bash
    mkdir images
    cd images
    mkdir input
    mkdir output
```
4. Save images needed inside the input folder


### User guide:

*** First open 1 terminal write: ***
```bash
cd DroneBoys
colcon build  ## Until GUI works use colcon build --packages-select drone_core
source install/setup.bash
```
*** To run finder "aka main file in drone_core": ***
```bash
ros2 run drone_core finder 
```
*** To run action server: ***
open new terminal, and write only first time:
```bash
cd DroneBoys
source install/setup.bash
```
*** Then for every action: ***
```bash
ros2 action send_goal /drone_command drone_core/action/FinderAction "{command_type: 'field', target_pose: [1,1]}"
```
    the 'field' is the action. So far we have 'field', 'sky', 'dark'

    target pose: [1,1] is name of input image and name of output image (without .jpeg)
    input images are those from images folder, and output you decise yourself (but only a number)

*** To run the interface use: ***
```bash
ros2 run drone_gui gui_app 
```