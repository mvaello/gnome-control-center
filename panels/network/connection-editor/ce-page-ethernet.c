/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2012 Red Hat, Inc
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

#include "config.h"

#include <glib-object.h>
#include <glib/gi18n.h>

#include <nm-utils.h>
#include <nm-device-ethernet.h>

#include <net/if_arp.h>

#include "ce-page-ethernet.h"

G_DEFINE_TYPE (CEPageEthernet, ce_page_ethernet, CE_TYPE_PAGE)

enum {
        PORT_DEFAULT,
        PORT_TP,
        PORT_AUI,
        PORT_BNC,
        PORT_MII
};

enum {
        SPEED_DEFAULT,
        SPEED_10,
        SPEED_100,
        SPEED_1000,
        SPEED_10000
};

static void
all_user_changed (GtkToggleButton *b, CEPageEthernet *page)
{
        gboolean all_users;
        NMSettingConnection *sc;

        sc = nm_connection_get_setting_connection (CE_PAGE (page)->connection);
        all_users = gtk_toggle_button_get_active (b);

        g_object_set (sc, "permissions", NULL, NULL);
        if (!all_users)
                nm_setting_connection_add_permission (sc, "user", g_get_user_name (), NULL);
}

static void
connect_ethernet_page (CEPageEthernet *page)
{
        NMSettingWired *setting = page->setting_wired;
        NMSettingConnection *sc;
        const char *port;
        const char *duplex;
        int port_idx = PORT_DEFAULT;
        int speed_idx;
        int mtu_def;
        char **mac_list;
        const GByteArray *s_mac;
        char *s_mac_str;
        GtkWidget *widget;
        const gchar *name;

        name = nm_setting_connection_get_id (page->setting_connection);
        gtk_entry_set_text (page->name, name);

        /* Port */
        port = nm_setting_wired_get_port (setting);
        if (port) {
                if (!strcmp (port, "tp"))
                        port_idx = PORT_TP;
                else if (!strcmp (port, "aui"))
                        port_idx = PORT_AUI;
                else if (!strcmp (port, "bnc"))
                        port_idx = PORT_BNC;
                else if (!strcmp (port, "mii"))
                        port_idx = PORT_MII;
        }
        gtk_combo_box_set_active (page->port, port_idx);

        /* Speed */
        switch (nm_setting_wired_get_speed (setting)) {
        case 10:
                speed_idx = SPEED_10;
                break;
        case 100:
                speed_idx = SPEED_100;
                break;
        case 1000:
                speed_idx = SPEED_1000;
                break;
        case 10000:
                speed_idx = SPEED_10000;
                break;
        default:
                speed_idx = SPEED_DEFAULT;
                break;
        }
        gtk_combo_box_set_active (page->speed, speed_idx);
        /* Duplex */
        duplex = nm_setting_wired_get_duplex (setting);
        if (duplex && !strcmp (duplex, "half"))
                gtk_toggle_button_set_active (page->duplex, FALSE);
        else
                gtk_toggle_button_set_active (page->duplex, TRUE);

        /* Autonegotiate */
        gtk_toggle_button_set_active (page->autonegotiate,
                                      nm_setting_wired_get_auto_negotiate (setting));

        /* Device MAC address */
        mac_list = ce_page_get_mac_list (CE_PAGE (page)->client, NM_TYPE_DEVICE_ETHERNET,
                                         NM_DEVICE_ETHERNET_PERMANENT_HW_ADDRESS);
        s_mac = nm_setting_wired_get_mac_address (setting);
        s_mac_str = s_mac ? nm_utils_hwaddr_ntoa (s_mac->data, ARPHRD_ETHER) : NULL;
        ce_page_setup_mac_combo (page->device_mac, s_mac_str, mac_list);
        g_free (s_mac_str);
        g_strfreev (mac_list);
        g_signal_connect_swapped (page->device_mac, "changed", G_CALLBACK (ce_page_changed), page);

        /* Cloned MAC address */
        ce_page_mac_to_entry (nm_setting_wired_get_cloned_mac_address (setting),
                              ARPHRD_ETHER, page->cloned_mac);
        g_signal_connect_swapped (page->cloned_mac, "changed", G_CALLBACK (ce_page_changed), page);

        /* MTU */
        mtu_def = ce_get_property_default (NM_SETTING (setting), NM_SETTING_WIRED_MTU);
        g_signal_connect (page->mtu, "output",
                          G_CALLBACK (ce_spin_output_with_default),
                          GINT_TO_POINTER (mtu_def));

        gtk_spin_button_set_value (page->mtu, (gdouble) nm_setting_wired_get_mtu (setting));

        g_signal_connect_swapped (page->name, "changed", G_CALLBACK (ce_page_changed), page);
        g_signal_connect_swapped (page->port, "changed", G_CALLBACK (ce_page_changed), page);
        g_signal_connect_swapped (page->speed, "changed", G_CALLBACK (ce_page_changed), page);
        g_signal_connect_swapped (page->duplex, "toggled", G_CALLBACK (ce_page_changed), page);
        g_signal_connect_swapped (page->autonegotiate, "toggled", G_CALLBACK (ce_page_changed), page);
        g_signal_connect_swapped (page->mtu, "value-changed", G_CALLBACK (ce_page_changed), page);

        /* Hide widgets we don't yet support */
        widget = GTK_WIDGET (gtk_builder_get_object (CE_PAGE (page)->builder, "heading_port"));
        gtk_widget_hide (widget);
        gtk_widget_hide (GTK_WIDGET (page->port));

        widget = GTK_WIDGET (gtk_builder_get_object (CE_PAGE (page)->builder, "heading_speed"));
        gtk_widget_hide (widget);
        gtk_widget_hide (GTK_WIDGET (page->speed));

        widget = GTK_WIDGET (gtk_builder_get_object (CE_PAGE (page)->builder, "check_duplex"));
        gtk_widget_hide (widget);
        widget = GTK_WIDGET (gtk_builder_get_object (CE_PAGE (page)->builder, "check_renegotiate"));
        gtk_widget_hide (widget);

        widget = GTK_WIDGET (gtk_builder_get_object (CE_PAGE (page)->builder,
                                                     "auto_connect_check"));
        sc = nm_connection_get_setting_connection (CE_PAGE (page)->connection);
        g_object_bind_property (sc, "autoconnect",
                                widget, "active",
                                G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
        widget = GTK_WIDGET (gtk_builder_get_object (CE_PAGE (page)->builder,
                                                     "all_user_check"));
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget),
                                      nm_setting_connection_get_num_permissions (sc) == 0);
        g_signal_connect (widget, "toggled",
                          G_CALLBACK (all_user_changed), page);
}

