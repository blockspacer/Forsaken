#ifndef __FORSAKEN_SHAREDDEFS_H_
#define __FORSAKEN_SHAREDDEFS_H_

// Steam ID length
#define MAX_STEAM_ID_LENGTH	32

// Base animation event
#define FORSAKEN_ANIMEVENT_BASE			10000
#define FORSAKEN_ANIMEVENT_MUZZLEFLASH	(FORSAKEN_ANIMEVENT_BASE + 1)

// Objective Types
#define OBJECTIVE_TYPE_BASE		0
#define OBJECTIVE_TYPE_CAPTURE	1
#define OBJECTIVE_TYPE_DESTROY	2

// Spawn Radius
#define FORSAKEN_SPAWN_RADIUS			32

// Button Defines
#define IN_KICK			(1 << 23)
#define IN_LEAN			(1 << 24)
#define IN_SPRINT		(1 << 25)
#define IN_OBJECTIVES	(1 << 26)

// Lean Defines
#define LEAN_DIR_NONE	0
#define LEAN_DIR_LEFT	1
#define LEAN_DIR_RIGHT	2
#define FORSAKEN_PLAYER_LEANLEFT_OFFSET		Vector(-20.0f, -5.0f, 0.0f)
#define FORSAKEN_PLAYER_LEANRIGHT_OFFSET	Vector(20.0f, -5.0f, 0.0f)

// Player Offset
#define FORSAKEN_PLAYER_DUCK_VIEW_OFFSET		Vector(0.0f, 0.0f, 42.94f)
#define FORSAKEN_PLAYER_STAND_VIEW_OFFSET		Vector(0.0f, 0.0f, 55.783f)

// Leader Preferences
#define LEADEROPTION_UNKNOWN	-1
#define LEADEROPTION_YES		0
#define LEADEROPTION_NO			1
#define LEADEROPTION_MAYBE		2

// Team Bitmasks
#define TEAM_CIVILIANS_BITMASK	1
#define TEAM_GOVERNORS_BITMASK	2
#define TEAM_RELIGIOUS_BITMASK	4

// Team Weapon Packs
// Civilian Weapon Packs and Accessories
#define CIV_WEAPACC_WTF					0x01
#define CIV_WEAPACC_WTFOMG				0x02
#define CIV_WEAPPACK_LDR_RPGRAGINGBULL	1		// RPG (2), Raging Bull w/ 15buls
#define CIV_WEAPPACK_LDR_RPGHKMK23		2		// RPG (3), HKMK23 w/ 24buls
#define CIV_WEAPPACK_LDR_SNIPER			3		// Sniper w/ 10buls, Raging Bull w/ 10buls
#define CIV_WEAPPACK_STD_RAGINGBULL		4		// Raging Bull w/ 20buls
#define CIV_WEAPPACK_STD_HKMK23MOLOTOV	5		// HKMK23 w/ 36buls, Molotov
#define CIV_WEAPPACK_STD_HKMK23GREN		6		// HKMK23 w/ 36buls, Grenade
#define CIV_WEAPPACK_STD_SNIPER			7		// Sniper w/ 20buls
#define CIV_WEAPPACK_STD_SNIPERHKMK23	8		// Sniper w/ 8buls, HKMK23 w/ 24buls
#define CIV_WEAPPACK_STD_RAGINGBULLGREN	9		// Raging Bull w/ 7buls, Grenade

// Governor Weapon Packs and Accessories
#define GOV_WEAPACC_WTF					0x01
#define GOV_WEAPACC_WTFOMG				0x02
#define GOV_WEAPPACK_LDR_SA80			1		// SA80 w/ 120buls, 2 Grens
#define GOV_WEAPPACK_LDR_SHOTGUNGREN	2		// Shotgun w/ 15buls, 2 Grens
#define GOV_WEAPPACK_LDR_SHOTGUNHKMK23	3		// Shotgun w/ 15buls, HKMK23 w/ 36buls
#define GOV_WEAPPACK_STD_SA80			4		// SA80 w/ 90buls
#define GOV_WEAPPACK_STD_SA80HKMK23		5		// SA80 w/ 30buls, HKMK23 w/ 18buls
#define GOV_WEAPPACK_STD_HKMK23GREN		6		// HKMK23 w/ 36buls, 1 Gren

