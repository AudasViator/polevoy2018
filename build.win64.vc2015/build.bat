@rem set PATH="C:\Program Files (x86)\CMake 3.0\bin\";%PATH%
call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" x86

call cmake.exe -G"Visual Studio 14 Win64" -DOpenCV_DIR="C:\misis\polevoy\OpenCV\build" ..
msbuild polevoy_d_v.sln /property:Configuration=Debug /m
@rem msbuild polevoy_d_v.sln /property:Configuration=Release /m