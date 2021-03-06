#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=akilcd
PKG_RELEASE:=1
PKG_VERSION:=1.1

include $(INCLUDE_DIR)/package.mk

define Package/akilcd
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Akizuki Graphic LCD
endef

define Package/akilcd/description
	Tool for controlling Akizuki Graphic LCD
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) CFLAGS="$(TARGET_CFLAGS) -I$(LINUX_DIR)/include"
endef

define Package/akilcd/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/akilcd $(1)/usr/bin/
endef

$(eval $(call BuildPackage,akilcd))
