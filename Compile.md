# Compiliation notes


I wrote this project in Visual Studio 2022.
Now it is important to note that the project files are set up in accordance to my own personal configuration, which may not be the same as yours.
I guess it goes without saying that you may therefore need to alter the configuration for the include-directories, and the external library directories.

Now the pre-build.ps1 file is also set to copy the required .dll files into the output folder VS2022 is configured to put the compiled stuff in.
I did purposely set VS2022 to output the .exe files and such in a different directory from where the .obj files are stored. It always confused the crap outta me what the user would need and what not, so this way I hoped to deal with that issue (and it's better from the start anyway).

# 3rd party dependencies

The game requires the next 3rd party libraries as dependency:
- SDL2
- SDL2-Image
- SDL2-Mixer
- zlib

# The dependencies I wrote myself

The game also requires some dependencies I set up myself. These come in full source and can also be found here on GitHub:

- Tricky's Units
- TQSG (and with that also come TQSE and TQSA)
- SuperTed
- JCR6

# Quick script to get the source for the game itself and my own dependencies 
(and aside from having to alter the 'include'-directory, VS2022 should see everything correctly).
This script has been written in PowerShell.

Please note! I do advise to create a folder completely dedicated to my projects only and to run this script from there!

~~~PowerShell
mkdir BallPlay++/Game
git clone 'https://github.com/PhantasarProductions/ballplay-plus-plus.git' BallPlay++/Game
git clone 'https://github.com/jcr6/JCR6_CPP.git' JCR6
git clone 'https://github.com/Tricky1975/TrickyUnits_CPP.git' TrickyUnits
git clone 'https://github.com/TrickyGameTools/TQSG.git'
git clone 'https://github.com/TrickyGameTools/SuperTed.git'
~~~

That should set all my own stuff right. Please note, these dependencies were meant to be merged into the .exe file and were not supposed to be separate .dll (or .o files in Unix/Linux) or anything.

