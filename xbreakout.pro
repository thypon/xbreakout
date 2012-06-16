TEMPLATE = app
TARGET = xbreakout
DEPENDPATH += . sources
INCLUDEPATH += . sources

include(generic.pro)

win {
  include(windows.pro)
}

osx {
  include(osx.pro)
}
