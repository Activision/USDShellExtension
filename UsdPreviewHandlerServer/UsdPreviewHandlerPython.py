#  Copyright 2021 Activision Publishing, Inc. 
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

from __future__ import print_function
import sys
import os
import argparse
from pxr import Usd, UsdUtils, Sdf, UsdAppUtils
from pxr.Usdviewq.stageView import StageView
from pxr.UsdAppUtils.complexityArgs import RefinementComplexities
import UsdPreviewHandler

try:
    from PySide2.QtCore import *
    from PySide2.QtGui import *
    from PySide2.QtWidgets import *
except ImportError:
    from PySide.QtCore import *
    from PySide.QtGui import *

class Widget(QWidget):
    def __init__(self, stage=None, app=None, previewApp=None):
        super(Widget, self).__init__()

        self.setStyleSheet(u"")

        self.model = StageView.DefaultDataModel()

        
        self.view = StageView(dataModel=self.model,
                              printTiming=True)

        self.model.viewSettings.showHUD = False

        self.app = app
        self.previewApp = previewApp

        self.layout = QVBoxLayout(self)
        self.layout.addWidget(self.view)
        self.layout.setContentsMargins(0, 0, 0, 0)

        self.isLoadComplete = False

        if stage:
            self.setStage(stage)

    def setStage(self, stage):
        self.model.stage = stage

    def OnComplexity(self, action):
        self.model.viewSettings.complexity = RefinementComplexities.fromName(action.text())

    def OnShadingMode(self, action):
        self.model.viewSettings.renderMode = str(action.text())

    def OnToggleDisplayGuide(self, action):
        self.model.viewSettings.displayGuide = (self.actionDisplay_Guide.isChecked())

    def OnToggleDisplayProxy(self, action):
        self.model.viewSettings.displayProxy = (self.actionDisplay_Proxy.isChecked())

    def OnToggleDisplayRender(self, action):
        self.model.viewSettings.displayRender = (self.actionDisplay_Render.isChecked())

    def OnRendererPlugin(self, plugin):
        if not self.view.SetRendererPlugin(plugin):
            # If SetRendererPlugin failed, we need to reset the check mark
            # to whatever the currently loaded renderer is.
            for action in self.rendererPluginActionGroup.actions():
                if action.text() == self.view.rendererDisplayName:
                    action.setChecked(True)
                    break
            # Then display an error message to let the user know something
            # went wrong, and disable the menu item so it can't be selected
            # again.
            for action in self.rendererPluginActionGroup.actions():
                if action.pluginType == plugin:
                    self.statusMessage(
                        'Renderer not supported: %s' % action.text())
                    action.setText(action.text() + " (unsupported)")
                    action.setDisabled(True)
                    break

    def buildContextMenu_Renderer(self, contextMenu):
        self.rendererPluginActionGroup = QActionGroup(self)
        self.rendererPluginActionGroup.setExclusive(True)

        rendererMenu = contextMenu.addMenu("Hydra Renderer")

        pluginTypes = self.view.GetRendererPlugins()
        for pluginType in pluginTypes:
            name = self.view.GetRendererDisplayName(pluginType)
            action = rendererMenu.addAction(name)
            action.setCheckable(True)
            action.pluginType = pluginType
            self.rendererPluginActionGroup.addAction(action)

            action.triggered[bool].connect(lambda _, pluginType=pluginType:
                    self.OnRendererPlugin(pluginType))

        # Now set the checked box on the current renderer (it should
        # have been set by now).
        currentRendererId = self.view.GetCurrentRendererId()
        foundPlugin = False
        
        for action in self.rendererPluginActionGroup.actions():
            if action.pluginType == currentRendererId:
                action.setChecked(True)
                foundPlugin = True
                break

        # Disable the menu if no plugins were found
        rendererMenu.setEnabled(foundPlugin)

    def buildContextMenu_Complexity(self, contextMenu):
        self.actionLow = QAction("Low", self)
        self.actionLow.setObjectName(u"actionLow")
        self.actionLow.setCheckable(True)
        self.actionMedium = QAction("Medium", self)
        self.actionMedium.setCheckable(True)
        self.actionMedium.setObjectName(u"actionMedium")
        self.actionHigh = QAction("High", self)
        self.actionHigh.setCheckable(True)
        self.actionHigh.setObjectName(u"actionHigh")
        self.actionVery_High = QAction("Very High", self)
        self.actionVery_High.setCheckable(True)
        self.actionVery_High.setObjectName(u"actionVery_High")

        self.complexityGroup = QActionGroup(self)
        self.complexityGroup.setExclusive(True)
        self.complexityGroup.addAction(self.actionLow)
        self.complexityGroup.addAction(self.actionMedium)
        self.complexityGroup.addAction(self.actionHigh)
        self.complexityGroup.addAction(self.actionVery_High)
        self.complexityGroup.triggered.connect(self.OnComplexity)

        self.actionLow.setChecked(True)

        complexityMenu = contextMenu.addMenu("Complexity")
        complexityMenu.addAction(self.actionLow)
        complexityMenu.addAction(self.actionMedium)
        complexityMenu.addAction(self.actionHigh)
        complexityMenu.addAction(self.actionVery_High)

    def buildContextMenu_ShadingMode(self, contextMenu):
        self.actionWireframe = QAction("Wireframe", self)
        self.actionWireframe.setCheckable(True)
        self.actionWireframeOnSurface = QAction("WireframeOnSurface", self)
        self.actionWireframeOnSurface.setCheckable(True)
        self.actionSmooth_Shaded = QAction("Smooth Shaded", self)
        self.actionSmooth_Shaded.setCheckable(True)
        self.actionFlat_Shaded = QAction("Flat Shaded", self)
        self.actionFlat_Shaded.setCheckable(True)
        self.actionPoints = QAction("Points", self)
        self.actionPoints.setCheckable(True)
        self.actionGeom_Only = QAction("Geom Only", self)
        self.actionGeom_Only.setCheckable(True)
        self.actionGeom_Smooth = QAction("Geom Smooth", self)
        self.actionGeom_Smooth.setCheckable(True)
        self.actionGeom_Flat = QAction("Geom Flat", self)
        self.actionGeom_Flat.setCheckable(True)
        self.actionHidden_Surface_Wireframe = QAction("Hidden Surface Wireframe", self)
        self.actionHidden_Surface_Wireframe.setCheckable(True)

        self.shadingGroup = QActionGroup(self)
        self.shadingGroup.setExclusive(True)
        self.shadingGroup.addAction(self.actionWireframe)
        self.shadingGroup.addAction(self.actionWireframeOnSurface)
        self.shadingGroup.addAction(self.actionSmooth_Shaded)
        self.shadingGroup.addAction(self.actionFlat_Shaded)
        self.shadingGroup.addAction(self.actionPoints)
        self.shadingGroup.addAction(self.actionGeom_Only)
        self.shadingGroup.addAction(self.actionGeom_Smooth)
        self.shadingGroup.addAction(self.actionGeom_Flat)
        self.shadingGroup.addAction(self.actionHidden_Surface_Wireframe)
        self.shadingGroup.triggered.connect(self.OnShadingMode)

        self.actionGeom_Smooth.setChecked(True)

        complexityMenu = contextMenu.addMenu("Shading Mode")
        complexityMenu.addAction(self.actionWireframe)
        complexityMenu.addAction(self.actionWireframeOnSurface)
        complexityMenu.addAction(self.actionSmooth_Shaded)
        complexityMenu.addAction(self.actionFlat_Shaded)        
        complexityMenu.addAction(self.actionPoints)        
        complexityMenu.addAction(self.actionGeom_Only)        
        complexityMenu.addAction(self.actionGeom_Smooth)        
        complexityMenu.addAction(self.actionGeom_Flat)        
        complexityMenu.addAction(self.actionHidden_Surface_Wireframe)        

    def buildContextMenu_DisplayPurposes(self, contextMenu):
        self.actionDisplay_Guide = QAction("Guide", self)
        self.actionDisplay_Guide.setCheckable(True)
        self.actionDisplay_Guide.triggered.connect(self.OnToggleDisplayGuide)
        self.actionDisplay_Guide.setChecked(self.model.viewSettings.displayGuide)
        self.actionDisplay_Proxy = QAction("Proxy", self)
        self.actionDisplay_Proxy.setCheckable(True)
        self.actionDisplay_Proxy.triggered.connect(self.OnToggleDisplayProxy)
        self.actionDisplay_Proxy.setChecked(self.model.viewSettings.displayProxy)
        self.actionDisplay_Render = QAction("Render", self)
        self.actionDisplay_Render.setCheckable(True)
        self.actionDisplay_Render.triggered.connect(self.OnToggleDisplayRender)
        self.actionDisplay_Render.setChecked(self.model.viewSettings.displayRender)

        displayPurposesMenu = contextMenu.addMenu("Display Purposes")
        displayPurposesMenu.addAction(self.actionDisplay_Guide)
        displayPurposesMenu.addAction(self.actionDisplay_Proxy)
        displayPurposesMenu.addAction(self.actionDisplay_Render)

    def buildContextMenu(self):
        self.contextMenu = QMenu(self)
        self.buildContextMenu_Renderer(self.contextMenu)
        self.buildContextMenu_Complexity(self.contextMenu)
        self.buildContextMenu_ShadingMode(self.contextMenu)
        self.buildContextMenu_DisplayPurposes(self.contextMenu)

                              
    def closeEvent(self, event):
        
        # Ensure to close the renderer to avoid GlfPostPendingGLErrors
        self.view.closeRenderer()

    def contextMenuEvent(self, event):
        modifiers = self.app.keyboardModifiers()

        altModifer = ((modifiers & Qt.AltModifier) == Qt.AltModifier)
        shiftModifer = ((modifiers & Qt.ShiftModifier) == Qt.ShiftModifier)
        controlModifer = ((modifiers & Qt.ControlModifier) == Qt.ControlModifier)

        if not altModifer and not shiftModifer and not controlModifer:
            self.contextMenu.exec_(self.mapToGlobal(event.pos()))

    def timerEvent(self, event):

        if (not self.isLoadComplete) and (not self.view._isFirstImage):
            self.isLoadComplete = True
            self.previewApp.LoadComplete()

        eventData = self.previewApp.PeekEvent()
        while eventData.event != UsdPreviewHandler.UsdPreviewEvent.NoMoreEvents:
            
            if eventData.event == UsdPreviewHandler.UsdPreviewEvent.Quit:
                self.app.quit()
                
            eventData = self.previewApp.PeekEvent()

