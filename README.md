# Cordless Power Tool Vacuum Cleaner Starter

This source code is designed to detect current in a cordless power tool and to automatically turn on and off a remote-controlled socket in order to control a vacuum cleaner or dust extractor.

There are a lot more details available on the [https://www.cgtk.co.uk/woodwork/powertools/cordlessvacuumstarter](project page).

The source code of this project is maintained using [https://mercurial-scm.org](Mercurial).  However, since bitbucket stopped offering hosting for mercurial projects and github is generally more widely used, I figured I ought to share it on github.  If you would like to contribute by submitting code, it's probably easiest if you send me patch (or attach one to a new github issue).

# Downloading Pre-Compiled Binaries

Binary files for the various sockets are available [https://github.com/abudden/CordlessVacuumStarter/releases](at this link).

# Compilation

There are a couple of ways you can compile this project:

1. Using docker-compose
2. Installing all the build applications (python, gcc-arm and mbed-cli)

Docker is easier (especially if you're Linux and already have docker-compose installed!); installing all the requirements gives a bit more control and quicker builds, but is a bit more involved.

## Using Docker-Compose

Go to https://docs.docker.com/compose/install/ and follow the instructions there.  Once everything is installed, you should be able to run:

```
docker-compose build
docker-compose up
```

## Installing all the build applications

Required applications:

* [https://www.python.org](python) (version 3.5 or newer)
* [https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads](gcc-arm)
* mbed-cli (once you've installed python, run `python -m pip install mbed-cli`
* The [https://os.mbed.com/users/mbed_official/code/mbed-sdk-tools/](mbed-sdk-tools).  Either get these with Mercurial or download the zip repository and extract into the project folder (in a subfolder called "tools", which should contain lots of folders and python files).

Make sure all of the above commands are in your path (the easiest way to check is to run the following commands in a terminal / command window):

```
arm-none-eabi-g++ --version
python --version
mbed-cli --version
```

In a terminal / command window, navigate to the tools subdirectory within this project and run:

```
python -m pip install -r requirements.txt
python -m pip install jsonschema future pyelftools
```

Once all of that is installed and working okay, you should be able to build your version by running:

```
python compile.py --all
```

That command will build all variants (which takes a while); alternatively you can build for a specific socket with something like:

```
python compile.py --manufacturer Dewenwils --number 5
```

For more information, try:

```
python compile.py --help
```

# Programming

Assuming successful compilation, the binary file (extension `.bin`) will be in a folder named something like `build/Dewenwils-5/WEACT_BLACKPILL_F411CE`.

To program the microcontroller, follow the instructions on the [https://www.cgtk.co.uk/woodwork/powertools/cordlessvacuumstarter](project page).

<!-- vim: set ft=pandoc : -->
