/* This file is part of the GNU plotutils package.  Copyright (C) 1995,
   1996, 1997, 1998, 1999, 2000, 2005, Free Software Foundation, Inc.

   The GNU plotutils package is free software.  You may redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software foundation; either version 2, or (at your
   option) any later version.

   The GNU plotutils package is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with the GNU plotutils package; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin St., Fifth Floor,
   Boston, MA 02110-1301, USA. */

/* This file defines the initialization for any SVGPlotter object,
   including both private data and public methods.  There is a one-to-one
   correspondence between public methods and user-callable functions in the
   C API. */

#include "sys-defines.h"
#include "extern.h"

#ifndef LIBPLOTTER
/* In libplot, this is the initialization for the function-pointer part of
   a SVGPlotter struct. */
const Plotter _pl_s_default_plotter = 
{
  /* initialization (after creation) and termination (before deletion) */
  _pl_s_initialize, _pl_s_terminate,
  /* page manipulation */
  _pl_s_begin_page, _pl_s_erase_page, _pl_s_end_page,
  /* drawing state manipulation */
  _pl_g_push_state, _pl_g_pop_state,
  /* internal path-painting methods (endpath() is a wrapper for the first) */
  _pl_s_paint_path, _pl_s_paint_paths, _pl_g_path_is_flushable, _pl_g_maybe_prepaint_segments,
  /* internal methods for drawing of markers and points */
  _pl_g_paint_marker, _pl_s_paint_point,
  /* internal methods that plot strings in Hershey, non-Hershey fonts */
  _pl_g_paint_text_string_with_escapes, _pl_s_paint_text_string,
  _pl_g_get_text_width,
  /* private low-level `retrieve font' method */
  _pl_g_retrieve_font,
  /* `flush output' method, called only if Plotter handles its own output */
  _pl_g_flush_output,
  /* error handlers */
  _pl_g_warning,
  _pl_g_error,
};
#endif /* not LIBPLOTTER */

/* The private `initialize' method, which is invoked when a Plotter is
   created.  It is used for such things as initializing capability flags
   from the values of class variables, allocating storage, etc.  When this
   is invoked, _plotter points to the Plotter that has just been
   created. */

