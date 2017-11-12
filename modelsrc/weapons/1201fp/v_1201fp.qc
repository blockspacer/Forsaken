$modelname "weapons\v_shot_1201fp.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "1201fp_reference.smd"
$body "leg" "leg_reference"

$cdmaterials "models\weapons\v_models\v_1201fp\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "Muzzle" "1201fp" 21.00 0.00 0.00 rotate 0 0 0
$attachment "1" "R_Hand" 11.00 0.50 3.40 rotate -15 -180 0

$surfaceprop "default"

$sequence idle "idle" ACT_VM_IDLE 1 fps 5.00 loop

$sequence shoot1 "shoot1" fps 30 snap activity ACT_VM_PRIMARYATTACK 1 {
  { event AE_MUZZLEFLASH 0 "1201FP Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 1 "EjectBrass_1201FP 2 20" }
}

$sequence shoot2 "shoot1" fps 30 snap activity ACT_VM_RECOIL1 1 {
  { event AE_MUZZLEFLASH 0 "1201FP Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 1 "EjectBrass_1201FP 2 20" }
}

$sequence shoot3 "shoot1" fps 30 snap activity ACT_VM_RECOIL2 1 {
  { event AE_MUZZLEFLASH 0 "1201FP Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 1 "EjectBrass_1201FP 2 20" }
}

$sequence shoot4 "shoot1" fps 30 snap activity ACT_VM_PRIMARYATTACK 1 {
  { event AE_MUZZLEFLASH 0 "1201FP Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 1 "EjectBrass_1201FP 2 20" }
}

$sequence insert "insert" fps 30 snap activity ACT_VM_RELOAD 1 {
  { event 5004 6 "Weapon_Forsaken_1201FP.Insert" }
}

$sequence toReload "toReload" ACT_SHOTGUN_RELOAD_START 1

$sequence fromReload "fromReload" ACT_SHOTGUN_RELOAD_FINISH 1 {
  { event 5004 0 "Weapon_Forsaken_1201FP.Draw" }
}

$sequence draw "draw" ACT_VM_DRAW 1 {
  { snap }
  { event 5004 8 "Weapon_Forsaken_1201FP.Draw" }
}

$sequence holster "holster" ACT_VM_HOLSTER 1 {
  { snap }
  { event 5004 0 "Weapon_Forsaken_1201FP.Draw" }
}

$sequence melee "melee" ACT_VM_SECONDARYATTACK 1 {
  { snap }
//  { event 5004 8 "Weapon_Forsaken_1201FP.Draw" }
}

$sequence miss "melee" ACT_VM_MISS 1 {
  { snap }
//  { event 5004 8 "Weapon_Forsaken_1201FP.Draw" }
}

$sequence kick "kick" ACT_VM_KICK 1 fps 40.00

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 25.00 loop
