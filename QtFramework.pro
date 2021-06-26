QT += core gui #widgets opengl

# We assume that the compiler is compatible with the C++ 11 standard.
greaterThan(QT_MAJOR_VERSION, 4){
    CONFIG         += c++11
    QT             += widgets
} else {
    QMAKE_CXXFLAGS += -std=c++0x
}

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 5){
    QT             += openglwidgets
}

win32 {
    message("Windows platform...")

    INCLUDEPATH += $$PWD/Dependencies/Include
    DEPENDPATH += $$PWD/Dependencies/Include

    LIBS += -lopengl32 -lglu32

    CONFIG(release, debug|release): {
        contains(QT_ARCH, i386) {
            message("x86 (i.e., 32-bit) release build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86/" -lglew32
        } else {
            message("x64 (i.e., 64-bit) release build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x64/" -lglew32
        }
    } else: CONFIG(debug, debug|release): {
        contains(QT_ARCH, i386) {
            message("x86 (i.e., 32-bit) debug build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x86/" -lglew32
        } else {
            message("x64 (i.e., 64-bit) debug build")
            LIBS += -L"$$PWD/Dependencies/Lib/GL/x64/" -lglew32
        }
    }

    msvc {
      QMAKE_CXXFLAGS += -openmp -arch:AVX -D "_CRT_SECURE_NO_WARNINGS"
      QMAKE_CXXFLAGS_RELEASE *= -O2
    }
}

unix: !mac {
    message("Unix/Linux platform...")

    # for GLEW installed into /usr/lib/libGLEW.so or /usr/lib/glew.lib
    LIBS += -lGLEW -lGLU
}

mac {
    message("Macintosh platform...")

    # IMPORTANT: change the letters x, y, z in the next two lines
    # to the corresponding version numbers of the GLEW library
    # which was installed by using the command 'brew install glew'
    INCLUDEPATH += "/usr/local/Cellar/glew/x.y.z/include/"
    LIBS += -L"/usr/local/Cellar/glew/x.y.z/lib/" -lGLEW

    # the OpenGL library has to added as a framework
    LIBS += -framework OpenGL
}


FORMS += \
    GUI/MainWindow.ui \
    GUI/SideWidget.ui

QMAKE_CXXFLAGS += -std=gnu++14

HEADERS += \
    Bezier/BicubicBezierPatches.h \
    Bezier/BicubicCompositeSurface3.h \
    Bezier/CubicBezierArcs3.h \
    Bezier/CubicCompositeCurve3.h \
    Core/Colors4.h \
    Core/Constants.h \
    Core/DCoordinates3.h \
    Core/GenericCurves3.h \
    Core/HCoordinates3.h \
    Core/Lights.h \
    Core/LinearCombination3.h \
    Core/Materials.h \
    Core/Matrices.h \
    Core/RealSquareMatrices.h \
    Core/ShaderPrograms.h \
    Core/TCoordinates4.h \
    Core/TensorProductSurfaces3.h \
    Core/TriangularFaces.h \
    Core/TriangulatedMeshes3.h \
    Cyclic/CyclicCurves3.h \
    GUI/GLWidget.h \
    GUI/MainWindow.h \
    GUI/SideWidget.h \
    Core/Exceptions.h \
    Parametric/ParametricCurves3.h \
    Parametric/ParametricSurfaces3.h \
    Test/TestFunctions.h \
    Trigonometric/TrigonometricBernsteinSurfaces.h

SOURCES += \
    Bezier/BicubicBezierPatches.cpp \
    Bezier/BicubicCompositeSurface3.cpp \
    Bezier/CubicBezierArcs3.cpp \
    Bezier/CubicCompositeCurve3.cpp \
    Core/GenericCurves3.cpp \
    Core/Lights.cpp \
    Core/LinearCombination3.cpp \
    Core/Materials.cpp \
    Core/RealSquareMatrices.cpp \
    Core/ShaderPrograms.cpp \
    Core/TensorProductSurfaces3.cpp \
    Core/TriangulatedMeshes3.cpp \
    Cyclic/CyclicCurves3.cpp \
    GUI/GLWidget.cpp \
    GUI/MainWindow.cpp \
    GUI/SideWidget.cpp \
    Parametric/ParametricCurves3.cpp \
    Parametric/ParametricSurfaces3.cpp \
    Test/TestFunctions.cpp \
    Trigonometric/TrigonometricBernsteinSurfaces.cpp \
    main.cpp

DISTFILES += \
    ../build/debug/Models/Lucy.off \
    ../build/debug/Models/Spot.off \
    ../build/debug/Models/angel.off \
    ../build/debug/Models/bird.off \
    ../build/debug/Models/bug.off \
    ../build/debug/Models/cone.off \
    ../build/debug/Models/cube.off \
    ../build/debug/Models/dragon.off \
    ../build/debug/Models/elephant.off \
    ../build/debug/Models/gangster.off \
    ../build/debug/Models/goblet.off \
    ../build/debug/Models/horse.off \
    ../build/debug/Models/icosahedron.off \
    ../build/debug/Models/mouse.off \
    ../build/debug/Models/seashell.off \
    ../build/debug/Models/space_station.off \
    ../build/debug/Models/sphere.off \
    ../build/debug/Models/star.off \
    ../build/debug/models/Lucy.off \
    ../build/debug/models/Spot.off \
    ../build/debug/models/angel.off \
    ../build/debug/models/bird.off \
    ../build/debug/models/bug.off \
    ../build/debug/models/cone.off \
    ../build/debug/models/cube.off \
    ../build/debug/models/dragon.off \
    ../build/debug/models/elephant.off \
    ../build/debug/models/gangster.off \
    ../build/debug/models/goblet.off \
    ../build/debug/models/horse.off \
    ../build/debug/models/icosahedron.off \
    ../build/debug/models/mouse.off \
    ../build/debug/models/seashell.off \
    ../build/debug/models/space_station.off \
    ../build/debug/models/sphere.off \
    ../build/debug/models/star.off \
    matrixes.in
