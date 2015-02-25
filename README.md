# Welcome to ODataCpp-Server
ODataCpp is an open-source C++ library that implements the Open Data Protocol (OData). It supports the [OData protocol version 4.0](http://docs.oasis-open.org/odata/odata/v4.0/os/part1-protocol/odata-v4.0-os-part1-protocol.html). This is the server library that helps you build OData V4 service with C++.

# Getting started

## Getting the source

    git clone https://github.com/odata/odatacpp-server

## Building and testing

Currently the following target platforms are supported:

  * Windows 32-bit
  * OS X
  * Linux

### Building on Windows with Visual Studio 2012/2013

1.Please ensure that you have Visual Studio 2012/2013 installed.

2.Open 'odatacpp.sln' under 'odatacpp-server\msvc' with VS2012/VS2013 and click 'Build Solution' in the 'BUILD' menu.

3.Built libraries are placed under 'odatacpp-server\output\\\<Configuration\>' where '\<Configuration\>' could be either 'Debug' or 'Release' according to your build configuration in VS2012/VS2013.

### Building on Windows with MSBuild

1.Setup build environment for VS2012/VS2013:

    cd odatacpp-server
    powershell
    .\setup_ps_env_VS2012.ps1 (or .\setup_ps_env_VS2013.ps1)

   If you receive an error message like 'running scripts is disabled on this system', please run PowerShell as administrator, type the following command, and then rerun the setup script above.

    Set-ExecutionPolicy RemoteSigned

2.Here are some examples to invoke MSBuild:

   1) Build Debug version of ODataCpp-Server libraries in parallel.

    msbuild /m

   2) Build Release version of ODataCpp-Server libraries.

    msbuild /p:Configuration=Release

   3) Rebuild Debug version of ODataCpp-Server.

    msbuild /t:Rebuild /p:Configuration=Debug

   4) Clean build outputs.

    msbuild /t:Clean

3.Built libraries are placed under the same folder of VS2012/VS2013. Actually they are no different than the ones built by VS2012/VS2013.

### Running tests on Windows

1.After you have successfully built the libraries, you can run our functional tests to check the basic functionality.

  1) Test the Debug version of ODataCpp-Server Libraries:

    cd odatacpp-server\output\Debug
    TestRunner.exe odata_functional_test.vs11d.dll /Desktop

  2) Test the Release version of ODataCpp-Server Libraries:

    cd odatacpp\output\Release
    TestRunner.exe odata_functional_test.vs11.dll /Desktop

  The '/Desktop' option here indicates to run tests for desktop (rather than rt).

### Building on OS X

1.Please ensure that you have OS X later than 10.9, Xcode later than 5.0 and Xcode Command Line Tools installed.

2.Install the Homebrew package manager (http://brew.sh). Skip this step if you want to use your own package manager.

3.Install the required packages to build ODataCpp-Server via Homebrew or your own package manager.

    brew install cmake boost

4.Return to the root folder of ODataCpp-Server and generate 'Makefile' using CMake.

    cmake -DCMAKE_BUILD_TYPE=Debug # replace 'Debug' with 'Release' if needed
    make

5.Please find your built libraries under 'output'.

### Running tests on OS X

After successfully building the libraries, you can run the functional and end-to-end tests via the terminal.

    cd odatacpp-server/output
    ./test_runner *tests*

### Building on Linux

1.This document is based on Ubuntu 14.04 LTS 64-bit. Process on other Linux distributions should be similar.

2.Install the required packages to build ODataCpp-Server via apt-get or your own package manager.

    sudo apt-get install cmake libxml2 libxml2-dev libboost1.55-dev libboost-system1.55.0 libboost-system1.55-dev libboost-locale1.55.0 libboost-locale1.55-dev libboost-filesystem1.55.0 libboost-filesystem1.55-dev

  You can choose other versions of boost to install but they are not guaranteed to work.

3.Return to the root folder of ODataCpp-Server and generate 'Makefile' using CMake.

    cmake -DCMAKE_BUILD_TYPE=Debug # replace 'Debug' with 'Release' if needed
    make # don't build in parallel because gcc will be very likely to crash

4.Please find your built libraries under 'output'.

### Running tests on Linux

After successfully building the libraries, you can run the functional and end-to-end tests via the terminal.

    cd odatacpp-server/output
    ./test_runner *tests*

# Community
## Issue tracker
To report bugs and require features, please use our [issue tracker](https://github.com/odata/odatacpp-server/issues?state=open).

## Team blog
Please visit http://blogs.msdn.com/b/odatateam/.
