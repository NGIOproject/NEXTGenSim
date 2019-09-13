#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Copyright 2007 Francesc Guim Bernat & Barcelona Supercomputing Centre
# Copyright 2015-2019 NEXTGenIO Project [EC H2020 Project ID: 671951] (www.nextgenio.eu)
# Copyright 2019 Daniel Rivas & Barcelona Supercomputing Centre
# 
# This file is part of NEXTGenSim.
# 
# NEXTGenSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# NEXTGenSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEXTGenSim.  If not, see <https://www.gnu.org/licenses/>.


AC_PREREQ([2.69])
AC_INIT([nextgensim], [3.0])
AM_INIT_AUTOMAKE(foreign subdir-objects)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_CPP
AC_PROG_MKDIR_P

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lboost_iostreams':
AC_CHECK_LIB([boost_iostreams], [gzip_header])
# FIXME: Replace `main' with a function in `-lboost_iostreams':
AC_CHECK_LIB([boost_filesystem], [is_directory])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lxml2':
AC_CHECK_LIB([xml2], [main])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_CHECK_FUNCS([floor modf pow sqrt strerror])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
