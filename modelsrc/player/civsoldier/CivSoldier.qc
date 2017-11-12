//==============================================//
//	Temp Civilian Soldier player model	//
//	Writen by Sean McBride 3/1/05		//
//	For Forsaken. www.forsaken-game.com	//
//==============================================//

$modelname "player/civSoldier/civSoldier.mdl"
$cdmaterials "models/player/civSoldier"
$cdmaterials "models/player/civLeader"

//$scale 1.14

$surfaceprop "flesh"

$model "civSoldier" "civSoldier_ref.smd"

$include "../animation/hierarchy.qci"
$include "..\animation\hitboxes.qci"
$include "ragdoll.qc"

$includemodel player/gov_player_shared.mdl