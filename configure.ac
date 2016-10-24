#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(wiui, 0.0.1, baozhu.zuo@foxmail.com)
AM_INIT_AUTOMAKE([-Wall foreign])
# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([ncurses])
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h locale.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset setlocale strcasecmp strchr strncasecmp strpbrk strspn])

AC_CONFIG_FILES([Makefile
                 plugins/Makefile
                 src/Makefile])
AC_OUTPUT
