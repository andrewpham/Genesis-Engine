# Genesis-Engine
3D game engine focused on advanced lighting models and particle systems

# Windows Dev Env Setup
Install Visual Studio 2013 and 2015.

Make sure you have a graphics card supporting at least OpenGL 4.3. Download the latest graphics drivers.

## Additional Dependencies
Download the x86 version of [irrKlang](http://www.ambiera.com/irrklang/downloads.html)

Put the following files in the Directory `Genesis-Engine/Debug`

  ikpFlac.dll
  
  ikpMP3.dll
  
  irrKlang.dll

## Using Visual Studio 2015:
Open Genesis.sln
  don't convert the C++ compiler if you get a pop up. Hit cancel.
  
In gengine Properties (Right click gengine in Solution Explorer > Properties):
  VC++ Directories: Include Directories. Add `$(SolutionDir)Deps/Include/` to the path.
  
  VC++ Directories: Library Directories. Add `$(SolutionDir)Deps/Libs/` to the path.
  
  Librarian > General: Additional Dependencies. Add `irrKlang.lib` to the path.

In Genesis Properties:
  VC++ Directories: Include Directories. Add `$(SolutionDir)Deps/Include/` to the path.
  
  VC++ Directories: Library Directories. Add `$(SolutionDir)Deps/Libs/;$(SolutionDir)Debug/` to the path.
  
  Linker > Input: Additional Dependencies. Add `irrKlang.lib` to the path.

Set Debug Build to x86

Build both gengine and Genesis (Right click Solution 'Genesis' > Build)

Run the program (click on green arrow play button in toolbar labeled Local Windows Debugger)

You can change the demo games in main.cpp by commenting out the appropriate lines.

# Problems
If you have no audio devices available, a runtime exception will be thrown.
