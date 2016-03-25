INCLUDEPATH += $$(OPENCV300)\include
LIBS += -L$$(OPENCV300)\x64\vc12\lib

debug {
LIBS +=  \
    -lopencv_world300d
}
release {
LIBS +=  \
    -lopencv_world300
}
