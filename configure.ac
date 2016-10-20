AC_INIT(wiui,[0.1.0],baozhu.zuo@foxmail.com)
AC_CONFIG_AUX_DIR(plugins)
AC_CONFIG_SRCDIR(src/main.c)


AM_INIT_AUTOMAKE
AX_CODE_COVERAGE

AX_VALGRIND_CHECK
AC_SUBST(VALGRIND_CHECK_RULES)

LT_INIT 
AC_PROG_CC

AC_CHECK_LIB([ncurses]) 

AC_PROG_LIBTOOL
AC_OUTPUT([
  Makefile 
  src/Makefile 
  plugins/Makefile
])


