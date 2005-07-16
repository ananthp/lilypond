/*
  axis-group-interface.cc -- implement Axis_group_interface

  source file of the GNU LilyPond music typesetter

  (c) 2000--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/

#include "axis-group-interface.hh"

#include "grob.hh"
#include "hara-kiri-group-spanner.hh"
#include "warn.hh"

void
Axis_group_interface::add_element (Grob *me, Grob *e)
{
  SCM axes = me->get_property ("axes");
  if (!scm_is_pair (axes))
    programming_error ("axes should be nonempty");
  
  for (SCM ax = axes; ax != SCM_EOL; ax = scm_cdr (ax))
    {
      Axis a = (Axis) scm_to_int (scm_car (ax));

      if (!e->get_parent (a))
	e->set_parent (me, a);

      e->internal_set_object ((a == X_AXIS)
			      ? ly_symbol2scm ("axis-group-parent-X")
				: ly_symbol2scm ("axis-group-parent-Y"),
			      me->self_scm ());
    }

  Pointer_group_interface::add_grob (me, ly_symbol2scm ("elements"), e);
  me->add_dependency (e);
}

bool
Axis_group_interface::has_axis (Grob *me, Axis a)
{
  /*
    urg. FIXME, check for Hara_kiri_group_spanner shouldn't be necessary?
  */
  return me->has_extent_callback (group_extent_callback_proc, a)
    || (me->has_extent_callback (Hara_kiri_group_spanner::y_extent_proc, a));
}

Interval
Axis_group_interface::relative_group_extent (Link_array<Grob> const &elts,
					     Grob *common, Axis a)
{
  Interval r;
  for (int i = 0; i < elts.size(); i++)
    {
      Grob *se = elts[i];
      Interval dims = se->extent (common, a);
      if (!dims.is_empty ())
	r.unite (dims);
    }
  return r;
}



MAKE_SCHEME_CALLBACK (Axis_group_interface, group_extent_callback, 2);
SCM
Axis_group_interface::group_extent_callback (SCM element_smob, SCM scm_axis)
{
  Grob *me = unsmob_grob (element_smob);
  Axis a = (Axis) scm_to_int (scm_axis);

  extract_grob_set (me, "elements", elts);
  Grob *common = common_refpoint_of_array (elts, me, a);

  Real my_coord = me->relative_coordinate (common, a);
  Interval r (relative_group_extent (elts, common, a));

  return ly_interval2scm (r - my_coord);
}

void
Axis_group_interface::set_axes (Grob *me, Axis a1, Axis a2)
{
  SCM sa1 = scm_int2num (a1);
  SCM sa2 = scm_int2num (a2);

  SCM axes = me->get_property ("axes");

  if (!scm_is_pair (axes)
      || scm_c_memq (sa1, axes) == SCM_BOOL_F
      || scm_c_memq (sa2, axes) == SCM_BOOL_F)
    {
      SCM ax = scm_cons (sa1, SCM_EOL);
      if (a1 != a2)
	ax = scm_cons (sa2, ax);
      me->set_property ("axes", ax);
    }

  if (a1 != X_AXIS && a2 != X_AXIS)
    me->set_extent (SCM_EOL, X_AXIS);
  if (a1 != Y_AXIS && a2 != Y_AXIS)
    me->set_extent (SCM_EOL, Y_AXIS);

  /*
    why so convoluted ? (fixme/documentme?)
  */
  if (me->has_extent_callback (Grob::stencil_extent_proc, a1))
    me->set_extent_callback (Axis_group_interface::group_extent_callback_proc, a1);
  if (me->has_extent_callback (Grob::stencil_extent_proc, a2))
    me->set_extent_callback (Axis_group_interface::group_extent_callback_proc, a2);
}

void
Axis_group_interface::get_children (Grob *me, Link_array<Grob> *found)
{
  found->push (me);

  if (!has_interface (me))
    return;

  extract_grob_set (me, "elements", elements);
  for (int i = 0; i < elements.size (); i++)
    {
      Grob *e = elements[i];
      Axis_group_interface::get_children (e, found);
    }
}

ADD_INTERFACE (Axis_group_interface, "axis-group-interface",
	       "An object that groups other layout objects.",
	       "axes elements");
