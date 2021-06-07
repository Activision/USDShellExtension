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

# This is a fix for USD issue #1521
# https://github.com/PixarAnimationStudios/USD/issues/1521

import sys
try:
    from importlib.util import spec_from_loader, module_from_spec
    from importlib.machinery import SourceFileLoader 
except ImportError:
    import imp

def _SetupOpenGLContextFix(width=100, height=100):
    try:
        from PySide2 import QtOpenGL
        from PySide2.QtWidgets import QApplication
        from PySide2 import QtCore # Activision Change
    except ImportError:
        from PySide import QtOpenGL
        from PySide.QtGui import QApplication
        from PySide import QtCore # Activision Change

    application = QApplication(sys.argv)

    glFormat = QtOpenGL.QGLFormat()
    glFormat.setSampleBuffers(True)
    glFormat.setSamples(4)

    glWidget = QtOpenGL.QGLWidget(glFormat)
    glWidget.setFixedSize(width, height)
    # BEGIN - Activision Change
    glWidget.setAttribute( QtCore.Qt.WA_DontShowOnScreen )
    # END - Activision Change    
    glWidget.show()
    glWidget.setHidden(True)

    return glWidget

def main():
    try:
        spec = spec_from_loader("usdrecord", SourceFileLoader("usdrecord", sys.argv[0]))
        usdrecord = module_from_spec(spec)
        spec.loader.exec_module(usdrecord)
    except:
        usdrecord = imp.load_source('usdrecord', sys.argv[0])

    usdrecord._SetupOpenGLContext = _SetupOpenGLContextFix
    return usdrecord.main()

if __name__ == '__main__':
    sys.exit(main())
