win32:CONFIG(release, debug|release): LIBS += -L$$BUILD_DIR/markerdetector/release/ -lmarkerdetector
else:win32:CONFIG(debug, debug|release): LIBS += -L$$BUILD_DIR/markerdetector/debug/ -lmarkerdetector
else:unix: LIBS += -L$$BUILD_DIR/markerdetector/ -lmarkerdetector

INCLUDEPATH += $$SOURCES_DIR/markerdetector
DEPENDPATH += $$SOURCES_DIR/markerdetector

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$BUILD_DIR/markerdetector/release/libmarkerdetector.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$BUILD_DIR/markerdetector/debug/libmarkerdetector.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$BUILD_DIR/markerdetector/release/markerdetector.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$BUILD_DIR/markerdetector/debug/markerdetector.lib
else:unix: PRE_TARGETDEPS += $$BUILD_DIR/markerdetector/libmarkerdetector.a
