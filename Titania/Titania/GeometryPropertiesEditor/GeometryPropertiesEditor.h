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

#ifndef __TITANIA_GEOMETRY_PROPERTIES_EDITOR_GEOMETRY_PROPERTIES_EDITOR_H__
#define __TITANIA_GEOMETRY_PROPERTIES_EDITOR_GEOMETRY_PROPERTIES_EDITOR_H__

#include "../Browser/BrowserWindow.h"
#include "../Undo/UndoStep.h"
#include "../UserInterfaces/X3DGeometryPropertiesEditorInterface.h"

namespace titania {
namespace puck {

class GeometryPropertiesEditor :
	public X3DGeometryPropertiesEditorInterface
{
public:

	///  @name Construction

	GeometryPropertiesEditor (BrowserWindow* const);

	///  @name Destruction

	virtual
	~GeometryPropertiesEditor ();


private:

	///  @name Construction

	virtual
	void
	initialize () final override;

	///  @name Event handlers

	void
	set_selection ();

	virtual
	void
	on_solid_toggled () final override;

	void
	set_solid ();

	void
	connectSolid (const X3D::SFBool &);

	virtual
	void
	on_ccw_toggled () final override;

	void
	set_ccw ();

	void
	connectCCW (const X3D::SFBool &);

	virtual
	void
	on_convex_toggled () final override;

	void
	set_convex ();

	void
	connectConvex (const X3D::SFBool &);

	virtual
	void
	on_creaseAngle_changed () final override;

	void
	set_creaseAngle ();

	void
	connectCreaseAngle (const X3D::SFFloat &);

	virtual
	void
	on_textureCoordinateGenerator_toggled () final override;

	void
	set_textureCoordinateGenerator ();

	void
	connectTextureCoordinateGenerator (const X3D::SFNode &);

	///  @name Operations

	template <class Type>
	void
	addUndoFunction (const std::string &);

	template <class Type>
	void
	addRedoFunction (const std::string &);

	///  @name Members

	X3D::X3DPtrArray <X3D::X3DGeometryNode>       geometryNodes;
	X3D::X3DPtr <X3D::TextureCoordinateGenerator> textureCoordinateGenerator;
	UndoStepPtr                                   undoStep;
	std::string                                   currentField;
	bool                                          changing;

};

template <class Type>
void
GeometryPropertiesEditor::addUndoFunction (const std::string & name)
{
	const auto lastUndoStep = getBrowserWindow () -> getLastUndoStep ();

	if (undoStep and lastUndoStep == undoStep and name == currentField)
		return;

	currentField = name;

	undoStep = std::make_shared <UndoStep> (_ ("Geometry Change"));

	undoStep -> addVariables (geometryNodes);

	// Undo field change

	for (const auto & geometry : geometryNodes)
	{
		try
		{
			auto & field = geometry -> getField <Type> (name);

			undoStep -> addUndoFunction (&Type::setValue, std::ref (field), field .getValue ());
		}
		catch (const X3D::X3DError &)
		{ }
	}

	getBrowserWindow () -> addUndoStep (undoStep);
}

template <class Type>
void
GeometryPropertiesEditor::addRedoFunction (const std::string & name)
{
	undoStep -> clearRedoFunctions ();

	// Redo field change

	for (const auto & geometry : geometryNodes)
	{
		try
		{
			auto & field = geometry -> getField <Type> (name);

			undoStep -> addRedoFunction (&Type::setValue, std::ref (field), field .getValue ());
		}
		catch (const X3D::X3DError &)
		{ }
	}
}

} // puck
} // titania

#endif
