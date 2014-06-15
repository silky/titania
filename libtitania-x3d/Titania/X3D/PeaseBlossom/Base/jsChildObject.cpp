// /* -*- Mode: C++; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*- */
// /*************************************************************************
//  *
//  * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
//  *
//  * Copyright 1999, 2012 Holger Seelig <holger.seelig@yahoo.de>.
//  *
//  * Titania - a multi-platform office productivity suite
//  *
//  * This file is part of the Titania Project.
//  *
//  * Titania is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU Lesser General Public License version 3
//  * only, as published by the Free Software Foundation.
//  *
//  * Titania is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU Lesser General Public License version 3 for more details
//  * (a copy is included in the LICENSE file that accompanied this code).
//  *
//  * You should have received a copy of the GNU Lesser General Public License
//  * version 3 along with Titania.  If not, see
//  * <http://www.gnu.org/licenses/lgpl.html>
//  * for a copy of the LGPLv3 License.
//  *
//  ************************************************************************/

#include "jsChildObject.h"

namespace titania {
namespace pb {

jsChildObject::jsChildObject () :
	            jsBase (),
	jsGarbageCollector (),
	    referenceCount (0),
	           parents (),
	              root (nullptr),
	          children ()
{ }

void
jsChildObject::addChild (const jsChildObject & child)
{
	assert (&child not_eq this);

	children .emplace (const_cast <jsChildObject*> (&child));

	const_cast <jsChildObject &> (child) .addParent (this);
}

void
jsChildObject::removeChild (const jsChildObject & child)
{
	children .erase (const_cast <jsChildObject*> (&child));

	const_cast <jsChildObject &> (child) .removeParent (this);
}

void
jsChildObject::addParent (jsChildObject* const parent)
{
	if (root)
	{
		// Best guess for the shortest way to a rooted object.

		if (parent -> getParents () .size () < root -> getParents () .size ())
			root = parent;
	}
	else
		root = parent;

	if (parents .emplace (parent) .second)
		++ referenceCount;
}

void
jsChildObject::replaceParent (jsChildObject* const parentToRemove, jsChildObject* const parentToAdd)
{
	if (root == parentToRemove)
		root = parentToAdd;

	if (parents .erase (parentToRemove))
	{
		if (not parents .emplace (parentToAdd) .second)
			-- referenceCount;
	}
	else
		addParent (parentToAdd);
}

void
jsChildObject::removeParent (jsChildObject* const parent)
{
	if (parents .erase (parent))
	{			
		if (root == parent)
			root = nullptr;

		-- referenceCount;

		if (referenceCount == 0)
		{
			parents .clear ();

			dispose ();

			addDisposedObject (this);

			return;
		}

		ChildObjectSet circle;

		if (hasRootedObjects (circle))
			return;

		for (auto & child : circle)
		{
			child -> referenceCount = 0;
			child -> parents .clear ();
		}

		for (auto & child : circle)
			child -> dispose ();

		addDisposedObjects (circle .begin (), circle .end ());
	}
}

void
jsChildObject::addWeakParent (jsChildObject* const weakParent)
{
	parents .emplace (weakParent);
}

void
jsChildObject::removeWeakParent (jsChildObject* const weakParent)
{
	parents .erase (weakParent);
}

bool
jsChildObject::hasRootedObjects ()
{
	ChildObjectSet seen;

	return hasRootedObjects (seen); 
}

bool
jsChildObject::hasRootedObjects (ChildObjectSet & seen)
{
	if (parents .empty ())
		return true;

	if (seen .emplace (this) .second)
	{
		// First test the proved way.

		if (root)
		{
			if (root -> hasRootedObjects (seen))
				return true;
		}

		// Test all other ways and save the good way.

		for (auto & parent : parents)
		{
			if (parent -> hasRootedObjects (seen))
			{
				root = parent;
				return true;
			}
		}

		// We have no root and must test the next time again as parents can change.

		root = nullptr;
	}

	return false;
}

// Object

void
jsChildObject::dispose ()
{
	parents .clear ();
	root = nullptr;

	for (const auto & child : children)
		child -> dispose ();

	children .clear ();
}

jsChildObject::~jsChildObject ()
{ }

} // pb
} // titania
