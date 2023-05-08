# Tutorial_Adventure
Vulkan rendering project for a small game with the intend for me to learn all parts of the development process, including writing a renderer.

Installation:
The project requires a Vulkan installation that supports the VK_KHR_maintenance1 device extension (1.1 or higher)
To create the visual studio solution create a "build" folder in the root directory then run cmake from the root directory of the project.
The last thing is to create an assets folder within and copy the assets from the following google drive link: https://drive.google.com/file/d/1cX1BgzgiTwI6_j9xHB5l16V6umoxpJ7x/view?usp=sharing
(Sometimes this folder is updated so if any sprites look unintentional maybe update your assets)

Additional information:
 - The code in the main branch is always able to be compiled and run successfully while other branches like renderer may have errors as they are under development
 - Currently in the renderer.cpp files it is assumed that the exe is build with standard visual studio file structure and the root directory is found under "../../../" if for your build system this is not the case you may need to change the defines for SHADER_PATH and ASSET_PATH
 - To recompile the shaders the "shaders" folder contains the shader source code and the windows batch file "Compile.bat". However, currently to use it you have to change the path in the batch file to an existing glslc.exe (The Vulkan installation does contain this exe) 
