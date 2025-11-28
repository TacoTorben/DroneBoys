#!/bin/bash
set -e

echo "====== DroneBoys Setup Script (Ubuntu 24.04 + ROS2 Jazzy) ======"

# ------------------------------------------------------------
# 1) Verify Ubuntu version
# ------------------------------------------------------------
UBUNTU_VERSION=$(lsb_release -rs)
if [[ "$UBUNTU_VERSION" != "24.04" ]]; then
    echo "⚠️ Warning: This script is made for Ubuntu 24.04. You are on $UBUNTU_VERSION."
fi

# ------------------------------------------------------------
# 2) Install system dependencies
# ------------------------------------------------------------
echo ""
echo "📦 Installing required system packages..."
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    git \
    g++ \
    pkg-config \
    libglfw3-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libx11-dev \
    libxext-dev \
    libglvnd-dev \
    libglm-dev \
    libfmt-dev \
    libjpeg-dev \
    libpng-dev

echo "✔️ System libraries installed."

# ImGui note:
echo "ℹ️ ImGui is header-only and must be included in your project (not apt)."

# ------------------------------------------------------------
# 3) Install ROS 2 Jazzy dependencies
# ------------------------------------------------------------
echo ""
echo "📦 Installing ROS2 Jazzy dependencies..."

sudo apt install -y \
    ros-jazzy-rclcpp \
    ros-jazzy-rclcpp-action \
    ros-jazzy-std-msgs \
    ros-jazzy-ament-index-cpp \
    ros-jazzy-rosidl-default-runtime

echo "✔️ ROS2 packages installed."

# ------------------------------------------------------------
# 4) Create the drone_boys_images directory
# ------------------------------------------------------------
BASE="$HOME/drone_boys_images"

echo ""
echo "📁 Creating image folders at: $BASE"

mkdir -p "$BASE/input"
mkdir -p "$BASE/output"

echo "✔️ Created:"
echo "   $BASE/input"
echo "   $BASE/output"

# ------------------------------------------------------------
# 5) Final summary
# ------------------------------------------------------------
echo ""
echo "====== Setup Complete ======"
echo "You can now build your ROS2 workspace:"
echo "  cd ~/your_ws"
echo "  colcon build"
echo ""
echo "🎉 All dependencies installed and folders created!"