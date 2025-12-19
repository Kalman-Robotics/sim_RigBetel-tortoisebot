#!/bin/bash

# source ROS 2 environment
source /opt/ros/humble/setup.bash

sudo apt update
# update rosdep inside ros2 workspace
rosdep update
rosdep install --from-paths src --ignore-src -r -y

# build and install YDLidar-SDK
cd /ros2_ws/src/tortoisebot/YDLidar-SDK/build
cmake ..
make
sudo make install

# install required ROS 2 packages
sudo apt install -y \
    ros-humble-cartographer \
    ros-humble-cartographer-ros \
    ros-humble-gazebo-plugins \
    ros-humble-joint-state-publisher \
    ros-humble-nav2* \
    ros-humble-robot-state-publisher \
    ros-humble-teleop-twist-joy \
    ros-humble-teleop-twist-keyboard \
    ros-humble-urdf \
    ros-humble-xacro

# build and install project
cd /ros2_ws/
colcon build
source ./install/setup.bash

echo -e "\e[32m----------SETUP COMPLETE----------\e[0m"