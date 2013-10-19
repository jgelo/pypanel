import fileinput, os, sys
from distutils.core import Extension, setup
from distutils import sysconfig

# Full paths to imlib2-config and freetype-config, adjust as needed -
configs = ["/usr/bin/freetype-config", "/usr/bin/imlib2-config"]

# Adjust or add any additional include directories here -
idirs   = ["/usr/X11R6/include"]

# Add any additional library directories here -
ldirs   = []

# Add any additional compile options here -
cargs   = ["-Wall"]

# Full path to libImlib2 shared library
imlib2  = "/usr/lib/libImlib2.so.1"

#------------------------------------------------------------------------------
# The rest of this script should not need to be modified! 
#------------------------------------------------------------------------------
libs  = [] # libraries (listed without -l)
largs = [] # extra link arguments
defs  = [] # define macros
files = ["COPYING", "README", "ppicon.png", "pypanelrc"]
install_dir = sysconfig.get_python_lib() + "/pypanel"

# Check for Python Xlib
try:
    from Xlib import X, display, Xatom, Xutil
except:
    print "\nPyPanel requires the Python X library -"
    print "http://sourceforge.net/projects/python-xlib/"
    sys.exit()
   
# Parse the build options
for config in configs:
    package = os.path.split(config)[1]
    
    if os.path.isfile(config):
        for cflag in os.popen("%s --cflags" % config).read().strip().split(): 
            flag = cflag[:2]
            opt  = cflag[2:]    
            
            if flag == "-I" and opt not in idirs:
                idirs.append(opt)
            else:
                if cflag not in cargs:
                    cargs.append(cflag)    
                
        for lib in os.popen("%s --libs" % config).read().strip().split():
            flag = lib[:2]
            opt  = lib[2:]
            
            if flag == "-L" and opt not in ldirs:
                ldirs.append(opt)
            elif flag == "-l":
                if opt not in libs:
                    libs.append(opt)
            else:
                if lib not in largs:
                    largs.append(lib)
                
        if package == "freetype-config":
            defs.append(("HAVE_XFT", 1))
            if "Xft" not in libs:
                libs.append("Xft")
                
        if package == "imlib2-config":
            # Add the workaround for Imlib2 version 1.2.x and up -
            # Python dlopens libImlib2 with RTLD_LOCAL by default.  To avoid
            # undefined symbols, dlopen it first with the RTLD_GLOBAL flag.
            version = os.popen("%s --version" % config).read().strip()
            if float(version[:3]) >= 1.2:
                defs.append(("IMLIB2_FIX", 1))
            
    else:
        if package == "imlib2-config":
            print "\nPyPanel requires the Imlib2 library -"
            print "http://www.enlightenment.org/pages/imlib2.html"
            sys.exit()

# Fix the shebang and add the Imlib2 workaround if necessary
if len(sys.argv) > 1 and sys.argv[1] != "sdist":
    for line in fileinput.input(["pypanel"], inplace=1):
        if fileinput.isfirstline():
            print "#!%s -OO" % sys.executable
        else:
            print line,  
            
    if ("IMLIB2_FIX", 1) in defs:
        for line in fileinput.input(["ppmodule.c"], inplace=1):
            if "handle = dlopen" in line:
                print '    handle = dlopen("%s", RTLD_NOW|RTLD_GLOBAL);' % imlib2
            else:
                print line,   
                
                   
# Distutils config
module = Extension("ppmodule",
                   sources            = ["ppmodule.c"],
                   include_dirs       = idirs,
                   library_dirs       = ldirs,
                   libraries          = libs,  
                   extra_compile_args = cargs,
                   extra_link_args    = largs,
                   define_macros      = defs,                    
                  )  

setup(name             = "PyPanel",
      author           = "Jon Gelo",
      author_email     = "jongelo@gmail.com",
      version          = "2.4",
      license          = "GPL",
      platforms        = "POSIX",
      description      = "Lightweight panel/taskbar for X11 Window Managers",
      long_description = "See README for more information",
      url              = "http://pypanel.sourceforge.net",
      data_files       = [(install_dir, files)],
      scripts          = ["pypanel"],
      ext_modules      = [module]
     )
