#!/bin/sh
FORMAT=libretro
START_DIR=`pwd`

#******************
# PROGRAM FUNCTIONS
#******************

clean()
{
	make -f Makefile.libretro clean
}

clean_libxenon()
{
	make -f Makefile.libretro platform=xenon clean
}

clean_ps3()
{
	make -f Makefile.libretro platform=ps3 clean
}

make_libretro()
{
	make -f Makefile.libretro
}

make_libretro_ntsc()
{
	make -f Makefile.libretro NTSC=1
}

make_libretro_debug()
{
	make -f Makefile.libretro DEBUG=1
}

make_libretro_ps3()
{
	make -f Makefile.libretro platform=ps3
}

make_libretro_libxenon()
{
	make -f Makefile.libretro platform=xenon
}

#******************
# DISPLAY FUNCTIONS
#******************

title()
{
	echo ""
	echo "***************************"
	echo "COMPILER SCRIPT FOR $FORMAT"
	echo "***************************"
}

display_clean()
{
	echo "clean          Clean the object files"
}

display_clean_libxenon()
{
	echo "clean_libxenon Clean the object files (for libxenon)"
}

display_clean_ps3()
{
	echo "clean_ps3      Clean the object files (for PS3)"
}

display_make()
{
	echo "make           Compile libretro library"
}

display_make_libxenon()
{
	echo "make_libxenon  Compile libretro library (for libxenon)"
}

display_make_ps3()
{
	echo "make_ps3       Compile libretro library (for PS3)"
}

display_make_ntsc()
{
	echo "make_ntsc      Compile libretro library (with NTSC filter)"
}

display_make_debug()
{
	echo "make_debug     Compile DEBUG libretro library "
}

display_all_options()
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

display_usage()
{
	echo "Usage: compile_libretro.sh [options]"
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
			make_libretro
		fi
		if [ "$i" = "make_libxenon" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_libxenon
			echo "*************************************"
			make_libretro_libxenon
		fi
		if [ "$i" = "make_ps3" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_ps3
			echo "*************************************"
			make_libretro_ps3
		fi
		if [ "$i" = "make_ntsc" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_ntsc
			echo "*************************************"
			make_libretro_ntsc
		fi
		if [ "$i" = "make_debug" ]; then
			echo ""
			echo "*************************************"
			echo "DOING:"
			display_make_debug
			echo "*************************************"
			make_libretro_debug
		fi
	done
fi
