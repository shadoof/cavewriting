/**
 * \author Daniel Keefe (dfk)
 *
 * \file  VRApp.H
 *
 */


#ifndef VRAPP_H
#define VRAPP_H

//#include <SDL.h>
#include <G3D/G3DAll.h>
#include <iostream>
using namespace std;

#include "DisplayTile.H"
#include "InputDevice.H"
#include "ProjectionVRCamera.H"

#ifdef USE_CLUSTER
#include <synchit_client.H>
#define VRG3D_DEFAULT_SYNCHIT_PORT 4045
#else 
#define CLUSTER_NET_PORT 4045
#endif


namespace VRG3D {


/** Base class for a basic G3D-style VR application.  This class
    provides a typical application structure that may be useful for
    many applications.  See vrg3d-demo.cpp for an example.
*/
class VRApp
{
public:

  /// Call init() after creating a new VRApp
  VRApp();
  virtual ~VRApp();

  /// This is an easy constructor to use for many typical situations.
  /// The vrSetup argument corresponds to a config file that specifies
  /// the type of window to open and devices to connect to.  Look in
  /// $G/share/VRG3D/desktop.vrsetup for an example.  Or, run with
  /// vrSetup == "" to print out a list of installed VR setups.
  virtual void init(const std::string &vrSetup, Log* appLog=NULL);

  /// This constructor provides flexibility for other non-standard setups.
  virtual void init(GWindow *_gwindow, RenderDevice *_renderDevice, DisplayTile _tile, 
                    Array<InputDevice*> _inputDevices, CoordinateFrame initialHeadFrame, 
                    const std::string &_clusterServerName="", Log *_log=NULL);

  /// This is intended to normally be reimplemented in subclasses
  virtual void doUserInput(Array<VRG3D::EventRef> &events);

  /// This is also intended to normally be reimplemented in subclasses
  virtual void doGraphics(RenderDevice *rd);

  /// Call this after init() to startup the program.
  virtual void run();

  /// If you need access to the raw GEvent's generated by G3D, you can
  /// reimplement this, but use with care.  One reason to get GEvent's
  /// is to work with the G3D::Gui* 2D screen gui elements.  The
  /// default implementation of this routine already does the
  /// necessary processing for you to use G3D::Gui* if you register
  /// your GuiWindow with VRApp's _widgetManager.
  virtual void guiProcessGEvents(Array<GEvent> &gevents, Array<VRG3D::EventRef> &newGuiEvents);

  /// When working with G3D::Gui*, use this routine rather than
  /// GuiPane::addButton() to have the event generated by the button
  /// click transformed into a VRG3D::Event named eventToGenerate.
  virtual GuiButton* addGuiButtonAndTrapEvent(GuiPane *pane,
                                              const std::string &eventToGenerate,
                                              const GuiCaption &text, 
                                              GuiTheme::ButtonStyle style=GuiTheme::NORMAL_BUTTON_STYLE);

  /// This is the guts of the main program loop.  It's unusual to need
  /// to reimplemet this.
  virtual void oneFrame();
  /// This is the graphics portion of the main program loop.  It's
  /// unusual to need to reimplement this.
  virtual void oneFrameGraphics();


  CoordinateFrame       getHeadFrame()         { return _camera->getHeadFrame(); }
  Log*                  getLog()               { return _log; }
  ProjectionVRCameraRef getCamera()            { return _camera; }
  RenderDevice*         getRenderDevice()      { return _renderDevice; }
  bool                  getReadyToEndProgram() { return _endProgram; }
  UserInput*            getG3DUserInput()      { return _userInput; }  
  WidgetManagerRef      getG3DWidgetManager()  { return _widgetManager; }

  /// For clustered rendering (Client side)
  void clientSetup();
  void clientRequestEvents();
  void clientReceiveEvents(Array<EventRef> &events);
  void clientRequestAndWaitForOkToSwap();
  void clientCheckConnection();

  void setClearColor(Color4 clearColor) { _clearColor = clearColor; }

  static void setupInputDevicesFromConfigFile(const std::string &filename, Log* log, 
                                              Array<InputDevice*> &inputDevices);

  static void pollWindowForGEvents(RenderDevice *rd, Array<GEvent> &gEvents);

  static void appendGEventsToEvents(RenderDevice *rd, DisplayTile &tile, Array<EventRef> &events, 
                                    Array<GEvent> &gEvents, Vector2 &mousePos);

  static std::string getKeyName(GKey::Value key, GKeyMod mod);


  /// Looks for filename first in the current directory, if it is not there,
  /// then looks in the default location $G/lib/VRG3D/.  Returns the
  /// full path to the file if it is found or "" if not.
  static std::string findVRG3DDataFile(const std::string &filename);

protected:

  Log*                  _log;
  DisplayTile           _tile;
  GWindow*              _gwindow;
  RenderDevice*         _renderDevice;
  Array<InputDevice*>   _inputDevices; 

  ProjectionVRCameraRef _camera;
  bool                  _endProgram;
  Vector2               _curMousePos;
  int64                 _frameCounter;
  Color4                _clearColor;

  bool                  _isAClusterClient;
  std::string           _clusterServerName;
#ifdef USE_CLUSTER
  SynchItClient*        _clusterSynchIt;
  char*                 _clusterSynchItBuf;
#else
  NetworkDevice*        _networkDevice;
  ReliableConduitRef    _conduitToServer;
#endif


  // To support integration of G3D::Gui* 
  WidgetManagerRef               _widgetManager;
  UserInput                     *_userInput;
  Table<GuiButton*, std::string> _guiBtnToEventMap;
};


/// Sleeps for milliseconds
void vrg3dSleepMsecs(double msecs);
/// Sleeps for seconds
void vrg3dSleepSecs(double secs);


} // end namespace



#endif


