# MarsRobot Application

This deployment serves as the reference for how to architect a two-wheeled robot in F Prime.

## Building and Running the MarsRobot Application

In order to build the MarsRobot application, or any other F´ application, we first need to generate a build directory. This can be done with the following commands:

```
cd MarsRobot
fprime-util generate
```

The next step is to build the MarsRobot application's code.
```
fprime-util build
```

## Running the application and F' GDS

The following command will spin up the F' GDS as well as run the application binary and the components necessary for the GDS and application to communicate.

```
cd MarsRobot
fprime-gds
```

To run the ground system without starting the MarsRobot app:
```
cd MarsRobot
fprime-gds --no-app
```

The application binary may then be run independently from the created 'bin' directory.

```
cd MarsRobot/build-artifacts/<platform>/bin/
./MarsRobot -a 127.0.0.1 -p 50000
```
