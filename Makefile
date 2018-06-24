APPLICATION = Pflanzen1

# PROJECT SPECIFIC SETTINGS
# =========================

# node properties
# ---------------

# This defines the node's behavior. It must be one of
# * sensor:		node that collects data fom attached sensors and forwards it
# * collector:  node that collects sensor data, forwards it to the gateway, and
#   		    controls the pump)
ROLE ?= sensor
#TODO error for invalid values
CFLAGS += -DNODE_ROLE=\"$(ROLE)\"
CFLAGS += -DNODE_ROLE_"$(shell echo $(ROLE) | tr a-z A-Z)"

# The node's id is used to identify it in data messages, and is used to
# assign it a unique IPv6 address. If not given explicitly, a random ID
# is generated on startup in the range (0001..feff)
# TODO: see if we can generate this deterministically from the node's hardware

# reserved node IDs
COLLECTOR_NODE_ID ?= ff01
GATEWAY_NODE_ID ?= ff99

ifeq ($(ROLE), collector)
	NODE_ID ?= $(COLLECTOR_NODE_ID)
else
	NODE_ID ?= random
endif
ifeq ($(NODE_ID), random)
	CFLAGS += -DNODE_ID_RANDOM
else
	CFLAGS += -DNODE_ID_="(0x$(NODE_ID))"
endif

# if a node cannot reach the collector directly (because of radio coverage
# issues), another node can be used as a relay
ifeq ($(ROLE), sensor)
	# default is to directly send to the collector node.
	UPSTREAM_NODE ?= $(COLLECTOR_NODE_ID)
# in the case of the collector, all data messages are forwarded to the gateway
else ifeq ($(ROLE), collector)
	UPSTREAM_NODE ?= $(GATEWAY_NODE_ID)
endif
ifdef UPSTREAM_NODE
	CFLAGS += -DUPSTREAM_NODE="(0x$(UPSTREAM_NODE))"
endif

# network configuration
# ---------------------

# RFC 4193. (prefix fd, random bytes 9c..af, subnet id ac01)
# network prefix length is hardcoded to 64 (see lib/network.c)
IPV6_NETWORK ?= 0xfd9c5921b4afac01
#XXX is this okay? is this safe? is this the best way to do this?
CFLAGS += -DH2O_NETWORK_PREFIX="((uint64_t)$(IPV6_NETWORK)U)"
# we need a third multicast group for this address (default is 2)
CFLAGS += -DGNRC_NETIF_IPV6_GROUPS_NUMOF=3

# internal settings
# -----------------

ifeq ($(USE_LIBC_ERRORH), 1)
	CFLAGS += -DUSE_LIBC_ERRORH
endif

# see `h2op_add_receive_hook`.
H2OP_RECEIVE_HOOKS_NUMOF ?= 5
CFLAGS += -DH2OP_RECEIVE_HOOKS_NUMOF="$(H2OP_RECEIVE_HOOKS_NUMOF)"

# GENERAL RIOT SETTINGS
# =====================

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/RIOT

# Uncomment this to enable scheduler statistics for ps:
#CFLAGS += -DSCHEDSTATISTICS

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
# net
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += gnrc_sock_udp
USEMODULE += gnrc_rpl
USEMODULE += fib
# USEMODULE += auto_init_gnrc_rpl
# saul
USEMODULE += saul_default
# utilities
USEMODULE += checksum

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

include $(RIOTBASE)/Makefile.include
