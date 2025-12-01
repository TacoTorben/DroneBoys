
### Before use:
1. Make setup script an executrable:
```bash
chmod +x setup.sh
```
2. Run setup
```bash
./setup.sh
```
3. Insert images into folder created in folder called "drone_boys_images"
Remember, image inputs are numbers
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
ros2 action send_goal /drone_command drone_core/action/FinderAction "{method: 'training', command_type: 'field', image_info: [1,1]}"
```

or:

```bash
ros2 action send_goal /drone_command drone_core/action/FinderAction "{method: 'test',command_type: 'field', image_info: []}"
```
    The 'method' is for either training or testing. The testing method goes into the test folder and takes all images inside and puts it into a output folder inside. The 'training' method takes one image at a time.

    The 'field' is the program we run. So far we have 'field', 'sky', 'dark'

    image_info: [1,1] is name of input image and name of output image (without .JPG)
I   Input images are those from images folder, and output you decise yourself (but only a number)
    This only needs number inputs when used in training.

*** To run the interface use: ***
```bash
ros2 run drone_gui gui_app 
```w

User guide for interface not included, good luck

