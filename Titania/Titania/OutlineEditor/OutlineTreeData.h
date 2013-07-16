/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstraße 31a, Leipzig, Germany 2011.
 *
 * All rights reserved. Holger Seelig <holger.seelig@yahoo.de>.
 *
 * THIS IS UNPUBLISHED SOURCE CODE OF create3000.
 *
 * The copyright notice above does not evidence any actual of intended
 * publication of such source code, and is an unpublished work by create3000.
 * This material contains CONFIDENTIAL INFORMATION that is the property of
 * create3000.
 *
 * No permission is granted to copy, distribute, or create derivative works from
 * the contents of this software, in whole or in part, without the prior written
 * permission of create3000.
 *
 * NON-MILITARY USE ONLY
 *
 * All create3000 software are effectively free software with a non-military use
 * restriction. It is free. Well commented source is provided. You may reuse the
 * source in any way you please with the exception anything that uses it must be
 * marked to indicate is contains 'non-military use only' components.
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 1999, 2012 Holger Seelig <holger.seelig@yahoo.de>.
 *
 * This file is part of the Titania Project.
 *
 * Titania is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 3 only, as published by the
 * Free Software Foundation.
 *
 * Titania is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License version 3 for more
 * details (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version 3
 * along with Titania.  If not, see <http://www.gnu.org/licenses/gpl.html> for a
 * copy of the GPLv3 License.
 *
 * For Silvio, Joy and Adi.
 *
 ******************************************************************************/

#ifndef __TITANIA_OUTLINE_EDITOR_OUTLINE_TREE_DATA_H__
#define __TITANIA_OUTLINE_EDITOR_OUTLINE_TREE_DATA_H__

#include <Titania/X3D.h>
#include <deque>

namespace titania {
namespace puck {

enum class OutlineIterType
{
	X3DFieldValue,
	X3DField,
	X3DBaseNode

};

class X3DOutlineTreeDataParent :
	public Glib::Object
{
public:

	X3D::X3DChildObject*
	get_object () const
	{ return object; }

	OutlineIterType
	get_type () const
	{ return type; }

	size_t
	get_index () const
	{ return index; }

	~X3DOutlineTreeDataParent ()
	{
		if (type == OutlineIterType::X3DBaseNode)
			delete object;
	}

protected:

	X3DOutlineTreeDataParent (OutlineIterType type, X3D::X3DChildObject* object, size_t index) :
		Glib::Object (),
		object (object),
		type (type),
		index (index)
	{
		if (type == OutlineIterType::X3DBaseNode)
			this -> object = new X3D::SFNode (static_cast <X3D::SFNode*> (object) -> getValue ());
	}

	X3DOutlineTreeDataParent (const X3DOutlineTreeDataParent & value) :
		X3DOutlineTreeDataParent (value .type, value .object, value .index)
	{ }

	X3DOutlineTreeDataParent (X3DOutlineTreeDataParent &&) = delete;

	X3DOutlineTreeDataParent &
	operator = (const X3DOutlineTreeDataParent &) = delete;


private:

	X3D::X3DChildObject*  object;
	const OutlineIterType type;
	const size_t          index;

};

class OutlineTreeDataParent :
	public X3DOutlineTreeDataParent
{
public:

	OutlineTreeDataParent (OutlineIterType type, X3D::X3DChildObject* object, size_t index) :
		X3DOutlineTreeDataParent (type, object, index)
	{ }

	OutlineTreeDataParent (const OutlineTreeDataParent & value) :
		X3DOutlineTreeDataParent (value)
	{ }

};

class OutlineTreeData :
	public OutlineTreeDataParent
{
public:

	typedef std::deque <OutlineTreeDataParent> parents_type;

	OutlineTreeData (OutlineIterType type, X3D::X3DChildObject* object, int index, const parents_type & parents) :
		OutlineTreeDataParent (type, object, index),
		parents (parents)
	{ }

	const parents_type &
	get_parents () const
	{ return parents; }


private:

	const parents_type parents;

};

} // puck
} // titania

#endif
