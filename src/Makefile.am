bin_PROGRAMS=wiui wictl
wiui_SOURCES=inputbox.c  main.c  Makefile.am  menubox.c  textbox.c  util.c  yesno.c 
wiui_LDADD=$(top_srcdir)/plugins/libwiui.a    -lncurses -lpthread
wiui_LDFLAGS=-D_GNU_SOURCE -DKBUILD_NO_NLS

wictl_SOURCES=wictl.c 
wictl_LDADD=$(top_srcdir)/plugins/libwiui.a  
wictl_LDFLAGS=-D_GNU_SOURCE -DKBUILD_NO_NLS
DEFS+=-D_GNU_SOURCE
