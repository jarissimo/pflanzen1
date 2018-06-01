APPLICATION = Pflanzen1

# This defines the node's behavior. It must be one of
# * sensor:		node that collects data fom attached sensors and forwards it
# * collector:  node that collects sensor data, forwards it to the gateway, and
#   		    controls the pump)
ROLE ?= sensor
#TODO error for invalid values
CFLAGS += -DNODE_ROLE=\"$(ROLE)\"

ifeq ($(ROLE), collector)
	NODE_ID ?= ff01
else
	NODE_ID ?= random
endif
ifeq ($(NODE_ID), random)
	CFLAGS += -DNODE_ID_RANDOM
else
	CFLAGS += -DNODE_ID_="(0x$(NODE_ID))"
endif

# RFC 4193. (prefix fd, random bytes 9c..af, subnet id ac01)
IPV6_NETWORK ?= 0xfd9c5921b4afac01
#XXX is this okay? is this safe? is this the best way to do this?
CFLAGS += -DH2O_NETWORK_PREFIX="((uint64_t)$(IPV6_NETWORK))"

# If no BOARD is found in the environment, use this default:
BOARD ?= native

ifeq ($(USE_LIBC_ERRORH), 1)
	CFLAGS += -DUSE_LIBC_ERRORH
endif

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
# saul
USEMODULE += saul_default
# utilities
USEMODULE += checksum

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

include $(RIOTBASE)/Makefile.include
