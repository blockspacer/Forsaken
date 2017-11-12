/*
	forsaken_mapentity_filter.cpp
	Map entity filter for round restarts.

	Copyright© 2005 Forsaken Mod Team.
	All Rights Reserved.
*/

#include "cbase.h"
#include "mapentities.h"
#include "UtlSortVector.h"
#include "forsaken_mapentity_filter.h"

// Constants
static const char *g_czEntityPreserveList[] = 
{
	"worldspawn",
	"player_manager",
	"player",
	"viewmodel",
	"soundent",
	"ai_network",
	"ai_hint",
	"forsaken_gamerules",
	"forsaken_player_manager",
	"forsaken_team_manager",
	"env_soundscape",
	"env_soundscape_proxy",
	"env_soundscape_triggerable",
	"env_sprite",
	"env_sun",
	"env_wind",
	"env_fog_controller",
	"func_brush",
	"func_wall",
	"func_illusionary",
	"info_node",
	"info_target",
	"info_node_hint",
	"point_viewcontrol",
	"shadow_control",
	"sky_camera",
	"scene_manager",
	"trigger_soundscape",
	"forsaken_civspawn",
	"forsaken_govspawn",
	"forsaken_relspawn",
	"forsaken_objspawn",
	"forsaken_joinspawn",
	"light",
	"infodecal",
	"move_rope",
	"keyframe_rope",
	"env_lightglow",
	"func_areaportal",
	"light_environment",
	"ambient_generic",
	"water_lod_control",
	"env_steam",
	"func_dustmotes",
	"forsaken_captureobjective_entity",
	"forsaken_destroyobjective_entity",
	"forsaken_fortifiedsupport_entity",
	"forsaken_fortified_entity",
	"forsaken_winlogic_entity",
	"forsaken_env_campfire",
	"",
};

// Constructor & Deconstructor
CForsakenMapEntityFilter::CForsakenMapEntityFilter()
{
}

CForsakenMapEntityFilter::~CForsakenMapEntityFilter()
{
}

// Functions
bool CForsakenMapEntityFilter::ShouldCreateEntity(const char *czClassname)
{
	for (unsigned short i = 0; g_czEntityPreserveList[i][0] != '\0'; i++)
	{
		if (Q_stricmp(czClassname, g_czEntityPreserveList[i]) == 0)
			return false;
	}

	return true;
}

CBaseEntity *CForsakenMapEntityFilter::CreateNextEntity(const char *czClassname)
{
	if (ShouldCreateEntity(czClassname))
	{
		DevMsg("%s\n", czClassname);
		return CreateEntityByName(czClassname);
	}
	else
		return NULL;
}