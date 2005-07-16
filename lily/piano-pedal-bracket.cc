/*
  piano-pedal-bracket.cc -- implement  Piano_pedal_bracket

  source file of the GNU LilyPond music typesetter

  (c) 2003--2005 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "stencil.hh"
#include "spanner.hh"
#include "item.hh"
#include "tuplet-bracket.hh"

struct Piano_pedal_bracket
{
  DECLARE_SCHEME_CALLBACK (print, (SCM));
  static bool has_interface (Grob *);
};

MAKE_SCHEME_CALLBACK (Piano_pedal_bracket, print, 1);
SCM
Piano_pedal_bracket::print (SCM smob)
{
  Spanner *me = dynamic_cast<Spanner *> (unsmob_grob (smob));
  Spanner *orig = dynamic_cast<Spanner *> (me->original_);

  Drul_array<bool> broken (false, false);
  Drul_array<Real> height = robust_scm2drul
    (me->get_property ("edge-height"), Interval (0, 0));
  Drul_array<Real> shorten = robust_scm2drul
    (me->get_property ("shorten-pair"), Interval (0, 0));
  Drul_array<Real> flare = robust_scm2drul
    (me->get_property ("bracket-flare"), Interval (0, 0));

  Grob *common = me->get_bound (LEFT)
    ->common_refpoint (me->get_bound (RIGHT), X_AXIS);
  Grob *textbit = unsmob_grob (me->get_object ("pedal-text"));

  if (textbit)
    common = common->common_refpoint (textbit, X_AXIS);

  Interval span_points (0, 0);
  Direction d = LEFT;
  do
    {
      Item *b = me->get_bound (d);
      broken[d] = b->break_status_dir () != CENTER;
      if (broken[d])
	{
	  if (orig
	      && ((d == RIGHT && me->get_break_index () != orig->broken_intos_.size () - 1)
		  || (d == LEFT && me->get_break_index ())))
	    height[d] = 0.0;
	  else
	    flare[d] = 0.0;
	}

      Interval ext = robust_relative_extent (b, common, X_AXIS);
      span_points[d] = ext [broken[d] ? RIGHT : LEFT];
    }
  while (flip (&d) != LEFT);

  /* For 'Mixed' style pedals, i.e.  a bracket preceded by text:  Ped._____|
     need to shorten by the extent of the text grob
  */
  if (textbit)
    {
      height[LEFT] = 0;

      Real padding = robust_scm2double (me->get_property ("bound-padding"), 0);

      span_points[LEFT] = padding
	+ robust_relative_extent (textbit, common, X_AXIS)[RIGHT];
    }

  Stencil m;
  if (!span_points.is_empty ()
      && span_points.length () > 0.001)
    {
      m = Tuplet_bracket::make_bracket (me, Y_AXIS,
					Offset (span_points.length (), 0),
					height,
					Interval (),
					flare, shorten);
    }
  m.translate_axis (span_points[LEFT]
		    - me->relative_coordinate (common, X_AXIS), X_AXIS);
  return m.smobbed_copy ();
}

ADD_INTERFACE (Piano_pedal_bracket, "piano-pedal-bracket-interface",
	       "The bracket of the piano pedal.  It can be tuned through the regular "
	       "bracket properties.",
	       "bound-padding edge-height shorten-pair bracket-flare pedal-text");
