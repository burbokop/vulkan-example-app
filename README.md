# vukan-app


# Dependencies

### GLFW


ubuntu: `sudo apt-get install libglfw3 libglfw3-dev`<br>

other os: https://www.glfw.org

### Vulkan SDK

https://vulkan.lunarg.com

### Vulkan drivers

ubuntu: `sudo apt-get install mesa-vulkan-drivers`

### Free Type

ubuntu: `sudo apt-get install libfreetype6 libfreetype6-dev`


# vkQueueSubmit crash

1. because of incorect debug report callback setup
2. because of incorect command buffers content
