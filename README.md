



### User guide:

*** First open 1 terminal write: ***
```bash
cd DroneBoys
colcon build
source install/setup.bash
```
*** To run finder "aka main file in drone_core": ***
```bash
ros2 run drone_core finder 
```
*** To run action server: ***
open new terminal, and write only first time:
```bash
source install/setup.bash
```
*** Then for every action: ***
```bash
ros2 action send_goal /drone_command drone_core/action/FinderAction "{command_type: 'field', target_pose: [1,1]}"
```
    the 'field' is the action. So far we have 'field', 'sky'

    target pose: [1,1] is name of input image and name of output image (without .jpeg)
    input images are those from images folder, and output you decise yourself (but only a number)
