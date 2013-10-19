/*
PyPanel v2.4 - Lightweight panel/taskbar for X11 window managers
Copyright (c) 2003-2005 Jon Gelo (jongelo@gmail.com)

This file is part of PyPanel.
PyPanel is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <Python.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Imlib2.h>

#ifdef IMLIB2_FIX
#include <dlfcn.h>
#endif

typedef unsigned long CARD32;

Display *dsp;
GC gc;
int scr;

#ifdef HAVE_XFT
#include <X11/Xft/Xft.h>
Colormap cmap;
Visual *visual;
XftDraw *draw;
XftFont *xf;
#else
XFontStruct *xf;
#endif

/*----------------------------------------*/
int pperror(Display *dsp, XErrorEvent *e) {
/*----------------------------------------*/
    return 0;
}

/*---------------------------------------------------------*/
static PyObject * _ppclear(PyObject *self, PyObject *args) {
/*---------------------------------------------------------*/
    Window win;
    int x, y, w, h;
    
    
    if (!PyArg_ParseTuple(args, "liiii", &win, &x, &y, &w, &h))
        return NULL;
    
    XClearArea(dsp, win, x, y, w, h, False);
    XFlush(dsp);
    return Py_BuildValue("i", 1);
}
    
/*--------------------------------------------------------*/
static PyObject * _ppfont(PyObject *self, PyObject *args) {
/*--------------------------------------------------------*/
#ifdef HAVE_XFT
    XftColor xftcol;
    XGlyphInfo ginfo;
    XRenderColor rcol;
#endif
    CARD32 pixel;
    Window win;
    XColor xcol;
    char *text;
    int len, font_y, p_height;
    float font_x, limit;
    
    if (!PyArg_ParseTuple(args, "llfifs#", &win, &pixel, &font_x, &p_height,
                          &limit, &text, &len))
        return NULL;
    
    xcol.pixel = pixel;
        
#ifdef HAVE_XFT
    if (limit) {
        while (1) {
            XftTextExtentsUtf8(dsp, xf, text, len, &ginfo);
            if (ginfo.width >= limit)
                len--;
            else
                break;
        }
    }
     
    XQueryColor(dsp, cmap, &xcol);
    font_y     = xf->ascent+((p_height-(xf->ascent+xf->descent))/2);
    rcol.red   = xcol.red;
    rcol.green = xcol.green;
    rcol.blue  = xcol.blue;
    rcol.alpha = 0xffff;
    XftColorAllocValue(dsp, visual, cmap, &rcol, &xftcol);
    XftDrawStringUtf8(draw, &xftcol, xf, font_x, font_y, text, len);
    XftColorFree(dsp, visual, cmap, &xftcol);
#else
    if (limit) {
        while (XTextWidth(xf, text, len) >= limit)
            len--;
    }
    
    XSetForeground(dsp, gc, pixel);
    font_y = xf->ascent+((p_height-xf->ascent)/2);
    XDrawString(dsp, win, gc, font_x, font_y, text, len);
#endif
    XFlush(dsp);
    return Py_BuildValue("i", 1);
}

/*------------------------------------------------------------*/
static PyObject * _ppfontsize(PyObject *self, PyObject *args) {
/*------------------------------------------------------------*/
#ifdef HAVE_XFT
    XGlyphInfo ginfo;
#endif
    char *text;
    int len;
    
    
    if (!PyArg_ParseTuple(args, "s#", &text, &len))
        return NULL;
    
#ifdef HAVE_XFT
    XftTextExtentsUtf8(dsp, xf, text, len, &ginfo); 
    return Py_BuildValue("i", ginfo.width);
#else
    return Py_BuildValue("i", (int)XTextWidth(xf, text, len));
#endif
}

/*--------------------------------------------------------*/
static PyObject * _ppicon(PyObject *self, PyObject *args) {
/*--------------------------------------------------------*/
    Imlib_Image icon;
    Pixmap win_icon, win_mask;
    Window panel, root;
    XStandardColormap *scm;
    char *data, *path; 
    int y, w, h, i_w, i_h, s1, s2;
    float x;
    
    if (!PyArg_ParseTuple(args, "lllfiiiiis#s#", &panel, &win_icon, &win_mask,
                          &x, &y, &w, &h, &i_w, &i_h, &data, &s1, &path, &s2))
        return NULL;
        
    if (s2 > 0)
        /* custom app icon */
        icon = imlib_load_image(path);
    else if (s1 > 0) 
        /* _net_wm_icon */
        icon = imlib_create_image_using_data(w, h, (DATA32*)data);
    else if (win_icon) {
        /* wmhints icon */
        if (!XGetGeometry(dsp, win_icon, &root, &s1, &s1, &s1, &s1, &s1, &s1))
            icon = NULL;
        else {
            scm = XAllocStandardColormap();
            imlib_context_set_drawable(win_icon);
            imlib_context_set_colormap(scm->colormap);
            icon = imlib_create_image_from_drawable(win_mask, 0, 0, w, h, 1);
            XFree(scm);
        }
    }
    else
        /* no icon defined */
        icon = NULL;
    
    if (!icon)
        return Py_BuildValue("i", 0);
    
    imlib_context_set_image(icon);
    imlib_image_set_has_alpha(1);
    imlib_context_set_drawable(panel);
    imlib_context_set_blend(1);
    imlib_render_image_on_drawable_at_size(x, y, i_w, i_h);
    imlib_free_image();
    return Py_BuildValue("i", 1);
}

