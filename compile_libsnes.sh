#!/bin/sh
FORMAT=libsnes
START_DIR=`pwd`

#******************
# PROGRAM FUNCTIONS
#******************

function clean()
{
	make -f Makefile.libsnes clean
}

function clean_libxenon()
{
	make -f Makefile.libsnes platform=xenon clean
}

function clean_ps3()
{
	make -f Makefile.libsnes platform=ps3 clean
}

function make_libsnes()
{
	make -f Makefile.libsnes
}

function make_libsnes_ntsc()
{
	make -f Makefile.libsnes NTSC=1
}

function make_libsnes_debug()
{
	make -f Makefile.libsnes DEBUG=1
}

function make_libsnes_ps3()
{
	make -f Makefile.libsnes platform=ps3
}

function make_libsnes_libxenon()
{
	make -f Makefile.libsnes platform=xenon
}

#******************
# DISPLAY FUNCTIONS
#******************

function title()
{
	echo ""
	echo "***************************"
	echo "COMPILER SCRIPT FOR $FORMAT"
	echo "***************************"
}

function display_clean()
{
	echo "clean          Clean the object files"
}

function display_clean_libxenon()
{
	echo "clean_libxenon Clean the object files (for libxenon)"
}

function display_clean_ps3()
{
	echo "clean_ps3      Clean the object files (for PS3)"
}

function display_make()
{
	echo "make           Compile libsnes library"
}

function display_make_libxenon()
{
	echo "make_libxenon  Compile libsnes library (for libxenon)"
}

function display_make_ps3()
{
	echo "make_ps3       Compile libsnes library (for PS3)"
}

function display_make_ntsc()
{
	echo "make_ntsc      Compile libsnes library (with NTSC filter)"
}

function display_make_debug()
{
	echo "make_debug     Compile DEBUG libsnes library "
}

function display_all_options()
{
	echo ""
	display_clean
	display_clean_libxenon
	display_clean_ps3
	echo ""
	display_make
	display_make_libxenon
	display_make_ps3
	display_make_ntsc
	display_make_debug
}

function display_usage()
{
	echo "Usage: compile_libsnes.sh [options]"
	echo "Options:"
	display_all_options
}

#***********************
# MAIN CONTROL FLOW LOOP
#***********************

title

if [ ! -n "$1" ]; then
	display_usage
else
	for i in "$@"
	do
		if [ "$i" = "help" ]; then
			display_usage
		fi
		if [ "$i" = "clean" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_clean
			echo "*************************************"
			clean
		fi
		if [ "$i" = "clean_ps3" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_clean_ps3
			echo "*************************************"
			clean_ps3
		fi
		if [ "$i" = "clean_libxenon" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_clean_libxenon
			echo "*************************************"
			clean_libxenon
		fi
		if [ "$i" = "make" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make
			echo "*************************************"
			make_libsnes
		fi
		if [ "$i" = "make_libxenon" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_libxenon
			echo "*************************************"
			make_libsnes_libxenon
		fi
		if [ "$i" = "make_ps3" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_ps3
			echo "*************************************"
			make_libsnes_ps3
		fi
		if [ "$i" = "make_ntsc" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_ntsc
			echo "*************************************"
			make_libsnes_ntsc
		fi
		if [ "$i" = "make_debug" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_debug
			echo "*************************************"
			make_libsnes_debug
		fi
	done
fi
