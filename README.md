_* this project has been copied from its original home at SourceForge for posterity's sake._


## PyPanel v2.4 Copyright (c) 2003-2005 Jon Gelo (jongelo@gmail.com)
#### Distributed under the GNU General Public License v2, see COPYING for details.

PyPanel is a lightweight panel/taskbar written in Python and C for X11 window
managers.  It can be easily customized to match any desktop theme or taste.
PyPanel works with EWMH compliant WMs.

The panel displays currently running tasks/applications and can also be
configured to display the current desktop name, date/time, a system tray
(notification area) and application launcher.

When the panel is minimized, either manually or by autohide, it can be restored
by moving the mouse over the top or bottom of the screen where the panel was
displayed prior to minimization.

All image rendering (icons and background transparency/tinting) is handled by
Imlib2.

Mouse button events on panel objects are handled by the ~/.pypanelrc
configuration file.  The file is a python script which gets imported/executed
when PyPanel is invoked.  Ensure that it contains proper Python formatting and
no syntax errors or the panel will not start!
    
-------------------------------------------------------------------------------
                        Requirements (Minimum versions)
-------------------------------------------------------------------------------
1. Python v2.2                  http://www.python.org
2. Python X Library v0.12       http://python-xlib.sourceforge.net
3. Imlib2 v1.1.1                http://www.enlightenment.org/pages/imlib2.html
4. libXft v1.0 (optional)       http://fontconfig.org

-------------------------------------------------------------------------------
                      Installation (Upgrading) & Starting
-------------------------------------------------------------------------------
1. python setup.py install
2. pypanel &
3. If upgrading to 2.x from 1.3, you will need to replace your current 
   ~/.pypanelrc with the new version:
 
   a. Move your current ~/.pypanelrc to a backup location or backup name
   b. Start PyPanel so a new ~/.pypanelrc is created
   c. Kill the panel process
   d. Merge any changes you wish to keep from your backup config to the
      new version

-------------------------------------------------------------------------------
                                Uninstallation
-------------------------------------------------------------------------------
PyPanel installs the following files -

1. /usr/bin/pypanel (may be in /usr/local/bin or other depending on system)
2. <path to your Python library>/site-packages/ppmodule.so (.sl)
3. <path to your Python library>/site-packages/pypanel/
4. ~/.pypanelrc

-------------------------------------------------------------------------------
                                 Configuration
-------------------------------------------------------------------------------
1. Configuration is handled via the ~/.pypanelrc python script, it's
   executed when PyPanel is started.
2. If the level of customization from the config script isn't enough, hack
   the PyPanel script itself.

-------------------------------------------------------------------------------
                             Contributions & Thanks
-------------------------------------------------------------------------------
1. My sincerest thanks to the many PyPanel users who have shared their ideas,
   suggestions, comments, code snippets and overall support for this project.
2. The default app icon included with this distribution is from the Amaranth
   icon set by Michael Doches.  Available from http://www.kde-look.org
3. Original shadowed text and no-task display patches were contributed by
   Johannes Winkelmann.  
      
-------------------------------------------------------------------------------   
                                    Contact
-------------------------------------------------------------------------------
 + Jon Gelo 
 + jongelo@gmail.com
 + http://pypanel.sourceforge.net
 + https://github.com/jgelo/pypanel

-------------------------------------------------------------------------------
                                    History
-------------------------------------------------------------------------------
+  050626 v2.4 -   
  + New features:
    + Shadowed text
    + Ability to disable the task display
  + Bug fixes:
    + Fix for wmhint icons with icon mask > maxint (eg AleVT)
    + Fix when reading the class name of apps that don't set class name
    + Race condition for destroyed task - drawing its wmhints icon
  + New config options:
    + SHADOWS                : Enable text shadows
    + TASK\_SHADOW\_COLOR      : Normal task shadow color
    + FOCUSED\_SHADOW\_COLOR   : Focused task shadow color
    + SHADED\_SHADOW\_COLOR    : Shaded task shadow color
    + MINIMIZED\_SHADOW\_COLOR : Minimized task shadow color
    + DESKTOP\_SHADOW\_COLOR   : Desktop name shadow color
    + CLOCK\_SHADOW\_COLOR     : Clock name shadow color

