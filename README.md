# FUS_Toolbox_Cpp_Qt

## Setting up the project:
	1. Clone the repository
	2. Make sure you have Qt installed on your system
	3. Make sure Qt version is set in the project file correctly
	4. Make sure all the Qt modules are selected in the project properties (Qt Serial Port might be unselected)
		core;gui;widgets;printsupport;serialport;charts;datavisualization;designer;uitools;uiplugin
	5. Make sure the Pico SDK is installed on your system.
		5.1. In prooject properties, C/C++ -> General -> Additional Include Directories, add the path to the Pico SDK
		5.2. In project properties, C/C++ -> General -> Additional Include Directories, add the path to the Resources folder in the project directory
		5.3. In project properties, Linker -> General -> Additional Library Directories, add the path to the Pico SDK lib folder
		5.4. In project properties, Linker -> General -> Additional Library Directories, add the path to the Resources folder in the project directory
	6. In project properties, Qt Project Settings -> General -> Build Config -> Release
	7. In project properties, Qt Project Settings -> General -> Multi-processor Compilation -> Yes (/MP)
	8. In project properties, C/C++ - > General -> Debug Information Format -> None
	9. In project properties, C/C++ -> Optimization -> Optimization -> Maximum Optimization (Favor Size) (/O1)
	10. In project properties, Linker -> Additional Dependencies -> Add these: ps4000.lib;visa64.lib
	11. In project properties, Linker -> Debugging -> Generate Debug Info -> No