void
_pl_s_initialize (S___(Plotter *_plotter))
{
#ifndef LIBPLOTTER
  /* in libplot, manually invoke superclass initialization method */
  _pl_g_initialize (S___(_plotter));
#endif

  /* override generic initializations (which are appropriate to the base
     Plotter class), as necessary */

#ifndef LIBPLOTTER
  /* tag field, differs in derived classes */
  _plotter->data->type = PL_SVG;
#endif

  /* output model */
  _plotter->data->output_model = PL_OUTPUT_ONE_PAGE;
  
  /* user-queryable capabilities: 0/1/2 = no/yes/maybe */
  _plotter->data->have_wide_lines = 1;
  _plotter->data->have_dash_array = 1;
  _plotter->data->have_solid_fill = 1;
  _plotter->data->have_odd_winding_fill = 1;
  _plotter->data->have_nonzero_winding_fill = 1;
  _plotter->data->have_settable_bg = 1;
  _plotter->data->have_escaped_string_support = 0;
  _plotter->data->have_ps_fonts = 1;
  _plotter->data->have_pcl_fonts = 1;
  _plotter->data->have_stick_fonts = 0;
  _plotter->data->have_extra_stick_fonts = 0;
  _plotter->data->have_other_fonts = 0;

  /* text and font-related parameters (internal, not queryable by user);
     note that we don't set kern_stick_fonts, because it was set by the
     superclass initialization (and it's irrelevant for this Plotter type,
     anyway) */
  _plotter->data->default_font_type = PL_F_POSTSCRIPT;
  _plotter->data->pcl_before_ps = false;
  _plotter->data->have_horizontal_justification = true;
  _plotter->data->have_vertical_justification = false;
  _plotter->data->issue_font_warning = true;

  /* path-related parameters (also internal); note that we
     don't set max_unfilled_path_length, because it was set by the
     superclass initialization */
  _plotter->data->have_mixed_paths = false;
  _plotter->data->allowed_arc_scaling = AS_ANY;
  _plotter->data->allowed_ellarc_scaling = AS_ANY;
  _plotter->data->allowed_quad_scaling = AS_ANY;
  _plotter->data->allowed_cubic_scaling = AS_ANY;
  _plotter->data->allowed_box_scaling = AS_ANY;
  _plotter->data->allowed_circle_scaling = AS_ANY;
  _plotter->data->allowed_ellipse_scaling = AS_ANY;

  /* color-related parameters (also internal) */
  _plotter->data->emulate_color = false;
  
  /* dimensions */
  _plotter->data->display_model_type = (int)DISP_MODEL_VIRTUAL;
  _plotter->data->display_coors_type = (int)DISP_DEVICE_COORS_REAL;
  _plotter->data->flipped_y = true;
  _plotter->data->imin = 0;	/* not used */
  _plotter->data->imax = 0;  
  _plotter->data->jmin = 0;
  _plotter->data->jmax = 0;  
  _plotter->data->xmin = 0.0;
  _plotter->data->xmax = 1.0;
  _plotter->data->ymin = 1.0;	/* note flipped y coordinate */
  _plotter->data->ymax = 0.0;
  _plotter->data->page_data = (plPageData *)NULL;

  /* initialize data members specific to this derived class */
  _plotter->s_matrix[0] = 1.0;/* dummy matrix values */
  _plotter->s_matrix[1] = 0.0;
  _plotter->s_matrix[2] = 0.0;
  _plotter->s_matrix[3] = 1.0;
  _plotter->s_matrix[4] = 0.0;
  _plotter->s_matrix[5] = 0.0;
  _plotter->s_matrix_is_unknown = true;
  _plotter->s_matrix_is_bogus = false;
  _plotter->s_bgcolor.red = -1;	/* initialized in s_begin_page */
  _plotter->s_bgcolor.green = -1;
  _plotter->s_bgcolor.blue = -1;
  _plotter->s_bgcolor_suppressed = false;

  /* Note: xmin,xmax,ymin,ymax determine the range of device coordinates
     over which the viewport will extend (and hence the transformation from
     user to device coordinates; see g_space.c).

     For an SVG Plotter, `device coordinates' are usually almost the same
     as libplot's NDC coordinates, on account of the way we wrap a global
     transformation matrix around all graphics in the output file; see
     s_closepl.c.  However, SVG uses a flipped-y convention: ymin,ymax are
     1 and 0 respectively (see above).

     Actually, if the user specifies a negative xsize or ysize as part of
     the PAGESIZE parameter, we perform an additional flipping, so we can
     emit nonnegative width and height attributes for the top-level SVG
     element.  We do this additional flipping right now. */

  /* determine page type, and viewport size and location/offset (the
     viewport size, i.e., (xsize,ysize), will be written out at the head of
     the SVG file, and the location/offset, i.e., (xorigin,yorigin) and
     (xoffset,yoffset), will be ignored) */
  _set_page_type (_plotter->data);

  if (_plotter->data->viewport_xsize < 0.0)
    /* flip map from user to NDC space */
    {
      _plotter->data->xmin = 1.0;
      _plotter->data->xmax = 0.0;
    }
  if (_plotter->data->viewport_ysize < 0.0)
    /* flip map from user to NDC space */
    {
      _plotter->data->ymin = 0.0;
      _plotter->data->ymax = 1.0;
    }

  /* compute the NDC to device-frame affine map, set it in Plotter */
  _compute_ndc_to_device_map (_plotter->data);
}

/* The private `terminate' method, which is invoked when a Plotter is
   deleted.  It may do such things as write to an output stream from
   internal storage, deallocate storage, etc.  When this is invoked,
   _plotter points to the Plotter that is about to be deleted. */

void
_pl_s_terminate (S___(Plotter *_plotter))
{
}

#ifdef LIBPLOTTER
SVGPlotter::SVGPlotter (FILE *infile, FILE *outfile, FILE *errfile)
	:Plotter (infile, outfile, errfile)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (FILE *outfile)
	:Plotter (outfile)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (istream& in, ostream& out, ostream& err)
	: Plotter (in, out, err)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (ostream& out)
	: Plotter (out)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter ()
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (FILE *infile, FILE *outfile, FILE *errfile, PlotterParams &parameters)
	:Plotter (infile, outfile, errfile, parameters)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (FILE *outfile, PlotterParams &parameters)
	:Plotter (outfile, parameters)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (istream& in, ostream& out, ostream& err, PlotterParams &parameters)
	: Plotter (in, out, err, parameters)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (ostream& out, PlotterParams &parameters)
	: Plotter (out, parameters)
{
  _pl_s_initialize ();
}

SVGPlotter::SVGPlotter (PlotterParams &parameters)
	: Plotter (parameters)
{
  _pl_s_initialize ();
}

SVGPlotter::~SVGPlotter ()
{
  _pl_s_terminate ();
}
#endif
