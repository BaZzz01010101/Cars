# Cars

Welcome to the **Cars** project!

This is a game with physics simulation and death match multiplayer mode. It was developed for a purpose of learning to prepare for the position of a game mechanics developer. The physics was written from scratch, RayLib was used for rendering, and RakNet was used for network communication.

## Dependencies
- [RayLib](https://www.raylib.com)
- [GLFW](https://www.glfw.org)
- [RakNet](http://www.jenkinssoftware.com)

## Build Guide
In order to build this project you need Visual Studio 2022. All dependencies are included in the repository.
- Run `git clone https://github.com/BaZzz01010101/Cars.git` from command line.
- Open **cars.sln** from the folder root
- Build the solution
- Run `cars.exe -test`

## Command Line
When the game starts without CLI options it will run only the game client that will try to connect using the default host:port (`127.0.0.1:60000`).
The executable supports the following CLI options:
- `-server` - run dedicated server
- `-test` - run both server and client as single process
- `-host 213.23.45.116` - uses provided IP for connecting/listening (default host: 127.0.0.1, for server use ```-host ""``` to listen on all interfaces)
- `-port 61000` - uses provided port for connecting/listening (default port: 60000)
- `-password 12345` - set the password for server or client connection
- `-players 2` - set max number of players limit for server (default value: 4, the max value can't be higher than Scene::MAX_CARS constant)
- `-wx 250` - set the X position for the client window (for testing)
- `-wy 120` - set the Y position for the client window (for testing)

The most simple way to start the game locally is to run ```cars.exe -test```.
If you want to play with friends, you need a server in a cloud or an external IP on your PC. In this case you should start the server by ```cars.exe -server -host ""``` and then connect to it by ```cars.exe -host SERVER_IP```.

## Configuration
The solution contains **Config.cpp** with many configuration parameters you can play with. 
### You must ensure that the physics and collisions configuration on the server and the client are the same.
It's a good idea to increase the version in the **version.hpp** file every time you make a breaking change in the code to avoid compatibility issues between server and clients.

## Controls
Use WSAD + Mouse to control the car and weapon.
Most of the hotkeys can be found in **ClientApp.cpp** in the **updateShortcuts** method. Here is the list of them:
- `W` / `UP` - accelerate
- `S` / `DOWN` - brake / reverse
- `A` / `LEFT` - turn left
- `D` / `RIGHT` - turn right
- `SPACE` - handbrake
- `LEFT MOUSE BUTTON` - primary fire (machine gun)
- `RIGHT MOUSE BUTTON` - secondary fire (cannon)
- `MIDDLE MOUSE BUTTON` - toggle camera zoom
- `Y` - invert mouse Y axis
- `LEFT SHIFT` - vertical thrust
- `LEFT ALT + LEFT CONTROL + SPACE` - reconnect to the server (for testing)
- `T` - render only wire frames
- `LEFT ALT + D` - show/hide debug info
- `TAB / SHIFT + TAB` - show graph for next / previous debug value
- `=` - make current value always displayed on graphs
- `-` - remove current value from always displaying on graphs
- `BACKSPACE` - remove all values from always displaying on graphs
- `U` - toggle drawing the server side car position (working only in `-test` mode and used for testing only)

## License
This project is licensed under the MIT License, see the [LICENSE.txt](LICENSE.txt) file for details.

## Third-Party Licenses

This project uses the following third-party libraries:

- **RayLib** - Licensed under the Zlib/libpng License. See [LICENSE.txt](3rd_party/raylib/LICENSE.txt) file for details.
- **GLFW** - Licensed under the Zlib/libpng License. See [LICENSE.txt](3rd_party/glfw/LICENSE.txt) file for details.
- **RakNet** - Licensed under the BSD 3-Clause License. See [LICENSE.txt](3rd_party/raknet/LICENSE.txt) file for details.

## Videos
[![Gameplay](https://img.youtube.com/vi/UEHLkFHvYt8/0.jpg)](https://www.youtube.com/watch?v=UEHLkFHvYt8)

## Screenshots
![Image 1](https://github.com/BaZzz01010101/Cars/assets/11557955/5856f344-1d54-4901-9a37-e95bdd670ec7)
![Image 2](https://github.com/BaZzz01010101/Cars/assets/11557955/62666d77-dd38-4526-9557-7ef87281e8ba)
![Image 3](https://github.com/BaZzz01010101/Cars/assets/11557955/82823434-7586-4c43-b654-de15d62ca50b)



