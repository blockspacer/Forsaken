//==============================================//
//	Governor Soldier player model		//
//	Writen by Sean McBride 3/1/05		//
//	For Forsaken. www.forsaken-game.com	//
//==============================================//

$modelname "player/govSoldier/govSoldier.mdl" 
$cdmaterials "models/player/govSoldier"

//$scale 1.14

$surfaceprop "flesh"

$model "govSoldier" "GovSoldier_Ref.smd"

$include "../animation/hierarchy.qci"
$include "../animation/hitboxes.qci"
$include "ragdoll.qc"

$includemodel player/gov_player_shared.mdl