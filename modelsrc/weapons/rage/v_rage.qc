$modelname "weapons\v_pist_ragingbull.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "ragingbull_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_rage\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "Muzzle" "RagingBull" 13 0 0 rotate 0 -22 0
$attachment "2" "R_Arm" 2 -4 0 rotate 90 -180 0

$surfaceprop "default"

$illumposition 5.962 3.334 -8.123

$sequence idle "idle" ACT_VM_IDLE 1 fps 5.00 loop

//$sequence shoot1 "shoot" ACT_VM_PRIMARYATTACK 1 fps 40.00 {
$sequence shoot1 "shoot" ACT_VM_PRIMARYATTACK 1 {
	{ event AE_MUZZLEFLASH 0 "RagingBull Muzzle" }
	{ snap }
}

//$sequence shoot2 "shoot" ACT_VM_RECOIL1 1 fps 40.00 {
//	{ snap }
//	{ event AE_MUZZLEFLASH 0 "RagingBull Muzzle" }
//}

$sequence shoot_empty "shootempty" ACT_VM_DRYFIRE 1 fps 40.00 {
	{ snap }
	{ event 5001 0 "21" }
}

$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 {
	{ snap }
	{ event 5004 10 "Weapon_Forsaken_ragingbull.OpenSpindle" }
	{ event 5004 18 "Weapon_Forsaken_ragingbull.Eject" }
	{ event AE_CLIENT_EFFECT_ATTACH 23 "EjectBrass_RagingBull 2 0" }
	{ event AE_CLIENT_EFFECT_ATTACH 23 "EjectBrass_RagingBull 2 0" }
	{ event AE_CLIENT_EFFECT_ATTACH 23 "EjectBrass_RagingBull 2 0" }
	{ event AE_CLIENT_EFFECT_ATTACH 23 "EjectBrass_RagingBull 2 0" }
	{ event AE_CLIENT_EFFECT_ATTACH 23 "EjectBrass_RagingBull 2 0" }
	{ event 5004 60 "Weapon_Forsaken_ragingbull.Insert" }
	{ event 5004 88 "Weapon_Forsaken_ragingbull.Insert" }
	{ event 5004 114 "Weapon_Forsaken_ragingbull.Insert" }
	{ event 5004 133 "Weapon_Forsaken_ragingbull.Insert" }
	{ event 5004 151 "Weapon_Forsaken_ragingbull.Insert" }
	{ event 5004 164 "Weapon_Forsaken_ragingbull.CloseSpindle" }

}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 {
	{ snap }
	{ event 5004 1 "Weapon_Forsaken_ragingbull.Draw" }
}

$sequence melee "melee" ACT_VM_SECONDARYATTACK 1 fps 30.00 { 
  { snap }
}

$sequence miss "melee" ACT_VM_MISS 1 fps 30.00 { 
  { snap }
}

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 loop