static void
ui_to_setting (CEPageEthernet *page)
{
        const char *port;
        guint32 speed;
        GByteArray *device_mac = NULL;
        GByteArray *cloned_mac = NULL;
        GtkWidget *entry;

        /* Port */
        switch (gtk_combo_box_get_active (page->port)) {
        case PORT_TP:
                port = "tp";
                break;
        case PORT_AUI:
                port = "aui";
                break;
        case PORT_BNC:
                port = "bnc";
                break;
        case PORT_MII:
                port = "mii";
                break;
        default:
                port = NULL;
                break;
        }

        /* Speed */
        switch (gtk_combo_box_get_active (page->speed)) {
        case SPEED_10:
                speed = 10;
                break;
        case SPEED_100:
                speed = 100;
                break;
        case SPEED_1000:
                speed = 1000;
                break;
        case SPEED_10000:
                speed = 10000;
                break;
        default:
                speed = 0;
                break;
        }

        entry = gtk_bin_get_child (GTK_BIN (page->device_mac));
        if (entry)
                device_mac = ce_page_entry_to_mac (GTK_ENTRY (entry), ARPHRD_ETHER, NULL);
        cloned_mac = ce_page_entry_to_mac (page->cloned_mac, ARPHRD_ETHER, NULL);

        g_object_set (page->setting_wired,
                      NM_SETTING_WIRED_MAC_ADDRESS, device_mac,
                      NM_SETTING_WIRED_CLONED_MAC_ADDRESS, cloned_mac,
                      NM_SETTING_WIRED_PORT, port,
                      NM_SETTING_WIRED_SPEED, speed,
                      NM_SETTING_WIRED_DUPLEX, gtk_toggle_button_get_active (page->duplex) ? "full" : "half",
                      NM_SETTING_WIRED_AUTO_NEGOTIATE, gtk_toggle_button_get_active (page->autonegotiate),
                      NM_SETTING_WIRED_MTU, (guint32) gtk_spin_button_get_value_as_int (page->mtu),
                      NULL);

        if (device_mac)
                g_byte_array_free (device_mac, TRUE);
        if (cloned_mac)
                g_byte_array_free (cloned_mac, TRUE);

        g_object_set (page->setting_connection,
                      NM_SETTING_CONNECTION_ID, gtk_entry_get_text (page->name),
                      NULL);
}

