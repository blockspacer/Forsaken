//==============================================//
//	Veteran player model     		//
//	Writen by Sean McBride  		//
//	For Brawl Player Contest		//
//==============================================//

// the veteran_ref.smd file that I have submitted is the smd I used to create the prop in game to
// look at while i was working on him. Since I sadly won't have time to rig him you guys will have
// to export this file once he is fully rigged.

$modelname "player/veteran/veteran.mdl"
$cdmaterials "models/player/veteran"

$surfaceprop "flesh"

$model "veteran" "veteran_ref.smd"

// Change these paths to wherever your shared qci files are... These are files that I shouldn't write
// because I don't know what values you are using for the other player models. Uncomment the lines when
// the paths are correct.

//$include "../animation/hierarchy.qci"
//$include "../animation/hitboxes.qci"

$include "ragdoll.qc"

// Same as above comment.
//$includemodel player/shared_player_anims.mdl