// Religous Weapon Packs and Accessories
#define REL_WEAPACC_WTF			0x01
#define REL_WEAPACC_WTFOMG		0x02
#define REL_WEAPPACK_LDR_INGRAMHKMK23	1		// Dual Ingrams w/ 120buls, HKMK23 w/ 36buls
#define REL_WEAPPACK_LDR_INGRAMMOLOTOV	2		// Dual Ingrams w/ 120buls, Molotov
#define REL_WEAPPACK_LDR_P99SMOLOTOVS	3		// Dual P99s w/ 48buls, 2 Molotovs
#define REL_WEAPPACK_STD_P99S			4		// Dual P99s w/ 60buls
#define REL_WEAPPACK_STD_SHOTGUN		5		// Shotgun w/ 15buls
#define REL_WEAPPACK_STD_HKMK23MOLOTOV	6		// HKMK23 w/ 36buls, Molotov
#define REL_WEAPPACK_STD_SHOTGUNHKMK23	7		// Shotgun w/ 5buls, HKMK23 w/ 18buls
#define REL_WEAPPACK_STD_P99SMOLOTOV	8		// Dual P99s w/ 24buls, Molotov
#define REL_WEAPPACK_STD_SHOTGUNMOLOTOV	9		// Shotgun w/ 5buls, Molotov

// Leader Auras
enum LeaderAuras
{
	Aura_None = 0,
	GovAura_ReduceRecoil,
	RelAura_PainThreshold,
	CivAura_IncrStamina,
};

typedef enum
{
	FORSAKEN_MOVEMENT_NONE = 0,

	FORSAKEN_MOVEMENT_WALK,
	FORSAKEN_MOVEMENT_RUN,
	FORSAKEN_MOVEMENT_CROUCH,
	FORSAKEN_MOVEMENT_CROUCHWALK,
	FORSAKEN_MOVEMENT_JUMP
} ForsakenMovementID;

// Resource Types
#define FORSAKEN_RESOURCETYPE_NONE			0
#define FORSAKEN_RESOURCETYPE_NUTRITION		1
#define FORSAKEN_RESOURCETYPE_ORDINANCE		2

// Win Types
#define FORSAKEN_WINTYPE_NONE	0
#define FORSAKEN_WINTYPE_ROUND	1
#define FORSAKEN_WINTYPE_MAP	2

// String Lengths
#define OBJECTIVE_NAME_LENGTH	256

// User Message Information
// CapturingObjective
#define UMSG_CAPTURE_INPROGRESS	0x01
#define UMSG_CAPTURE_PAUSE		0x02

// Stun Grenade Duration
#define FORSAKEN_M84STUN_DEFLECTIONTRACE	6.0f
#define FORSAKEN_M84STUN_FADERADIUS_MAX		1024.0f
#define FORSAKEN_M84STUN_FADERADIUS_MIN		64.0f
#define FORSAKEN_M84STUN_SIMPLERADIUS		512.0f
#define FORSAKEN_M84STUN_TIME				10.0f

// PointsNotice
#define UMSG_POINTS_KILLASSIST	0x01
#define UMSG_POINTS_KILL		0x02
#define UMSG_POINTS_OBJCAPTURE	0x03
#define UMSG_POINTS_OBJDESTROY	0x04
#define UMSG_POINTS_OBJASSIST	0x05
#define UMSG_POINTS_PROTECTLDR	0x06
#define UMSG_POINTS_TEAMKILL	0x07

// Win Scenario Sound Names
#define FORSAKEN_WINSOUND_MAP_CIV			"Map.CivWin"
#define FORSAKEN_WINSOUND_MAP_GOV			"Map.GovWin"
#define FORSAKEN_WINSOUND_MAP_REL			"Map.RelWin"
#define FORSAKEN_WINSOUND_ROUND_CIV			"Round.CivWin"
#define FORSAKEN_WINSOUND_ROUND_GOV			"Round.GovWin"
#define FORSAKEN_WINSOUND_ROUND_REL			"Round.RelWin"
#define FORSAKEN_WINSOUND_WAR_CIV			"War.CivWin"
#define FORSAKEN_WINSOUND_WAR_GOV			"War.GovWin"
#define FORSAKEN_WINSOUND_WAR_REL			"War.RelWin"

// Credits defines
#define CREDITS_LOGO	1
#define CREDITS_INTRO	2
#define CREDITS_OUTRO	3

// Fortfied Position Levels
#define FORSAKEN_FORTIFIEDLEVEL_ZERO		0
#define FORSAKEN_FORTIFIEDLEVEL_ONE			1
#define FORSAKEN_FORTIFIEDLEVEL_TWO			2
#define FORSAKEN_FORTIFIEDLEVEL_THREE		3

// Fortified Position Spawn Amounts
#define FORSAKEN_FORTIFIEDLEVELSPAWN_ZERO	0.0f
#define FORSAKEN_FORTIFIEDLEVELSPAWN_ONE	0.25f
#define FORSAKEN_FORTIFIEDLEVELSPAWN_TWO	0.5f
#define FORSAKEN_FORTIFIEDLEVELSPAWN_THREE	1.0f

// Fortified Position Status'
#define FORSAKEN_FORTIFIEDSTATUS_NONE		0
#define FORSAKEN_FORTIFIEDSTATUS_BUILDING	1
#define FORSAKEN_FORTIFIEDSTATUS_UPGRADING	2
#define FORSAKEN_FORTIFIEDSTATUS_DESTROYED	3
#define FORSAKEN_FORTIFIEDSTATUS_COMPLETE	4

#endif