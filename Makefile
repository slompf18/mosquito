#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := homekit

include $(IDF_PATH)/make/project.mk

toll:
	(cd test; make flash monitor)