/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager Connection editor -- Connection editor for NetworkManager
 *
 * Dan Williams <dcbw@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * (C) Copyright 2008 - 2012 Red Hat, Inc.
 */

#ifndef __CE_PAGE_8021X_SECURITY_H
#define __CE_PAGE_8021X_SECURITY_H

#include <NetworkManager.h>
#include "wireless-security.h"

#include <glib.h>
#include <glib-object.h>

#include "ce-page.h"

#define CE_TYPE_PAGE_8021X_SECURITY (ce_page_8021x_security_get_type ())
G_DECLARE_FINAL_TYPE (CEPage8021xSecurity, ce_page_8021x_security, CE, PAGE_8021X_SECURITY, CEPage)

CEPage *ce_page_8021x_security_new (NMConnection     *connection,
                                    NMClient         *client);

#endif  /* __CE_PAGE_8021X_SECURITY_H */
