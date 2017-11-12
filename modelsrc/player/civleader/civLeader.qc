//==============================================//
//	Civilian Leader player model		//
//	Writen by Sean McBride 3/1/05		//
//	For Forsaken. www.forsaken-game.com	//
//==============================================//

$modelname "player/civLeader/civLeader.mdl"
$cdmaterials "models/player/civLeader"

//$scale 1.14

$surfaceprop "flesh"

$model "civLeader" "civLeader_ref.smd" {

  flexfile "Aim.vta"
  flex "Aim" frame 10
  flexcontroller "FacialExp" "Aim" "range" 0 1

  flexfile "Blink.vta"
  flex "Blink" frame 10
  flexcontroller "FacialExp" "Blink" "range" 0 1

  flexfile "Dead.vta"
  flex "Dead" frame 10
  flexcontroller "FacialExp" "Dead" "range" 0 1

  flexfile "Evil.vta"
  flex "Evil" frame 10
  flexcontroller "FacialExp" "Evil" "range" 0 1

  flexfile "Fire.vta"
  flex "Fire" frame 10
  flexcontroller "FacialExp" "Fire" "range" 0 1

  flexfile "Mad.vta"
  flex "Mad" frame 10
  flexcontroller "FacialExp" "Mad" "range" 0 1

  flexfile "Pain.vta"
  flex "Pain" frame 10
  flexcontroller "FacialExp" "Pain" "range" 0 1

  flexfile "Sad.vta"
  flex "Sad" frame 10
  flexcontroller "FacialExp" "Sad" "range" 0 1

  flexfile "Worried.vta"
  flex "Worried" frame 10
  flexcontroller "FacialExp" "Worried" "range" 0 1

  %Aim = Aim
  %Blink = Blink
  %Dead = Dead
  %Evil = Evil
  %Fire = Fire
  %Mad = Mad
  %Pain = Pain
  %Sad = Sad
  %Worried = Worried
}

$include "../animation/hierarchy.qci"
$include "../animation/hitboxes.qci"
$include "ragdoll.qc"

$includemodel player/gov_player_shared.mdl