
set(EMIPLIB_FOUND 1)


find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)

set(EMIPLIB_INCLUDE_DIRS  "C:/local2/jthread/include" "C:/local2/jrtplib/include" "D:/Qt/5.15.0/mingw81_64/include/" "D:/Qt/5.15.0/mingw81_64/include/QtGui" "D:/Qt/5.15.0/mingw81_64/include/QtCore" "D:/Qt/5.15.0/mingw81_64/.//mkspecs/win32-g++" "D:/Qt/5.15.0/mingw81_64/include//QtANGLE" "D:/Qt/5.15.0/mingw81_64/include/QtWidgets" "D:/Qt/5.15.0/mingw81_64/include/QtOpenGL" "C:/Program Files (x86)/emiplib/include")

set(EMIPLIB_LIBRARIES  "optimized" "C:/Program Files (x86)/emiplib/lib/libemiplib.lib" "debug" "C:/Program Files (x86)/emiplib/lib/libemiplib.lib" "-LC:/local2/jthread/lib" "-ljthread" "-LC:/local2/jrtplib/lib" "-ljrtp" "ws2_32" "winmm" "Qt5::Gui" "Qt5::Widgets" "Qt5::OpenGL")