static gboolean
validate (CEPage        *page,
          NMConnection  *connection,
          GError       **error)
{
        CEPageEthernet *self = CE_PAGE_ETHERNET (page);
        gboolean invalid = FALSE;
        GByteArray *ignore;
        GtkWidget *entry;

        entry = gtk_bin_get_child (GTK_BIN (self->device_mac));
        if (entry) {
                ignore = ce_page_entry_to_mac (GTK_ENTRY (entry), ARPHRD_ETHER, &invalid);
                if (invalid)
                        return FALSE;
                if (ignore)
                        g_byte_array_free (ignore, TRUE);
        }

        ignore = ce_page_entry_to_mac (self->cloned_mac, ARPHRD_ETHER, &invalid);
        if (invalid)
                return FALSE;
        if (ignore)
                g_byte_array_free (ignore, TRUE);

        ui_to_setting (self);

        return nm_setting_verify (NM_SETTING (self->setting_connection), NULL, error) &&
               nm_setting_verify (NM_SETTING (self->setting_wired), NULL, error);
}

static void
ce_page_ethernet_init (CEPageEthernet *page)
{
}

static void
ce_page_ethernet_class_init (CEPageEthernetClass *class)
{
        CEPageClass *page_class= CE_PAGE_CLASS (class);

        page_class->validate = validate;
}

CEPage *
ce_page_ethernet_new (NMConnection     *connection,
                      NMClient         *client,
                      NMRemoteSettings *settings)
{
        CEPageEthernet *page;

        page = CE_PAGE_ETHERNET (ce_page_new (CE_TYPE_PAGE_ETHERNET,
                                              connection,
                                              client,
                                              settings,
                                              "/org/gnome/control-center/network/ethernet-page.ui",
                                              _("Identity")));

        page->name = GTK_ENTRY (gtk_builder_get_object (CE_PAGE (page)->builder, "entry_name"));
        page->device_mac = GTK_COMBO_BOX_TEXT (gtk_builder_get_object (CE_PAGE (page)->builder, "combo_mac"));
        page->cloned_mac = GTK_ENTRY (gtk_builder_get_object (CE_PAGE (page)->builder, "entry_cloned_mac"));
        page->port = GTK_COMBO_BOX (gtk_builder_get_object (CE_PAGE (page)->builder, "combo_port"));
        page->speed = GTK_COMBO_BOX (gtk_builder_get_object (CE_PAGE (page)->builder, "combo_speed"));
        page->duplex = GTK_TOGGLE_BUTTON (gtk_builder_get_object (CE_PAGE (page)->builder, "check_duplex"));
        page->autonegotiate = GTK_TOGGLE_BUTTON (gtk_builder_get_object (CE_PAGE (page)->builder, "check_renegotiate"));
        page->mtu = GTK_SPIN_BUTTON (gtk_builder_get_object (CE_PAGE (page)->builder, "spin_mtu"));

        page->setting_connection = nm_connection_get_setting_connection (connection);
        page->setting_wired = nm_connection_get_setting_wired (connection);

        connect_ethernet_page (page);

        return CE_PAGE (page);
}