TYPE=exe

TARGET = test

SRC_DIR=src
INC_DIR=include

SRCS = \
../nugget/common/syscalls/printf.s \
../nugget/common/crt0/crt0.s 			\
$(SRC_DIR)/main.c                 \
$(SRC_DIR)/global.c               \
$(SRC_DIR)/memory/lstack.c        \
$(SRC_DIR)/memory/arena.c         \
$(SRC_DIR)/archive/arc.c          \
$(SRC_DIR)/collision/collision.c  \
$(SRC_DIR)/input/input.c          \
$(SRC_DIR)/sound/sound.c          \
$(SRC_DIR)/texture/texture.c      \
$(SRC_DIR)/actor/actor.c          \
$(SRC_DIR)/actor/a_player.c       \
$(SRC_DIR)/module/gameplay.c      \
$(SRC_DIR)/scene/scene.c          \
$(SRC_DIR)/model/clip.c           \
$(SRC_DIR)/model/agm.c            \
$(SRC_DIR)/model/sgm.c            \
$(SRC_DIR)/model/sgm2.c           \
$(SRC_DIR)/math/math.c            \
$(SRC_DIR)/camera/camera.c        \
$(SRC_DIR)/actor/a_obj_syokudai.c \
$(SRC_DIR)/actor/a_obj_tsubo.c    \
$(SRC_DIR)/actor/a_obj_grass.c    \
$(SRC_DIR)/actor/a_obj_grass_cut.c\
$(SRC_DIR)/particles/particles.c  \
$(SRC_DIR)/fpmath.S							  \
$(SRC_DIR)/lz77.S                 \
$(SRC_DIR)/asmdata.S                      

CPPFLAGS += -flto -Wno-deprecated \
-I../psyq/include 				  \
-I../nugget/psyq/include 		  \
-I$(INC_DIR)                  	  \

LDFLAGS += -L../psyq/lib -flto -Wno-deprecated
LDFLAGS += -Wl,--start-group
LDFLAGS += -lapi
LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -lcd
LDFLAGS += -letc
LDFLAGS += -lgpu
LDFLAGS += -lgs
LDFLAGS += -lgte
LDFLAGS += -lgun
LDFLAGS += -lhmd
LDFLAGS += -lmath
LDFLAGS += -lmcrd
LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
LDFLAGS += -ltap
LDFLAGS += -Wl,--end-group

LDSCRIPT ?= $(ROOTDIR)/ps-$(TYPE).ld
include ../nugget/common.mk