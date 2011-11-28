#!/bin/sh
FORMAT=WII
START_DIR=`pwd`
BUILD_DIR_EXECUTABLES=`pwd`/wii/pkg

#******************
# PROGRAM FUNCTIONS
#******************

function clean()
{
	make -f Makefile.wii clean
}

function make_wii()
{
	make -f Makefile.wii
}

function make_wii_pkg()
{
	make -f Makefile.wii
	if [ -f "$START_DIR/boot.dol" ] ; then
		rm $START_DIR/boot.dol
	fi
}

#******************
# DISPLAY FUNCTIONS
#******************

function title()
{
	echo ""
	echo "***********************"
	echo "COMPILER SCRIPT FOR $FORMAT"
	echo "***********************"
}

function display_clean()
{
	echo "clean          Clean the object files"
}

function display_make()
{
	echo "make           Compile Wii version"
}

function display_pkg()
{
	echo "pkg            Create Wii executable dir (for HBC)"
}

function display_all_options()
{
	display_clean
	display_make
	display_pkg
}

#***********************
# MAIN CONTROL FLOW LOOP
#***********************

title
if [ ! -n "$1" ]; then
	echo "Usage: compile_wii.sh [options]"
	echo "Options:"
	display_all_options
else
	for i in "$@"
	do
		if [ "$i" = "clean" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_clean
			echo "*************************************"
			clean
		fi
		if [ "$i" = "make" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make
			echo "*************************************"
			make_wii
		fi
		if [ "$i" = "pkg" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_pkg
			echo "*************************************"
			make_wii_pkg
		fi
	done
fi