/*---------------------------------------------------------*/
static PyObject * _ppshade(PyObject *self, PyObject *args) {
/*---------------------------------------------------------*/ 
    Imlib_Image bg;
    Pixmap bgpm, mask, rpm;
    Window panel;
    char filter[100];
    int x, y, w, h, r, g, b, a;
 

    if (!PyArg_ParseTuple(args, "lliiiiiiii", &panel, &rpm, &x, &y, &w, &h,
                          &r, &g, &b, &a))
        return NULL;
        
    if (r > 255) r = 255;
    if (r < 0)   r = 0;
    if (g > 255) g = 255;
    if (g < 0)   g = 0;
    if (b > 255) b = 255;
    if (b < 0)   b = 0;
    if (a > 255) a = 255;
    if (a < 0)   a = 0;
    
    imlib_context_set_drawable(rpm);
    bg = imlib_create_image_from_drawable(0, x, y, w, h, 1);
        
    if (!bg) {
        printf("Failed to create background image in ppshade!\n");
        return Py_BuildValue("i", 0);
    }
    
    imlib_context_set_image(bg);    
    sprintf(filter,"tint(x=0,y=0,w=%d,h=%d,red=%d,green=%d,blue=%d,alpha=%d);",
            w,h,r,g,b,a);
    imlib_apply_filter(filter);
    imlib_render_pixmaps_for_whole_image(&bgpm, &mask);
    XSetWindowBackgroundPixmap(dsp, panel, bgpm);
    imlib_free_pixmap_and_mask(bgpm); 
    imlib_free_image();
    return Py_BuildValue("i", 1);
}

/*--------------------------------------------------------*/
static PyObject * _ppinit(PyObject *self, PyObject *args) { 
/*--------------------------------------------------------*/
#ifdef IMLIB2_FIX
    void *handle;
#endif
    Window panel;
    XGCValues gcv;
    char *font;
    
    
    XSetErrorHandler(pperror);
    gcv.graphics_exposures = False;
    dsp = XOpenDisplay(NULL);
    scr = DefaultScreen(dsp);
    
    if (!PyArg_ParseTuple(args, "ls", &panel, &font))
        return NULL;
    
    imlib_context_set_display(dsp);
    imlib_context_set_visual(DefaultVisual(dsp, scr));
    imlib_context_set_colormap(DefaultColormap(dsp, scr));
    imlib_context_set_dither(1);
    
#ifdef IMLIB2_FIX
    /* Kludge to get around a problem where dlopen fails to open several
       Imlib2 (versions 1.2 and greater) image loaders, namely png and jpeg,
       because of undefined symbols.  This problem is somehow related to
       calling the Imlib2 code from this Python extension module.  As a
       workaround, I go ahead and open the shared libs here with the RTLD_LAZY
       flag, avoiding the undefined symbols.  This stops the Imlib2 code from
       attempting to open them later with the RTLD_NOW flag which fails.  If
       you're reading this and know of a proper solution, please let me know ..
    */
    handle = dlopen("/usr/lib/libImlib2.so.1", RTLD_NOW|RTLD_GLOBAL);

    if (!handle) {
        printf("Imlib2 dlopen failed: %s\n", dlerror());
    }
#endif
    
#ifdef HAVE_XFT
    if (font[0] == '-')
        xf = XftFontOpenXlfd(dsp, scr, font);
    else
        xf = XftFontOpenName(dsp, scr, font);
    
    visual = DefaultVisual(dsp, scr);
    cmap   = DefaultColormap(dsp, scr);
    draw   = XftDrawCreate(dsp, panel, visual, cmap);
    gc     = XCreateGC(dsp, RootWindow(dsp, scr), GCGraphicsExposures, &gcv);
#else
    xf = XLoadQueryFont(dsp, font);
    if (!xf)
        xf = XLoadQueryFont(dsp, "fixed");
    gcv.font = xf->fid;
    gc = XCreateGC(dsp, RootWindow(dsp, scr), GCFont|GCGraphicsExposures, &gcv);
#endif
    return Py_BuildValue("i", 1);
}

/*-------------------------------*/
static PyMethodDef PPMethods[] = {
/*-------------------------------*/
    {"ppclear",    _ppclear,    METH_VARARGS, "Clear Area"},
    {"ppfont",     _ppfont,     METH_VARARGS, "Font Rendering"},
    {"ppfontsize", _ppfontsize, METH_VARARGS, "Return Size of Given Font"},
    {"ppicon",     _ppicon,     METH_VARARGS, "Icon Rendering"},
    {"ppshade",    _ppshade,    METH_VARARGS, "Background Rendering"},
    {"ppinit",     _ppinit,     METH_VARARGS, "Initialization"},
    {NULL, NULL, 0, NULL}
};

/*----------------------*/
void initppmodule(void) {
/*----------------------*/    
    Py_InitModule("ppmodule", PPMethods);
}
