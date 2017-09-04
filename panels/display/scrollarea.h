/* Copyright 2006, 2007, 2008, Soren Sandmann <sandmann@daimi.au.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SCROLLAREA_H
#define _SCROLLAREA_H

#include <cairo.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define FOO_TYPE_SCROLL_AREA (foo_scroll_area_get_type ())
G_DECLARE_FINAL_TYPE (FooScrollArea, foo_scroll_area, FOO, SCROLL_AREA, GtkContainer)

typedef enum
  {
    FOO_BUTTON_PRESS,
    FOO_BUTTON_RELEASE,
    FOO_DRAG_HOVER,
    FOO_DROP,
    FOO_MOTION
  } FooScrollAreaEventType;

typedef struct
{
  FooScrollAreaEventType      type;
  int                         x;
  int                         y;
} FooScrollAreaEvent;

typedef void (* FooScrollAreaEventFunc) (FooScrollArea      *area,
                                         FooScrollAreaEvent *event,
                                         gpointer            data);

FooScrollArea *foo_scroll_area_new (void);

/* Set the requisition for the widget. */
void          foo_scroll_area_set_min_size (FooScrollArea *scroll_area,
                                            int            min_width,
                                            int            min_height);

/* Set how much of the canvas can be scrolled into view */
void          foo_scroll_area_set_size (FooScrollArea          *scroll_area,
                                        int                     width,
                                        int                     height);
void          foo_scroll_area_set_size_fixed_y (FooScrollArea  *scroll_area,
                                                int             width,
                                                int             height,
                                                int             old_y,
                                                int             new_y);
void          foo_scroll_area_set_viewport_pos (FooScrollArea  *scroll_area,
                                                int             x,
                                                int             y);
void          foo_scroll_area_get_viewport (FooScrollArea *scroll_area,
                                            GdkRectangle  *viewport);
void          foo_scroll_area_add_input_from_stroke (FooScrollArea           *scroll_area,
                                                     cairo_t                    *cr,
                                                     FooScrollAreaEventFunc   func,
                                                     gpointer                 data);
void          foo_scroll_area_add_input_from_fill (FooScrollArea *scroll_area,
                                                   cairo_t         *cr,
                                                   FooScrollAreaEventFunc func,
                                                   gpointer       data);
void          foo_scroll_area_invalidate_region (FooScrollArea  *area,
                                                 cairo_region_t *region);
void          foo_scroll_area_invalidate (FooScrollArea *scroll_area);
void          foo_scroll_area_invalidate_rect (FooScrollArea *scroll_area,
                                               int            x,
                                               int            y,
                                               int            width,
                                               int            height);
void foo_scroll_area_begin_grab (FooScrollArea *scroll_area,
                                 FooScrollAreaEventFunc func,
                                 gpointer       input_data);
void foo_scroll_area_end_grab (FooScrollArea *scroll_area,
                               FooScrollAreaEvent *event);
gboolean foo_scroll_area_is_grabbed (FooScrollArea *scroll_area);

void foo_scroll_area_begin_auto_scroll (FooScrollArea *scroll_area);
void foo_scroll_area_auto_scroll (FooScrollArea *scroll_area,
                                  FooScrollAreaEvent *event);
void foo_scroll_area_end_auto_scroll (FooScrollArea *scroll_area);

G_END_DECLS

#endif /* _SCROLLAREA_H */
