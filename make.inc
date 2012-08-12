NAME   = identisaurus
CC     = gcc
GCC    = gcc
PLUGIN = -fplugin=$(NAME).so

# Macros I use for debugging
GCC_OBJ_DIR = /home/enferex/docs/edu/go/dev/gcc-obj-4.7.1/
GCC_BIN_DIR = /home/enferex/docs/edu/go/dev/gcc-install-4.7.1/
DBG_CC      = $(GCC_OBJ_DIR)/gcc/cc1
DBG_GCC     = $(GCC_BIN_DIR)/bin/gcc
GCC         = $(GCC_BIN_DIR)/bin/gcc
CC          = $(GCC_BIN_DIR)/bin/g++
PLUGIN=-fplugin=/home/enferex/proj/identisaurus/$(NAME).so
