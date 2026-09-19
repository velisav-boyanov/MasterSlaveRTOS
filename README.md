# Install the prerequisites:
sudo apt update
sudo apt install git build-essential cmake

# Set up repos:
Clone my repo, enter the new folder and then clone the FreeRTOS-Kernel
- git clone https://github.com/velisav-boyanov/MasterSlaveRTOS.git
- cd MasterSlaveRTOS
- git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git

# To build:
In the root of the project
mkdir build; cd build
cmake ..
cmake --build .
./target_name
