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
