bin_PROGRAMS=wiui
wiui_SOURCES=checklist.c inputbox.c  main.c  Makefile.am  menubox.c  textbox.c  util.c  yesno.c 
wiui_LDADD=$(top_srcdir)/plugins/libwiui.a    -lncurses
wiui_LDFLAGS=-D_GNU_SOURCE -DKBUILD_NO_NLS
DEFS+=-D_GNU_SOURCE
