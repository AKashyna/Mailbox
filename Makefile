# Kompilator i flagi
CXX = g++
CXXFLAGS = -pthread -fPIC
LDFLAGS = -lsqlite3 -lQt5Core -lQt5Widgets -lQt5Gui
INCLUDES = -I /usr/include/x86_64-linux-gnu/qt5 \
           -I /usr/include/x86_64-linux-gnu/qt5/QtWidgets \
           -I /usr/include/x86_64-linux-gnu/qt5/QtGui \
           -I /usr/include/x86_64-linux-gnu/qt5/QtCore
LIBDIR = -L /usr/lib/x86_64-linux-gnu

# Pliki i katalogi
SRV_SOURCES = server.cpp Database.cpp
SRV_OUTPUT = server.out

GUI_SOURCES = client.cpp GUI.cpp
GUI_OUTPUT = GUI.out

# Cele kompilacji
all: $(SRV_OUTPUT) $(GUI_OUTPUT)

$(SRV_OUTPUT): $(SRV_SOURCES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

$(GUI_OUTPUT): $(GUI_SOURCES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) $(LIBDIR)

# Czyszczenie
clean:
	rm -f $(SRV_OUTPUT) $(GUI_OUTPUT)
