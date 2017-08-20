Genesis Game Engine
-------------------

Genesis is a comprehensive game engine for both 2D and 3D projects, presenting a few of the graphics 
features enabled by OpenGL 4.3.  Some of the features made possible by restricting development to 
more recent specifications include particle systems based on compute shaders and shader storage buffer 
objects, and terrain detail with the inclusion of tessellation shaders.  Standardized GLSL scripts 
provide the basis for a rich lighting model that will be iterated on over time to provide the most 
realistic environments possible over a wide range of potential projects.  

Many of the features have already been implemented in a number of OpenGL demos that you can browse at  
your leisure.  Current development focuses on hashing out a data-oriented architecture from the
current OOP implementation in order to optimize every last bit of code.

### Screenshots
<img src="http://i.imgur.com/M1AYyeE.jpg" width=293>
<img src="http://i.imgur.com/rkS8ezP.jpg" width=293>
<img src="http://i.imgur.com/15YIRQ3.jpg" width=293>
<img src="http://i.imgur.com/gcWuemh.jpg" width=293>
<img src="http://i.imgur.com/xPtCAwP.jpg" width=293>
<img src="http://i.imgur.com/XngJaro.jpg" width=293>
<img src="http://i.imgur.com/1MUQuuJ.png" width=293>
<img src="http://i.imgur.com/8A0Dla5.png" width=293>
<img src="http://i.imgur.com/qixOy2X.png" width=293>

### Additional features
* 3D rendering that works well with modern PCs.
* Abstraction of the numerous OpenGL calls into a simpler API.

### Todo
* Abstract Physics and AI gameplay code into extensible system classes.
* Create a simple data-oriented architecture.
* Extend the base object shaders to incorporate increasingly advanced lighting models.
* Add a simple UI, billboarding, and other helpful features.
* Improve collision detection algorithms.
* Improve general purpose algorithms such as random functions.

### How to build on Windows 7/8/10
You will require both the 2013 and 2015 Visual C++ redistributable packages.  Easiest way to fulfill this 
requirement is to just simply install both Visual Studio 2013 and Visual Studio 2015 **or** 2017, which you can find
[here.](https://www.visualstudio.com/en-us/news/releasenotes/vs2013-archive).

Make sure you have a graphics card that supports OpenGL 4.3.  You may have to install the latest drivers
provided by the manufacturer of your graphics card.  *Note that you will not need need Visual Studio 2015/2017
in order to run the code, and the solution works perfectly with just Visual Studio 2013.  However, in the 
interest of future development moving forward, I would like to at least describe the exact workflow of the 
current developers in order to mitigate potential issues.*

##### Downloading additional dependencies for runtime
Download the 32 bit version of [irrKlang.](http://www.ambiera.com/irrklang/downloads.html)

Put the following files from `bin/win32-visualStudio` into `Genesis-Engine/Debug`:
* ikpFlac.dll
* ikpMP3.dll
* irrKlang.dll
* You may also have to include **assimpd.dll** into the Debug folder, which you download [here.](http://assimp.sourceforge.net/main_downloads.html)

##### Setting up Visual Studio
* Open `Genesis.sln`.
* If you get a pop-up asking you to convert the C++ compiler, hit cancel.
* Under gengine **Configuration Properties** (right click gengine in the Solution Explorer > Properties):
	- VC++ Directories:  Include Directories.  Add `$(SolutionDir)Deps/Include/` by clicking the down arrow to the right of the field and selecting `<Edit...>`.
	- VC++ Directories:  Library Directories.  Add `$(SolutionDir)Deps/Libs/` by clicking the down arrow to the right of the field and selecting `<Edit...>`.
         - General: Platform Toolset.  Make sure `Visual Studio 2015 (v140)` is selected (if working in Visual Studio 2017).
* Under **Librarian > General** still in the gengine properties, make sure that the following **Additional Dependencies** are added by selecting `<Edit...>`:
	- opengl32.lib
	- glfw3.lib
	- SOIL.lib
	- glew32s.lib
	- assimpd.lib
	- irrKlang.lib
        - legacy_stdio_definitions.lib (if working in Visual Studio 2017)
* Under Genesis **Configuration Properties**:
	- VC++ Directories: Include Directories.  Add `$(SolutionDir)Deps/Include/`.
	- VC++ Directories: Library Directories.  Add `$(SolutionDir)Deps/Libs/` and `$(SolutionDir)Debug/` using the same method as above.
        - General: Platform Toolset.  Make sure `Visual Studio 2015 (v140)` is selected (if working in Visual Studio 2017).
* Under **Linker > Input** still in the Genesis properties, make sure that the following **Additional Dependencies** are added by selecting `<Edit...>`:
	- opengl32.lib
	- glfw3.lib
	- SOIL.lib
	- glew32s.lib
	- assimpd.lib
	- irrKlang.lib
	- gengine.lib
        - legacy_stdio_definitions.lib (if working in Visual Studio 2017)
	
In order to run the solution, you will need to set the build in the upper panel to `Debug` and `x86`. 
Then build both the game engine (gengine) and gameplay code (Genesis)  by right clicking Genesis 
in the Solution Explorer and selecting Build.

You can play through the various demos by accessing the `main.cpp` code and uncommenting the appropriate 
lines, making sure that only one function is uncommented at any given point.

##### Troubleshooting
If you have no audio devices available, a runtime exception will be thrown.

### Life of Gaben controls
* **Mouse** - Aim
* **WASD** - Movement
* **Spacebar** - Fire Weapon
* **E** - Plant Tower Mine If Available off Cooldown

### Rigid Body Simulation controls
* **Mouse** - Aim
* **WASD** - Movement
* **Spacebar** - Launch/Fire Box

### Authors and contributors
* Andrew Pham (Creator, developer)

### License
[MIT License](http://opensource.org/licenses/MIT)

### Contributing
If you are a developer, we need your help.  Genesis is a young project and we have a lot of new features
to implement and bugs to fix.  Feel free to refactor the current codebase and improve what we already have.  
Once the Visual Studio workflow is set, contributing is as easy as changing just a few lines of code.  Feel 
free to shoot me an email at andrew.pham@uwaterloo.ca to begin a correspondence on how you can help grow 
the project!
