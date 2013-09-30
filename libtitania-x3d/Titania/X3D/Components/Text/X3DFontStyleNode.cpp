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

#include "X3DFontStyleNode.h"

#include "../../Execution/X3DExecutionContext.h"
#include "../../Miscellaneous/FontConfig.h"

#include <Titania/OS/FileExists.h>

namespace titania {
namespace X3D {

X3DFontStyleNode::Fields::Fields () :
	     family (new MFString ({ "SERIF" })),
	      style (new SFString ("PLAIN")),
	    spacing (new SFFloat (1)),
	 horizontal (new SFBool (true)),
	    justify (new MFString ({ "BEGIN" })),
	topToBottom (new SFBool (true)),
	leftToRight (new SFBool (true)),
	   language (new SFString ())
{ }

X3DFontStyleNode::X3DFontStyleNode () :
	X3DPropertyNode (),
	         fields (),
	           font (),
	     lineHeight (1),
	          scale (1),
	     alignments ({ Alignment::BEGIN, Alignment::BEGIN })
{
	addNodeType (X3DConstants::X3DFontStyleNode);
}

void
X3DFontStyleNode::initialize ()
{
	X3DPropertyNode::initialize ();

	family  () .addInterest (this, &X3DFontStyleNode::set_font);
	style   () .addInterest (this, &X3DFontStyleNode::set_font);
	spacing () .addInterest (this, &X3DFontStyleNode::set_font);
	justify () .addInterest (this, &X3DFontStyleNode::set_justify);

	set_font ();
	set_justify ();
}

X3DFontStyleNode::Alignment
X3DFontStyleNode::getAlignment (const size_t index) const
{
	if (justify () [index] == "BEGIN")
		return Alignment::BEGIN;

	else if (justify () [index] == "FIRST")
		return Alignment::FIRST;

	else if (justify () [index] == "MIDDLE")
		return Alignment::MIDDLE;

	else if (justify () [index] == "END")
		return Alignment::END;

	return index ? Alignment::FIRST : Alignment::BEGIN;
}

int
X3DFontStyleNode::loadFont ()
{
	bool isExactMatch = false;

	for (const auto & familyName : family ())
	{
		std::string filename = getFilename (familyName, isExactMatch);

		if (isExactMatch)
		{
			// Create a pixmap font from a TrueType file.
			font .reset (new FTPolygonFont (filename .c_str ()));

			// Check for errors	
			if (font -> Error () == 0)
				return 0;
		}
	}

	font .reset (new FTPolygonFont (getFilename ("SERIF", isExactMatch) .c_str ()));
	
	return font -> Error ();
}

std::string
X3DFontStyleNode::getFilename (const String & familyName, bool & isExactMatch) const
{
	// Test if familyName is a valid path

	basic::uri uri = getExecutionContext () -> getWorldURL () .transform (familyName .raw ());
	
	if (uri .is_local ())
	{
		if (os::file_exists (uri .path ()))
		{
			isExactMatch = true;
			return uri .path ();
		}
	}

	// Get a filename from font server

	FcPattern* pattern = FcNameParse ((FcChar8*) familyName .c_str ());

	FcPatternAddString (pattern, "style", (FcChar8*) (style () == "BOLDITALIC" ? "bold italic" : style () .getValue () .c_str ()));

	FcConfigSubstitute (nullptr, pattern, FcMatchPattern);
	FcDefaultSubstitute (pattern);

	String familyNameAfterConfiguration = get_family_name (pattern);

	FcResult   result;
	FcPattern* match = FcFontMatch (nullptr, pattern, &result);

	String familyNameAfterMatching = get_family_name (match);

	isExactMatch = familyNameAfterConfiguration .lowercase () == familyNameAfterMatching .lowercase ();

	std::string filename = get_filename (match);

	FcPatternDestroy (pattern);
	FcPatternDestroy (match);

	return filename;
}

void
X3DFontStyleNode::set_font ()
{
	// Create a polygon font from a TrueType file.
	loadFont ();
	
	if (font -> Error () == 0)
	{
		font -> CharMap (ft_encoding_unicode);
		font -> UseDisplayList (true);

		// Set the font size to large text.
		font -> FaceSize (100);

		// Calculate lineHeight.
		lineHeight = font -> LineHeight () * spacing ();

		// Calculate scale.
		scale = getSize () / font -> LineHeight ();
	}
	else
		font .reset ();
}

void
X3DFontStyleNode::set_justify ()
{
	alignments [0] = justify () .size () > 0
	                 ? getAlignment (0)
						  : Alignment::BEGIN;

	alignments [1] = justify () .size () > 1
	                 ? getAlignment (1)
						  : Alignment::FIRST;
}

void
X3DFontStyleNode::dispose ()
{
	font .reset ();

	X3DPropertyNode::dispose ();
}

} // X3D
} // titania
