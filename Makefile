#
# Copyright 2015 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
TARGETS := subscriber publisher
SOURCES := subscriber.cpp publisher.cpp

#####################################################################
# MAIN APPLICATIONS
#####################################################################
.DEFAULT_GOAL := all
.PHONY: all
all: ${TARGETS}

#####################################################################
# DEPENDENCIES
#####################################################################
ifeq (,$(strip $(filter $(MAKECMDGOALS),clean)))
  DEPENDENCIES := $(patsubst %.c,%.d,$(filter %.c,$(SOURCES))) $(patsubst %.cpp,%.d,$(filter %.cpp,$(SOURCES)))
  ifneq (,$(strip $(DEPENDENCIES)))
    $(DEPENDENCIES): $(PROGRAM_DBUS_GLUE) $(wildcard *.yang)
    -include $(DEPENDENCIES)
  endif
endif

#####################################################################
# CONFIGURATION
#####################################################################

PKG_CFLAGS := $(shell pkg-config --cflags libsystemd libzmq)
PKG_LFLAGS := $(shell pkg-config --libs libsystemd libzmq)
CPS_LFLAGS := -pthread -lpthread -L. -ldn_common -levent_log -lcps-api-common -lcps-class-map-util -lhiredis

CXX      := g++
CPPFLAGS += ${PKG_CFLAGS} -I.
CXXFLAGS += -std=c++11
#CXXFLAGS += -std=c++17 -Wno-register
LDFLAGS  += ${PKG_LFLAGS} ${CPS_LFLAGS}
#LDFLAGS  += -L/usr/local/lib ${PKG_LFLAGS} -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl -L. -lcps-api-common

#####################################################################
# RULES
#####################################################################

# Implicit rules:
# -----------------
%.d : %.cpp
	@printf "%b[1;36m%s%b[0m\n" "\0033" "Dependency: $< -> $@" "\0033"
	$(CC) -MM -MG -MT '$@ $(@:.d=.o)' $(CFLAGS) $(INCLUDES) -o $@ $<
	@printf "\n"

%: %.o
	@printf "%b[1;36m%s%b[0m\n" "\0033" "Linking: $< -> $@" "\0033"
	$(CXX) $^ $(LDFLAGS) -o $@

#YANG_MODPATH=$YANG_MODPATH:/home/mbelanger/work/cps/ngos/workspace/debian/stretch/x86_64/sysroot/usr/local/share/yang/modules/ietf:/home/mbelanger/os10_repos/mgmt-model/features/dell:/home/mbelanger/os10_repos/base-model/common/dell:/home/mbelanger/os10_repos/base-model/rfc:/home/mbelanger/os10_repos/base-model/yang-models:/home/mbelanger/os10_repos/infra-model/yang-models/ PYTHONPATH=$PYTHONPATH:/home/mbelanger/work/cps/ngos/workspace/debian/stretch/x86_64/sysroot/usr/local/lib/python2.7/dist-packages PATH=$PATH:/home/mbelanger/work/cps/ngos/workspace/debian/stretch/x86_64/sysroot/usr/local/bin python yin_parser.py file=/home/mbelanger/os10_repos/mgmt-model/features/dell/dell-vlt.yang cpsheader=/tmp/model/f.h cpssrc=/tmp/model/f.c output=cps history=/tmp/model
.PRECIOUS: %.h
%.c %.h: %.yang
	@mkdir -p ./model
	@printf "%b[1;36m%s%b[0m\n" "\0033" "Compiling model: $< -> $@" "\0033"
	PYTHONPATH=$$PYTHONPATH:/home/mbelanger/work/cps/ngos/workspace/debian/stretch/x86_64/sysroot/usr/local/lib/python2.7/dist-packages:/home/mbelanger/os10_repos/cps-api/scripts/lib/yang_tools/py \
    PATH=$$PATH:/home/mbelanger/work/cps/ngos/workspace/debian/stretch/x86_64/sysroot/usr/local/bin \
    python /home/mbelanger/os10_repos/cps-api/scripts/lib/yang_tools/py/yin_parser.py file=$< cpsheader=$*.h cpssrc=$*.c output=cps history=./model


#####################################################################
# CLEAN
#####################################################################
ifeq (clean,$(MAKECMDGOALS))

YANGS   := $(wildcard *.yang)
RM_LIST := $(wildcard ${YANGS:.yang=.c} ${YANGS:.yang=.h} ${TARGETS} *.d *.o model)
.PHONY: clean
clean:
	@printf "%b[1;36m%s%b[0m\n" "\0033" "Cleaning" "\0033"
ifneq (,$(RM_LIST))
	rm -rf $(RM_LIST)
	@printf "\n"
endif
	@printf "%b[1;32m%s%b[0m\n\n" "\0033" "Done!" "\0033"

endif # ifeq (clean,$(MAKECMDGOALS))

