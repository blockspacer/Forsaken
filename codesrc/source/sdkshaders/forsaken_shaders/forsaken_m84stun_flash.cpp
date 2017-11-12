/*
	forsaken_m84stun_flash.cpp
	Forsaken shader for the M84 Stun Grenade flash.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "BaseVSShader.h"
#include "convar.h"

// Shader Includes
#include "forsaken_m84stun_flash_ps20.inc"
#include "forsaken_m84stun_flash_vs20.inc"

// Shader Start
BEGIN_VS_SHADER( Forsaken_M84Stun_Flash, "Help for Forsaken_M84Stun_Flash" )

	BEGIN_SHADER_PARAMS
		SHADER_PARAM( FLASHCOLOR, SHADER_PARAM_TYPE_COLOR, "0 0 0", "The color of the flash." )
		SHADER_PARAM( TIMECONSTANT, SHADER_PARAM_TYPE_FLOAT, "0.0", "The amount of time elapsed for LERP (0...1)" )
	END_SHADER_PARAMS

	// Set up anything that is necessary to make decisions in SHADER_FALLBACK.
	SHADER_INIT_PARAMS()
	{
	}

	SHADER_FALLBACK
	{
		return 0;
	}

	bool NeedsFullFrameBufferTexture( IMaterialVar **params ) const
	{
		return true;
	}

	SHADER_INIT
	{
	}

	SHADER_DRAW
	{
		SHADOW_STATE
		{
			// Enable the texture for base texture and lightmap.
			pShaderShadow->EnableTexture( SHADER_TEXTURE_STAGE0, true );
			pShaderShadow->EnableDepthWrites( false );

			int fmt = VERTEX_POSITION;
			pShaderShadow->VertexShaderVertexFormat( fmt, 1, 0, 0, 0 );

			DECLARE_STATIC_PIXEL_SHADER(forsaken_m84stun_flash_ps20);
			SET_STATIC_PIXEL_SHADER(forsaken_m84stun_flash_ps20);

			DECLARE_STATIC_VERTEX_SHADER(forsaken_m84stun_flash_vs20);
			SET_STATIC_VERTEX_SHADER(forsaken_m84stun_flash_vs20);

			DefaultFog();
		}
		DYNAMIC_STATE
		{
			float fTimeConstant[4];
			Vector vFlashColor;

			// Setup the flash color.
			const float *fFlashColor = params[FLASHCOLOR]->GetVecValue();
			vFlashColor = Vector(fFlashColor[0], fFlashColor[1], fFlashColor[2]);

			// Setup the time constant.
			fTimeConstant[0] = params[TIMECONSTANT]->GetFloatValue();
			fTimeConstant[1] = fTimeConstant[2] = fTimeConstant[3] = fTimeConstant[0];

			// Upload the constants.
			pShaderAPI->SetPixelShaderConstant(0, fTimeConstant);
			pShaderAPI->SetPixelShaderConstant(1, vFlashColor.Base());

			// Setup the texture and index.
			pShaderAPI->BindFBTexture( SHADER_TEXTURE_STAGE0 );

			DECLARE_DYNAMIC_PIXEL_SHADER(forsaken_m84stun_flash_ps20);
			SET_DYNAMIC_PIXEL_SHADER(forsaken_m84stun_flash_ps20);

			DECLARE_DYNAMIC_VERTEX_SHADER(forsaken_m84stun_flash_vs20);
			SET_DYNAMIC_VERTEX_SHADER(forsaken_m84stun_flash_vs20);
		}

		Draw();
	}
END_SHADER