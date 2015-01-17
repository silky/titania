/* -*- Mode: C++; coding: utf-8; tab-width: 3; indent-tabs-mode: tab; c-basic-offset: 3 -*-
 *******************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright create3000, Scheffelstra�e 31a, Leipzig, Germany 2011.
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

#ifndef __TITANIA_X3D_JAVA_SCRIPT_PEASE_BLOSSOM_FIELDS_ARRAY_FIELDS_H__
#define __TITANIA_X3D_JAVA_SCRIPT_PEASE_BLOSSOM_FIELDS_ARRAY_FIELDS_H__

#include "../Array.h"
#include "../../../Fields/ArrayFields.h"

#include "SFColor.h"
#include "SFColorRGBA.h"
#include "SFImage.h"
#include "SFMatrix3.h"
#include "SFMatrix4.h"
#include "SFNode.h"
#include "SFRotation.h"
#include "SFVec2.h"
#include "SFVec3.h"
#include "SFVec4.h"
#include "X3DScalar.h"

namespace titania {
namespace X3D {
namespace peaseblossom {

using MFBool      = Array <SFBool, X3D::MFBool>;
using MFColor     = Array <SFColor, X3D::MFColor>;
using MFColorRGBA = Array <SFColorRGBA, X3D::MFColorRGBA>;
using MFDouble    = Array <SFDouble, X3D::MFDouble>;
using MFFloat     = Array <SFFloat, X3D::MFFloat>;
using MFImage     = Array <SFImage, X3D::MFImage>;
using MFInt32     = Array <SFInt32, X3D::MFInt32>;
using MFMatrix3d  = Array <SFMatrix3d, X3D::MFMatrix3d>;
using MFMatrix3f  = Array <SFMatrix3f, X3D::MFMatrix3f>;
using MFMatrix4d  = Array <SFMatrix4d, X3D::MFMatrix4d>;
using MFMatrix4f  = Array <SFMatrix4f, X3D::MFMatrix4f>;
using MFNode      = Array <SFNode, X3D::MFNode>;
using MFString    = Array <SFString, X3D::MFString>;
using MFTime      = Array <SFTime, X3D::MFTime>;
using MFRotation  = Array <SFRotation, X3D::MFRotation>;
using MFVec2d     = Array <SFVec2d, X3D::MFVec2d>;
using MFVec2f     = Array <SFVec2f, X3D::MFVec2f>;
using MFVec3d     = Array <SFVec3d, X3D::MFVec3d>;
using MFVec3f     = Array <SFVec3f, X3D::MFVec3f>;
using MFVec4d     = Array <SFVec4d, X3D::MFVec4d>;
using MFVec4f     = Array <SFVec4f, X3D::MFVec4f>;


template <>
constexpr ObjectType
MFBool::getType ()
{ return ObjectType::MFBool; }

template <>
constexpr ObjectType
MFColor::getType ()
{ return ObjectType::MFColor; }

template <>
constexpr ObjectType
MFColorRGBA::getType ()
{ return ObjectType::MFColorRGBA; }

template <>
constexpr ObjectType
MFDouble::getType ()
{ return ObjectType::MFDouble; }

template <>
constexpr ObjectType
MFFloat::getType ()
{ return ObjectType::MFFloat; }

template <>
constexpr ObjectType
MFImage::getType ()
{ return ObjectType::MFImage; }

template <>
constexpr ObjectType
MFInt32::getType ()
{ return ObjectType::MFInt32; }

template <>
constexpr ObjectType
MFMatrix3d::getType ()
{ return ObjectType::MFMatrix3d; }

template <>
constexpr ObjectType
MFMatrix3f::getType ()
{ return ObjectType::MFMatrix3f; }

template <>
constexpr ObjectType
MFMatrix4d::getType ()
{ return ObjectType::MFMatrix4d; }

template <>
constexpr ObjectType
MFMatrix4f::getType ()
{ return ObjectType::MFMatrix4f; }

template <>
constexpr ObjectType
MFNode::getType ()
{ return ObjectType::MFNode; }

template <>
constexpr ObjectType
MFString::getType ()
{ return ObjectType::MFString; }

template <>
constexpr ObjectType
MFTime::getType ()
{ return ObjectType::MFTime; }

template <>
constexpr ObjectType
MFRotation::getType ()
{ return ObjectType::MFRotation; }

template <>
constexpr ObjectType
MFVec2d::getType ()
{ return ObjectType::MFVec2d; }

template <>
constexpr ObjectType
MFVec2f::getType ()
{ return ObjectType::MFVec2f; }

template <>
constexpr ObjectType
MFVec3d::getType ()
{ return ObjectType::MFVec3d; }

template <>
constexpr ObjectType
MFVec3f::getType ()
{ return ObjectType::MFVec3f; }

template <>
constexpr ObjectType
MFVec4d::getType ()
{ return ObjectType::MFVec4d; }

template <>
constexpr ObjectType
MFVec4f::getType ()
{ return ObjectType::MFVec4f; }

extern template class Array <SFBool, X3D::MFBool>;
extern template class Array <SFColor, X3D::MFColor>;
extern template class Array <SFColorRGBA, X3D::MFColorRGBA>;
extern template class Array <SFDouble, X3D::MFDouble>;
extern template class Array <SFFloat, X3D::MFFloat>;
extern template class Array <SFImage, X3D::MFImage>;
extern template class Array <SFInt32, X3D::MFInt32>;
extern template class Array <SFMatrix3d, X3D::MFMatrix3d>;
extern template class Array <SFMatrix3f, X3D::MFMatrix3f>;
extern template class Array <SFMatrix4d, X3D::MFMatrix4d>;
extern template class Array <SFMatrix4f, X3D::MFMatrix4f>;
extern template class Array <SFNode, X3D::MFNode>;
extern template class Array <SFString, X3D::MFString>;
extern template class Array <SFTime, X3D::MFTime>;
extern template class Array <SFRotation, X3D::MFRotation>;
extern template class Array <SFVec2d, X3D::MFVec2d>;
extern template class Array <SFVec2f, X3D::MFVec2f>;
extern template class Array <SFVec3d, X3D::MFVec3d>;
extern template class Array <SFVec3f, X3D::MFVec3f>;
extern template class Array <SFVec4d, X3D::MFVec4d>;
extern template class Array <SFVec4f, X3D::MFVec4f>;

} // peaseblossom
} // X3D
} // titania

#endif