+  050510 v2.3  -   
  + New features:
    + Improved support for Fluxbox, Blackbox, XFWM4 and Enlightenment
    + Ability to customize individual app icons
    + Added an Application Launcher
    + Added a check to enforce only one running instance of the panel
    + Option for the panel to stay above or below other apps (ABOVE)
  + Bug fixes:
    + Clock updates 
    + Several system tray and performance improvements
  + New config options:
    + ABOVE         : Panel is above or below other apps
    + APPL\_I\_WIDTH  : Application launcher icon width
    + APPL\_I\_HEIGHT : Application launcher icon height 
    + ICON\_LIST     : List of custom icons for specific apps
    + LAUNCHER      : To enable and position the application launcher
    + LAUNCH\_LIST   : List of executables and their icons
    + TRAY\_I\_WIDTH  : Can now be set to 0 to allow app specific size
            
+  050206 v2.2   - New features/options:
  + Read config from /etc/pypanelrc first if it exists
  + Option to show only minimized/iconified apps (SHOWMINIMIZED)
  + Option to show a border around the panel (SHOWBORDER)
  + Option to set the minimized panel size (HIDDEN\_SIZE)
  + Added panelButtonEvent() to handle button events on empty panel
  + Bug fixes:
    + Removed legacy WindowMaker support, it's now EWMH capable
    + Fixed setup.py to gather correct compile/link arguments
    + Added a workaround for failing Imlib2 image loaders

+ 040926 v2.0   - New features/options:
  + Now using Imlib2 for all image rendering
  + Panel background tinting using BG\_COLOR
  + Show desktop function (EWMH only)
  + Show apps from all desktops
  + EWMH task ordering (oldest to newest, left to right)
  + Support for XFWM4
  + Autohiding (AUTOHIDE)
  + Minimized panel now restored by moving mouse over the top or
  + bottom of the screen where the panel was displayed
  + Ability to define the clock update interval (CLOCK\_DELAY)
  + Ability to define each desktop's name (DESKTOP\_NAMES)
  + Ability to define a custom default app icon (ICON)
  + Bug fixes:
    + Clock overwriting 
    + Font/text display issues
    + Background change garbled panel text

+ 040719 v1.3   - 
  + Added a System Tray
  + More flexibility for the panel layout
  + Tweaked the config
  + Fixed a bug with font handling

+ 040328 v1.2   - 
  + Ability to change the color of focused tasks (FOCUSED\_COLOR)
  + Added taskFocus() method for button events which allows
  + tasks which aren't focused to become focused else it toggles
  + the tasks minimization.
  + Decreased the amount of time between clock updates when idle.
  + Bug fixes:
    + Crash with certain panel size/dimension settings
    + Unnecessary task refreshing caused flashing/blinking                

+ 040218 v1.1   - 
  + Bug fixes:
  + Broken hide list
  + Exception during get\_wm\_state

+ 040131 v1.0   - 
  + Xft support
  + Ability to minimize the panel (toggleHidden)
  + Ability to hide certain apps (HIDE\_LIST)
  + Removed the SHOWALL option
  + Numerous bug fixes and optimizations

+ 031109 v0.9   - 
  + Clock support
  + Customizable button event handling 
  + Ability to place the workspace name/clock on right/left
  + Vertical line seperators can be turned off
  + Bug fixes:
  + Background change or restart sometimes killed panel
  + Sending task to another desktop didn't update correctly

+ 031012 v0.8   - 
  + Create ~/.pypanelrc if it doesn't exist
  + Raise & focus a task via middle mouse click
  + Workspace switching via clicks/scroll on the workspace name

+ 031009 v0.7.1 - Bug fixes:
  + Strut and visible name hints
  + Icons with no/invalid masks
  + Tasks with no name                

+ 031004 v0.7   - OpenBox3 support
  + Window shading/unshading via right mouse click
  + Config option for panel width, x position and top/bottom
  + Config option for displaying all tasks or just minimized

+ 030927 v0.6   - 
  + Added EWMH support (Kahakai and PekWM) 

+ 030921 v0.5   - 
  + Initial Beta release
