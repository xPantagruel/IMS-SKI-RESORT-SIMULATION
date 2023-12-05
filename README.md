# IMS-Project

## Build

    type `make` in directory witch main.cpp to build the project and generate the executable file `ims` or 'make run' to build and run the project

## Run

    type `./ims` to run the project

## Clean

    type `make clean` to clean the build files

## Usage

    ./ims [day|night] [-m marta1Imp] [-p pomaImp|pomaOpti] [-s <speed_value>] [-h]

## Arguments:

- day|night: Specify the mode of the tool (choose either 'day' or 'night'). Day is set as the default mode.

### Options:

- -m marta1Imp: Specify the mode as marta1Imp for experiment 2.
- -p pomaImp: Specify the mode as pomaImp for experiment 3.
- -p pomaOpti: Specify the mode as pomaOpti for experiment 4.
- -s <speed_value>: Specify the speed value (a double) of generating skiers in simulation.
- -h: Display the help message.

## Example:

./ims day -m marta1Imp -p pomaOpti -s 5.6

This command runs the './ims' tool in the 'day' mode with the following configurations:

- -m: marta1Imp mode.
- -p: pomaOpti mode.
- -s: Speed value of '5.6' added to default value of generator Exp time.

## Help:

For more information and usage details, run the tool with the '-h' option:

    ./ims -h

This will display the help message, explaining the available options and their usage.
