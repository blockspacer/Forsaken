//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
// 
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "c_forsaken_player.h"
#include "forsaken/forsaken_weapon_base.h"
#include "vguimatsurface/IMatSystemSurface.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/IMesh.h"
#include "materialsystem/imaterialvar.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Draws the zoom screen
//-----------------------------------------------------------------------------
class CHudZoom : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE( CHudZoom, vgui::Panel );

public:
	CHudZoom( const char *pElementName );
	
	bool	ShouldDraw( void );
	void	Init( void );
	void	LevelInit( void );
	void	Reset();
	void	VidInit();

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint( void );

private:
	bool	m_bZoomOn;
	float	m_flZoomStartTime;
	bool	m_bPainted;

	CPanelAnimationVarAliasType( float, m_flCircle1Radius, "Circle1Radius", "66", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flCircle2Radius, "Circle2Radius", "74", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDashGap, "DashGap", "16", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDashHeight, "DashHeight", "4", "proportional_float" );

	//CMaterialReference m_ZoomMaterial;
	int m_textureScope;
};

DECLARE_HUDELEMENT( CHudZoom );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudZoom::CHudZoom( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "HudZoom")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	// Forsaken Addition: Hide HUD if credits are going.
	SetHiddenBits( HIDEHUD_PLAYERDEAD | HIDEHUD_CREDITS );
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudZoom::Init( void )
{
	m_bZoomOn = false;
	m_bPainted = false;
	m_flZoomStartTime = -999.0f;
	//m_ZoomMaterial.Init( "vgui/scopetest", TEXTURE_GROUP_VGUI );
	m_textureScope = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_textureScope, "vgui/scopetest", true, true);
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CHudZoom::LevelInit( void )
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CHudZoom::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
	SetFgColor(scheme->GetColor("ZoomReticleColor", GetFgColor()));

	int screenWide, screenTall;
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudZoom::ShouldDraw( void )
{
	bool bNeedsDraw = false;

	C_ForsakenPlayer *pPlayer = C_ForsakenPlayer::GetLocalForsakenPlayer();
	if ( pPlayer == NULL )
		return false;

	if(pPlayer->GetTeamNumber() == TEAM_SPECTATOR && pPlayer->GetObserverMode() == OBS_MODE_IN_EYE)
	{
		C_BaseEntity *pBaseFollowed = ClientEntityList().GetEnt( GetSpectatorTarget() );
		C_ForsakenPlayer *pFsknFollowed = ToForsakenPlayer( pBaseFollowed );
		if ( pFsknFollowed == NULL )
			return false;
		else
			pPlayer = pFsknFollowed;
	}

	C_ForsakenWeaponBase *pWeapon = pPlayer->GetActiveForsakenWeapon();
	if ( pWeapon == NULL )
		return false;

	if ( pWeapon->IsWeaponZoomed() )
	{
		// need to paint
		bNeedsDraw = true;
	}
	else if ( m_bPainted )
	{
		// keep painting until state is finished
		//bNeedsDraw = true;
	}

	return ( bNeedsDraw && CHudElement::ShouldDraw() );
}

