# ---------------------------------------------
#           This file is part of
#      _  _   __    _   _    __    __
#     ( \/ ) /__\  ( )_( )  /__\  (  )
#      \  / /(__)\  ) _ (  /(__)\  )(__
#      (__)(__)(__)(_) (_)(__)(__)(____)
#
#     Yet Another HW Abstraction Library
#      Copyright (C) Andreas Terstegge
#      BSD Licensed (see file LICENSE)
#
# ---------------------------------------------

# platform and YAHAL location
PLATFORM  = msp432
YAHAL_DIR = /home/thorben/dev/yahal

# name of final target
TARGET = msp432.out

# folder for build artifacts
# (same as eclipse build folder)
BUILD_DIR = Debug

# all source folders of our project
SRC_DIRS = .

# include dirs
INC_DIRS += $(YAHAL_DIR)/src/platform/$(PLATFORM)
INC_DIRS += $(YAHAL_DIR)/include/interface
INC_DIRS += $(YAHAL_DIR)/src/driver
INC_DIRS += $(YAHAL_DIR)/src/util
INC_DIRS += $(YAHAL_DIR)/src/uGUI
INC_DIRS += $(YAHAL_DIR)/src/uGUI/fonts

# linked in libraries
LINK_LIBS = $(YAHAL_DIR)/libYAHAL_msp432.a 

include $(YAHAL_DIR)/makefiles/common.mk

