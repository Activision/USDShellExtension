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
import pxr.Usdviewq as Usdviewq

class LauncherOverride(Usdviewq.Launcher):

    def LaunchPreamble(self, arg_parse_result):
        try:
            from PySide2 import QtGui
        except ImportError:
            from PySide import QtGui

        (app, appController) = super().LaunchPreamble(arg_parse_result)
        
        # set the icon that will appear in the taskbar
        appController._mainWindow.setWindowIcon(QtGui.QIcon('%PATH_TO_USD_ICO%'))
        # activate the window (SetForegroundWindow)
        appController._mainWindow.activateWindow()

        return (app, appController)

if __name__ == '__main__':
    # Let Ctrl-C kill the app.
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    try:
        LauncherOverride().Run()
    except Usdviewq.InvalidUsdviewOption as e:
        print("ERROR: " + e.message, file=sys.stderr)
        sys.exit(1)
