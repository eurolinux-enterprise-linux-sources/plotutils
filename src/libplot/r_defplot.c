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

/* This file defines the initialization for any ReGISPlotter object,
   including both private data and public methods.  There is a one-to-one
   correspondence between public methods and user-callable functions in the
   C API. */

/* Note that ReGIS displays are rectangular, and wider than they are high.
   In terms of integer ReGIS coordinates a ReGIS display is a
   [0..767]x[0..479] rectangle, and we choose our viewport to be the square
   [144..623]x[0..479].  I.e. we define it to be a square, occupying the
   entire height of the display, and centered on the display. */

#include "sys-defines.h"
#include "extern.h"

#ifndef LIBPLOTTER
/* In libplot, this is the initialization for the function-pointer part of
   a ReGISPlotter struct. */
const Plotter _pl_r_default_plotter = 
{
  /* initialization (after creation) and termination (before deletion) */
  _pl_r_initialize, _pl_r_terminate,
  /* page manipulation */
  _pl_r_begin_page, _pl_r_erase_page, _pl_r_end_page,
  /* drawing state manipulation */
  _pl_g_push_state, _pl_g_pop_state,
  /* internal path-painting methods (endpath() is a wrapper for the first) */
  _pl_r_paint_path, _pl_r_paint_paths, _pl_r_path_is_flushable, _pl_r_maybe_prepaint_segments,
  /* internal methods for drawing of markers and points */
  _pl_g_paint_marker, _pl_r_paint_point,
  /* internal methods that plot strings in Hershey, non-Hershey fonts */
  _pl_g_paint_text_string_with_escapes, _pl_g_paint_text_string,
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
_pl_r_initialize (S___(Plotter *_plotter))
{
#ifndef LIBPLOTTER
  /* in libplot, manually invoke superclass initialization method */
  _pl_g_initialize (S___(_plotter));
#endif

  /* override superclass initializations, as necessary */

#ifndef LIBPLOTTER
  /* tag field, differs in derived classes */
  _plotter->data->type = PL_REGIS;
#endif

  /* output model */
  _plotter->data->output_model =  PL_OUTPUT_VIA_CUSTOM_ROUTINES_IN_REAL_TIME;
  
  /* user-queryable capabilities: 0/1/2 = no/yes/maybe */
  _plotter->data->have_wide_lines = 0;
  _plotter->data->have_dash_array = 0;
  _plotter->data->have_solid_fill = 1;
  _plotter->data->have_odd_winding_fill = 1;
  _plotter->data->have_nonzero_winding_fill = 0;
  _plotter->data->have_settable_bg = 1;
  _plotter->data->have_escaped_string_support = 0;
  _plotter->data->have_ps_fonts = 0;
  _plotter->data->have_pcl_fonts = 0;
  _plotter->data->have_stick_fonts = 0;
  _plotter->data->have_extra_stick_fonts = 0;
  _plotter->data->have_other_fonts = 0;

  /* text and font-related parameters (internal, not queryable by user);
     note that we don't set kern_stick_fonts, because it was set by the
     superclass initialization (and it's irrelevant for this Plotter type,
     anyway) */
  _plotter->data->default_font_type = PL_F_HERSHEY;
  _plotter->data->pcl_before_ps = false;
  _plotter->data->have_horizontal_justification = false;
  _plotter->data->have_vertical_justification = false;
  _plotter->data->issue_font_warning = true;

  /* path-related parameters (also internal) */
  _plotter->data->max_unfilled_path_length = PL_MAX_UNFILLED_PATH_LENGTH;
  _plotter->data->have_mixed_paths = false;
  _plotter->data->allowed_arc_scaling = AS_NONE;
  _plotter->data->allowed_ellarc_scaling = AS_NONE;  
  _plotter->data->allowed_quad_scaling = AS_NONE;  
  _plotter->data->allowed_cubic_scaling = AS_NONE;  
  _plotter->data->allowed_box_scaling = AS_NONE;
  _plotter->data->allowed_circle_scaling = AS_UNIFORM;
  _plotter->data->allowed_ellipse_scaling = AS_NONE;

  /* dimensions */
  _plotter->data->display_model_type = (int)DISP_MODEL_VIRTUAL;
  _plotter->data->display_coors_type = (int)DISP_DEVICE_COORS_INTEGER_NON_LIBXMI;
  _plotter->data->flipped_y = true;
  _plotter->data->imin = 144;
  _plotter->data->imax = 623;  
  _plotter->data->jmin = 479;
  _plotter->data->jmax = 0;		/* flipped y */
  _plotter->data->xmin = 0.0;
  _plotter->data->xmax = 0.0;  
  _plotter->data->ymin = 0.0;
  _plotter->data->ymax = 0.0;  
  _plotter->data->page_data = (plPageData *)NULL;

  /* compute the NDC to device-frame affine map, set it in Plotter */
  _compute_ndc_to_device_map (_plotter->data);

  /* initialize data members specific to this derived class */
  _plotter->regis_pos.x = 0;	/* dummy */
  _plotter->regis_pos.y = 0;	/* dummy */
  _plotter->regis_position_is_unknown = true;  
  _plotter->regis_line_type = PL_L_SOLID; /* dummy */
  _plotter->regis_line_type_is_unknown = true;  
  _plotter->regis_fgcolor = 0; /* dummy */
  _plotter->regis_bgcolor = 0; /* dummy */
  _plotter->regis_fgcolor_is_unknown = true;
  _plotter->regis_bgcolor_is_unknown = true;

  /* initialize certain data members from device driver parameters */

}

/* The private `terminate' method, which is invoked when a Plotter is
   deleted, provided that it is non-NULL.  It may do such things as write
   to an output stream from internal storage, deallocate storage, etc.
   When this is invoked, _plotter points to the Plotter that is about to be
   deleted. */

void
_pl_r_terminate (S___(Plotter *_plotter))
{
#ifndef LIBPLOTTER
  /* invoke generic method, e.g. to deallocate instance-specific copies
     of class variables */
  _pl_g_terminate (S___(_plotter));
#endif
}

#ifdef LIBPLOTTER
ReGISPlotter::ReGISPlotter (FILE *infile, FILE *outfile, FILE *errfile)
	:Plotter (infile, outfile, errfile)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (FILE *outfile)
	:Plotter (outfile)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (istream& in, ostream& out, ostream& err)
	: Plotter (in, out, err)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (ostream& out)
	: Plotter (out)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter ()
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (FILE *infile, FILE *outfile, FILE *errfile, PlotterParams &parameters)
	:Plotter (infile, outfile, errfile, parameters)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (FILE *outfile, PlotterParams &parameters)
	:Plotter (outfile, parameters)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (istream& in, ostream& out, ostream& err, PlotterParams &parameters)
	: Plotter (in, out, err, parameters)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (ostream& out, PlotterParams &parameters)
	: Plotter (out, parameters)
{
  _pl_r_initialize ();
}

ReGISPlotter::ReGISPlotter (PlotterParams &parameters)
	: Plotter (parameters)
{
  _pl_r_initialize ();
}

ReGISPlotter::~ReGISPlotter ()
{
  /* if luser left the Plotter open, close it */
  if (_plotter->data->open)
    _API_closepl ();

  _pl_r_terminate ();
}
#endif