def setStyleSheetUsingState(app, resourceDir):
    # We use a style file that is actually a template, which we fill
    # in from state, and is how we change app font sizes, for example.

    # Qt style sheet accepts only forward slashes as path separators
    resourceDir = resourceDir.replace("\\", "/")

    fontSize = 10
    baseFontSizeStr = "%spt" % str(fontSize)
    
    # The choice of 8 for smallest smallSize is for performance reasons,
    # based on the "Gotham Rounded" font used by usdviewstyle.qss . If we
    # allow it to float, we get a 2-3 hundred millisecond hit in startup
    # time as Qt (apparently) manufactures a suitably sized font.  
    # Mysteriously, we don't see this cost for larger font sizes.
    smallSize = 8 if fontSize < 12 else int(round(fontSize * 0.8))
    smallFontSizeStr = "%spt" % str(smallSize)

    # Apply the style sheet to it
    sheet = open(os.path.join(resourceDir, 'usdviewstyle.qss'), 'r')
    sheetString = sheet.read() % {
        'RESOURCE_DIR'  : resourceDir,
        'BASE_FONT_SZ'  : baseFontSizeStr,
        'SMALL_FONT_SZ' : smallFontSizeStr }

    app.setStyleSheet(sheetString)

        
def main():
    programName = os.path.basename(sys.argv[0])
    parser = argparse.ArgumentParser(prog=programName,
        description='Preview for Windows Explorer')

    parser.add_argument('usdFilePath', action='store', type=str,
        help='USD file to preview')

    parser.add_argument('--hwnd', action='store', type=int,
        default=0,
        help='The HWND of the parent window')

    parser.add_argument('--usdviewqDir', action='store', type=str,
        help='Full path to the usdviewq python folder')

    UsdAppUtils.rendererArgs.AddCmdlineArgs(parser)

    args = parser.parse_args()

    with Usd.StageCacheContext(UsdUtils.StageCache.Get()):
        stage = Usd.Stage.Open(args.usdFilePath)

    previewApp = UsdPreviewHandler.UsdPreviewApp()

    app = QApplication([])

    setStyleSheetUsingState(app, args.usdviewqDir)

    window = Widget(stage, app, previewApp)
    window.setWindowFlags( Qt.Popup | Qt.Tool )
    window.setAttribute( Qt.WA_DontShowOnScreen )
    window.show()

    # poll for events every so often
    window.startTimer( 250 )

    try:
        import ctypes
        ctypes.pythonapi.PyCObject_AsVoidPtr.restype = ctypes.c_void_p
        ctypes.pythonapi.PyCObject_AsVoidPtr.argtypes = [ctypes.py_object]
        previewApp.SetParent( args.hwnd, ctypes.pythonapi.PyCObject_AsVoidPtr(window.effectiveWinId()), ctypes.pythonapi.PyCObject_AsVoidPtr(window.view.effectiveWinId()) )
    except:
        previewApp.SetParent( args.hwnd, window.effectiveWinId(), window.view.effectiveWinId() )

    window.setAttribute( Qt.WA_DontShowOnScreen, False )

    # Make camera fit the loaded geometry
    window.view.updateView(resetCam=True, forceComputeBBox=True)

    # force a draw when hidden
    window.view.glDraw()

    window.buildContextMenu()

    app.exec_()

if __name__ == "__main__":
    sys.exit(main())