#define	ZOOM_FADE_TIME	0.4f
//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CHudZoom::Paint( void )
{
	m_bPainted = false;

	// see if we're zoomed any
	C_ForsakenPlayer *pPlayer = C_ForsakenPlayer::GetLocalForsakenPlayer();
	if ( pPlayer == NULL )
		return;

	if(pPlayer->GetTeamNumber() == TEAM_SPECTATOR && pPlayer->GetObserverMode() == OBS_MODE_IN_EYE)
	{
		C_BaseEntity *pBaseFollowed = ClientEntityList().GetEnt( GetSpectatorTarget() );
		C_ForsakenPlayer *pFsknFollowed = ToForsakenPlayer( pBaseFollowed );
		if ( pFsknFollowed == NULL )
			return;
		else
			pPlayer = pFsknFollowed;
	}

	C_ForsakenWeaponBase *pWeapon = pPlayer->GetActiveForsakenWeapon();
	if ( pWeapon == NULL )
		return;

	if ( pWeapon->IsWeaponZoomed() && m_bZoomOn == false )
	{
		m_bZoomOn = true;
		m_flZoomStartTime = gpGlobals->curtime;
	}
	else if ( pWeapon->IsWeaponZoomed() == false && m_bZoomOn )
	{
		m_bZoomOn = false;
		m_flZoomStartTime = gpGlobals->curtime;
	}

	// Initial paint state.
	SetPaintBackgroundEnabled(false);

	// draw the appropriately scaled zoom animation
	float deltaTime = ( gpGlobals->curtime - m_flZoomStartTime );
	float scale = clamp( deltaTime / ZOOM_FADE_TIME, 0.0f, 1.0f );
	
	float alpha;

	if ( m_bZoomOn )
	{
		alpha = scale;
	}
	else
	{
		if ( scale >= 1.0f )
			return;

		alpha = ( 1.0f - scale ) * 0.25f;
		scale = 1.0f - ( scale * 0.5f );
	}

	Color col = GetFgColor();
	col[3] = alpha * 64;

	surface()->DrawSetColor( col );
	
	// draw zoom circles
	int wide, tall;
	GetSize(wide, tall);
	surface()->DrawOutlinedCircle(wide / 2, tall / 2, m_flCircle1Radius * scale, 48);
	surface()->DrawOutlinedCircle(wide / 2, tall / 2, m_flCircle2Radius * scale, 64);

	// draw dashed lines
	int dashCount = 1;
	int ypos = (tall - m_flDashHeight) / 2;
	int xpos = (int)((wide / 2) + (m_flDashGap * ++dashCount * scale));
	while (xpos < wide && xpos > 0)
	{
		surface()->DrawFilledRect(xpos, ypos, xpos + 1, ypos + m_flDashHeight);
		surface()->DrawFilledRect(wide - xpos, ypos, wide - xpos + 1, ypos + m_flDashHeight);
		xpos = (int)((wide / 2) + (m_flDashGap * ++dashCount * max(scale,0.1f)));
	}

	// Draw center dot and circle
	surface()->DrawSetColor( 255, 0, 0, col[3] );
	surface()->DrawOutlinedCircle(wide / 2.0f, tall / 2.0f, 1.0f, 10);
	surface()->DrawSetColor( 255, 0, 0, col[3] );
	surface()->DrawOutlinedCircle(wide / 2.0f, tall / 2.0f, 10.0f, 20);

	//materials->Bind( m_ZoomMaterial );
	//IMesh *pMesh = materials->GetDynamicMesh( true, NULL, NULL, NULL );

	float xOffset = (wide - tall) / 2.0f;
	float x0 = 0.0f, x1 = xOffset, x2 = xOffset + tall, x3 = wide;
	float y0 = 0.0f, y2 = tall;

	/*float uv0 = 0.0f + (1.0f / 255.0f);
	float uv1 = 1.0f - (1.0f / 255.0f);

	struct coord_t
	{
		float x, y;
		float u, v;
	};
	coord_t coords[4] = 
	{
		// scope
		{ x1, y0, uv1, uv0 },
		{ x2, y0, uv0, uv0 },
		{ x2, y2, uv0, uv1 },
		{ x1, y2, uv1, uv1 },
	};

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	for (int i = 0; i < 4; i++)
	{
		meshBuilder.Color4f( 0.0, 0.0, 0.0, alpha );
		meshBuilder.TexCoord2f( 0, coords[i].u, coords[i].v );
		meshBuilder.Position3f( coords[i].x, coords[i].y, 0.0f );
		meshBuilder.AdvanceVertex();
	}

	meshBuilder.End();

	pMesh->Draw();*/

	vgui::surface()->DrawSetColor(255, 255, 255, alpha*255);
	vgui::surface()->DrawSetTexture(m_textureScope);
	vgui::surface()->DrawTexturedRect(x1, y0, x2, y2);

	vgui::surface()->DrawSetColor(0, 0, 0, alpha*255);
	vgui::surface()->DrawFilledRect(x0, y0, x1, y2);
	vgui::surface()->DrawFilledRect(x2, y0, x3, y2);

	m_bPainted = true;
}

void CHudZoom::Reset()
/*

Pre: 
Post: 
*/
{
	m_bZoomOn = false;
}

void CHudZoom::VidInit()
/*

Pre: 
Post: 
*/
{
	m_bZoomOn = false;
}