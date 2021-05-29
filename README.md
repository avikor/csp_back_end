# CspBackEnd
This project is the back end side of [csp_front_end](https://github.com/avikor/csp_front_end). </br>
It uses [Restbed](https://github.com/Corvusoft/restbed) and [JSON for Modern C++](https://github.com/nlohmann/json) libraries 
to create a RESTful web service.</br>
This service uses the [constraint_satisfaction](https://github.com/avikor/constraint_satisfaction) library to solve constraint
satisfaction problems.</br>

## Dependencies
Quick installation instructions for [Restbed](https://github.com/Corvusoft/restbed) using 
[vcpkg](https://github.com/Microsoft/vcpkg) can be found [here](https://github.com/Corvusoft/restbed) 
under "Building restbed - Using vcpkg".</br>
[JSON for Modern C++](https://github.com/nlohmann/json) is a Header-only library and is already included in this git repository.</br>
[constraint_satisfaction](https://github.com/avikor/constraint_satisfaction) library is also already included and no 
further installation is required.

## Usage
Compile cspBackEnd.sln using Visual Studio, then run cspBackEnd.exe