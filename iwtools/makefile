NODEPS=3rd

PROJECT=DKREADER

include ../../makefile.def

.PHONY : 3rd

3rd:
	chmod +w glib\gobject\gmarshal.c	
	chmod +w glib\gobject\gmarshal.c	
	chmod +w glib\gobject\stamp-gmarshal.h
	chmod +w glib\gobject\glib-mkenums
	chmod +w pango\pango\module-defs.h
	chmod +w pango\pango\module-defs-atsui.c
	chmod +w pango\pango\module-defs-fc.c
	chmod +w pango\pango\module-defs-lang.c
	chmod +w pango\pango\module-defs-win32.c
	chmod +w pango\pango\module-defs-x.c
	chmod +w libzip\lib\zipconf.h
	touch libxml2\configure	
	touch libzip\configure	
	touch cairo\aclocal.m4			
	touch cairo\acinclude.m4	
	touch cairo\configure			
	touch pango\aclocal.m4			
	touch pango\acinclude.m4	
	touch pango\configure		
	${MAKE} -f makefile.thirdparty
