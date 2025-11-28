
#!/bin/bash
set -e

echo "====== DroneBoys Setup Script (Ubuntu 24.04 + ROS 2 Jazzy) ======"

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
echo "ℹ️ ImGui is header-only and must be included in your project (not apt)."

# ------------------------------------------------------------
# 3) Install ROS 2 Jazzy core + build tooling
# ------------------------------------------------------------
echo ""
echo "📦 Installing ROS 2 Jazzy core & build tools..."



sudo apt install -y \
    ros-jazzy-rclcpp \
    ros-jazzy-rclcpp-action \
    ros-jazzy-std-msgs \
    ros-jazzy-ament-index-cpp \
    ros-jazzy-rosidl-default-runtime \
    ros-jazzy-ament-cmake \
    ros-jazzy-ros-base \
    python3-colcon-common-extensions \
    python3-rosdep



# Initialize rosdep (safe to run multiple times)
sudo rosdep init || true
rosdep update 

# Source ROS 2 now and for future shells
if ! grep -q "source /opt/ros/jazzy/setup.bash" ~/.bashrc; then
    echo 'source /opt/ros/jazzy/setup.bash' >> ~/.bashrc
fi
source /opt/ros/jazzy/setup.bash



if [[ -d "src" ]]; then
    echo "📦 Installing package dependencies via rosdep..."
    rosdep install --from-paths src --ignore-src --rosdistro jazzy -y
fi


echo "✔️ ROS 2 core & build tooling installed and sourced."

# ------------------------------------------------------------
# 4) (Optional) Install workspace dependencies via rosdep
# ------------------------------------------------------------
# If you run this script from your workspace root (one level above src),
# this will install dependencies declared in your packages:
if [[ -d "src" ]]; then
    echo ""
    echo "🔍 Installing package dependencies from src/ via rosdep..."
    rosdep install --from-paths src --rosdistro jazzy -y --skip-keys=""
    echo "✔️ Package dependencies installed."
else
    echo ""
    echo "ℹ️ Skipping rosdep workspace install (no src/ in current directory)."
    echo "   To install later: cd <your_ws> && rosdep install --from-paths src --rosdistro jazzy -y"
fi

# ------------------------------------------------------------
# 5) Create the drone_boys_images directory
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
# 6) Final summary
# ------------------------------------------------------------
echo ""
echo "====== Setup Complete ======"
echo "You can now build your ROS 2 workspace:"
echo "  cd ~/your_ws"
echo "  source /opt/ros/jazzy/setup.bash"
echo "  rosdep install --from-paths src --rosdistro jazzy -y"
echo "  colcon build"
echo ""
echo "🎉 All dependencies installed and folders created!"
