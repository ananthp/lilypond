/*
  notehead.cc -- implement Note_head

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "note-head.hh"

#include <cmath>
#include <cctype>
#include <algorithm> 		//  min, max
using namespace std;

#include "directional-element-interface.hh"
#include "staff-symbol.hh"
#include "misc.hh"
#include "dots.hh"
#include "warn.hh"
#include "font-interface.hh"
#include "music.hh"
#include "rhythmic-head.hh"
#include "staff-symbol-referencer.hh"
#include "lookup.hh"
#include "output-def.hh"

/*
  clean up the mess left by ledger line handling.
*/
static Stencil
internal_print (Grob *me, String *font_char)
{
  SCM style = me->get_property ("style");
  if (!scm_is_symbol (style))
    style = ly_symbol2scm ("default");

  SCM log = scm_from_int (Note_head::get_balltype (me));
  String suffix = to_string (min (robust_scm2int (me->get_property ("duration-log"), 2), 2));
  if (style != ly_symbol2scm ("default"))
    {
      SCM proc = me->get_property ("glyph-name-procedure");
      if (ly_is_procedure (proc))
	suffix = ly_scm2string (scm_call_2 (proc, log, style));
    }
  Font_metric *fm = Font_interface::get_default_font (me);

  String idx = "noteheads.s" + suffix;

  Stencil out = fm->find_by_name (idx);
  if (out.is_empty ())
    {
      String prefix = "noteheads.";
      Grob *stem = unsmob_grob (me->get_object ("stem"));
      Direction stem_dir = stem ? get_grob_direction (stem) : CENTER;

      if (stem_dir == CENTER)
	programming_error ("must have stem dir for note head");

      idx = prefix + ((stem_dir == UP) ? "u" : "d") + suffix;
      out = fm->find_by_name (idx);
    }

  if (out.is_empty ())
    {
      me->warning (_f ("note head `%s' not found", idx.to_str0 ()));
      out = Stencil (Box (Interval (0, 0), Interval (0, 0)), SCM_EOL);
    }
  else
    *font_char = idx;

  return out;
}

/*
  TODO: make stem X-parent of notehead. 
 */
MAKE_SCHEME_CALLBACK (Note_head, stem_x_shift, 2);
SCM
Note_head::stem_x_shift (SCM smob, SCM axis)
{
  (void) axis;
  
  Grob *me = unsmob_grob (smob);
  Grob *stem = unsmob_grob (me->get_object ("stem"));
  if (stem)
    (void) stem->get_property ("positioning-done");

  return scm_from_int (0);
}

MAKE_SCHEME_CALLBACK (Note_head, print, 1);
SCM
Note_head::print (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  String idx;
  return internal_print (me, &idx).smobbed_copy ();
}

Real
Note_head::stem_attachment_coordinate (Grob *me, Axis a)
{
  SCM brewer = me->get_property ("print-function");
  Font_metric *fm = Font_interface::get_default_font (me);

  if (brewer == Note_head::print_proc)
    {
      String key;
      internal_print (me, &key);

      int k = fm->name_to_index (key);
      if (k >= 0)
	{
	  Box b = fm->get_indexed_char (k);
	  Offset wxwy = fm->attachment_point (key);
	  Interval v = b[a];
	  if (!v.is_empty ())
	    return 2 * (wxwy[a] - v.center ()) / v.length ();
	}
    }

  /*
    Fallback
  */
  SCM v = me->get_property ("stem-attachment-function");
  if (!ly_is_procedure (v))
    return 0.0;

  SCM result = scm_call_2 (v, me->self_scm (), scm_from_int (a));
  if (!scm_is_pair (result))
    return 0.0;

  result = (a == X_AXIS) ? scm_car (result) : scm_cdr (result);

  return robust_scm2double (result, 0);
}

int
Note_head::get_balltype (Grob *me)
{
  SCM s = me->get_property ("duration-log");
  return scm_is_number (s) ? min (scm_to_int (s), 2) : 0;
}

ADD_INTERFACE (Note_head, "note-head-interface",
	       "Note head",

	       /* properties */
	       "note-names "
	       "glyph-name-procedure "
	       "accidental-grob "
	       "style "
	       "stem-attachment-function");

