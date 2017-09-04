/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * Licensed under the GNU General Public License Version 2
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __CE_PAGE_H
#define __CE_PAGE_H

#include <glib-object.h>

#include <NetworkManager.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CE_TYPE_PAGE (ce_page_get_type ())
G_DECLARE_DERIVABLE_TYPE (CEPage, ce_page, CE, PAGE, GObject)

struct _CEPageClass
{
        GObjectClass parent_class;

        gboolean (*validate) (CEPage *page, NMConnection *connection, GError **error);
        void (*changed)     (CEPage *page);
        void (*initialized) (CEPage *page, GError *error);
};

GtkBuilder  *ce_page_get_builder     (CEPage           *page);
NMConnection *ce_page_get_connection (CEPage           *page);
NMClient    *ce_page_get_client      (CEPage           *page);
GtkWidget   *ce_page_get_page        (CEPage           *page);
const gchar *ce_page_get_title       (CEPage           *page);
void         ce_page_set_security_setting (CEPage           *page,
                                           const gchar      *security_setting);
const gchar *ce_page_get_security_setting (CEPage           *page);
gboolean     ce_page_validate        (CEPage           *page,
                                      NMConnection     *connection,
                                      GError          **error);
gboolean     ce_page_get_initialized (CEPage           *page);
void         ce_page_changed         (CEPage           *page);
CEPage      *ce_page_new             (GType             type,
                                      NMConnection     *connection,
                                      NMClient         *client,
                                      const gchar      *ui_resource,
                                      const gchar      *title);
void         ce_page_complete_init   (CEPage           *page,
                                      const gchar      *setting_name,
                                      GVariant         *variant,
                                      GError           *error);

gchar      **ce_page_get_mac_list    (NMClient         *client,
                                      GType             device_type,
                                      const gchar      *mac_property);
void         ce_page_setup_mac_combo (GtkComboBoxText  *combo,
                                      const gchar      *current_mac,
                                      gchar           **mac_list);
gint         ce_get_property_default (NMSetting        *setting,
                                      const gchar      *property_name);
gint         ce_spin_output_with_default (GtkSpinButton *spin,
                                          gpointer       user_data);
gboolean     ce_page_address_is_valid (const gchar *addr);
gchar       *ce_page_trim_address (const gchar *addr);

typedef enum {
        NAME_FORMAT_TYPE,
        NAME_FORMAT_PROFILE
} NameFormat;

gchar * ce_page_get_next_available_name (const GPtrArray *connections,
                                         NameFormat format,
                                         const gchar *type_name);



G_END_DECLS

#endif /* __CE_PAGE_H */

