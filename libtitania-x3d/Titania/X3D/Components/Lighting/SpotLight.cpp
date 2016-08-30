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

#include "SpotLight.h"

#include "../../Browser/X3DBrowser.h"
#include "../../Execution/X3DExecutionContext.h"
#include "../../Tools/Lighting/SpotLightTool.h"
#include "../Shaders/X3DProgrammableShaderObject.h"

namespace titania {
namespace X3D {

static constexpr int32_t SPOT_LIGHT = 3;

const ComponentType SpotLight::component      = ComponentType::LIGHTING;
const std::string   SpotLight::typeName       = "SpotLight";
const std::string   SpotLight::containerField = "children";

SpotLight::Fields::Fields () :
	attenuation (new SFVec3f (1, 0, 0)),
	  direction (new SFVec3f (0, 0, -1)),
	   location (new SFVec3f ()),
	     radius (new SFFloat (100)),
	  beamWidth (new SFFloat (0.785398)),
	cutOffAngle (new SFFloat (1.5708))
{ }

SpotLight::SpotLight (X3DExecutionContext* const executionContext) :
	 X3DBaseNode (executionContext -> getBrowser (), executionContext),
	X3DLightNode (),
	      fields ()
{
	addType (X3DConstants::SpotLight);

	addField (inputOutput, "metadata",         metadata ());
	addField (inputOutput, "global",           global ());
	addField (inputOutput, "on",               on ());
	addField (inputOutput, "color",            color ());
	addField (inputOutput, "intensity",        intensity ());
	addField (inputOutput, "ambientIntensity", ambientIntensity ());
	addField (inputOutput, "attenuation",      attenuation ());
	addField (inputOutput, "location",         location ());
	addField (inputOutput, "direction",        direction ());
	addField (inputOutput, "radius",           radius ());
	addField (inputOutput, "beamWidth",        beamWidth ());
	addField (inputOutput, "cutOffAngle",      cutOffAngle ());

	location ()    .setUnit (UnitCategory::LENGTH);
	radius ()      .setUnit (UnitCategory::LENGTH);
	beamWidth ()   .setUnit (UnitCategory::ANGLE);
	cutOffAngle () .setUnit (UnitCategory::ANGLE);
}

X3DBaseNode*
SpotLight::create (X3DExecutionContext* const executionContext) const
{
	return new SpotLight (executionContext);
}

void
SpotLight::initialize ()
{
	X3DLightNode::initialize ();

	addInterest (this, &SpotLight::eventsProcessed);

	eventsProcessed ();
}

void
SpotLight::eventsProcessed ()
{
	const float glAmbientIntensity = math::clamp <float> (ambientIntensity (), 0, 1);
	const float glIntensity        = math::clamp <float> (intensity (), 0, 1);

	glAmbient [0] = glAmbientIntensity * color () .getRed ();
	glAmbient [1] = glAmbientIntensity * color () .getGreen ();
	glAmbient [2] = glAmbientIntensity * color () .getBlue ();
	glAmbient [3] = 1;

	glDiffuseSpecular [0] = glIntensity * color () .getRed ();
	glDiffuseSpecular [1] = glIntensity * color () .getGreen ();
	glDiffuseSpecular [2] = glIntensity * color () .getBlue ();
	glDiffuseSpecular [3] = 1;

	glSpotExponent = math::clamp <float> (beamWidth () ? 0.5f / beamWidth () : 0.0f, 0, 128);
	glSpotCutOff   = math::clamp <float> (math::degrees <float> (cutOffAngle ()), 0, 90);

	glPosition [0] = location () .getX ();
	glPosition [1] = location () .getY ();
	glPosition [2] = location () .getZ ();
	glPosition [3] = 1; // point light

	glSpotDirection [0] = direction () .getX ();
	glSpotDirection [1] = direction () .getY ();
	glSpotDirection [2] = direction () .getZ ();
}

void
SpotLight::draw (GLenum lightId)
{
	glLightfv (lightId, GL_AMBIENT,  glAmbient);
	glLightfv (lightId, GL_DIFFUSE,  glDiffuseSpecular);
	glLightfv (lightId, GL_SPECULAR, glDiffuseSpecular);

	glLightf (lightId, GL_SPOT_EXPONENT, glSpotExponent);
	glLightf (lightId, GL_SPOT_CUTOFF,   glSpotCutOff);

	glLightf (lightId, GL_CONSTANT_ATTENUATION,  std::max (0.0f, attenuation () .getX ()));
	glLightf (lightId, GL_LINEAR_ATTENUATION,    std::max (0.0f, attenuation () .getY ()));
	glLightf (lightId, GL_QUADRATIC_ATTENUATION, std::max (0.0f, attenuation () .getZ ()));

	glLightfv (lightId, GL_POSITION,       glPosition);
	glLightfv (lightId, GL_SPOT_DIRECTION, glSpotDirection);
}

void
SpotLight::renderShadowMap (LightContainer* const lightContainer)
{

}

void
SpotLight::setShaderUniforms (X3DProgrammableShaderObject* const shaderObject, const size_t i, const Matrix4d & modelViewMatrix)
{
	const auto worldLocation  = Vector3f (modelViewMatrix .mult_vec_matrix (location () .getValue ()));
	const auto worldDirection = Vector3f (normalize (modelViewMatrix .mult_dir_matrix (direction () .getValue ())));

	glUniform1i  (shaderObject -> getLightTypeUniformLocation             () [i], SPOT_LIGHT);
	glUniform3fv (shaderObject -> getLightColorUniformLocation            () [i], 1, color () .getValue () .data ());
	glUniform1f  (shaderObject -> getLightIntensityUniformLocation        () [i], intensity ());        // clamp
	glUniform1f  (shaderObject -> getLightAmbientIntensityUniformLocation () [i], ambientIntensity ()); // clamp
	glUniform3fv (shaderObject -> getLightAttenuationUniformLocation      () [i], 1, attenuation () .getValue () .data ());
	glUniform3fv (shaderObject -> getLightLocationUniformLocation         () [i], 1, worldLocation .data ());
	glUniform3fv (shaderObject -> getLightDirectionUniformLocation        () [i], 1, worldDirection .data ());
	glUniform1f  (shaderObject -> getLightBeamWidthUniformLocation        () [i], beamWidth ());   // clamp
	glUniform1f  (shaderObject -> getLightCutOffAngleUniformLocation      () [i], cutOffAngle ()); // clamp
	glUniform1f  (shaderObject -> getLightRadiusUniformLocation           () [i], radius ());
}

void
SpotLight::addTool ()
{
	X3DLightNode::addTool (new SpotLightTool (this));
}

} // X3D
} // titania
