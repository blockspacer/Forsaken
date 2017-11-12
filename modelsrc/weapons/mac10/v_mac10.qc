$modelname "weapons\v_smg_dualm10.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "mac10s_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\v_models\v_mac10"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "MuzzleR" "R_Mac10" 8.60 0.00 0.00 rotate 0 0 0
$attachment "MuzzleL" "L_Mac10" 8.60 0.00 0.00 rotate 0 0 0
$attachment "3" "R_Mac10" 3.20 0.30 0.00 rotate -90 -180 0
$attachment "4" "L_Mac10" 3.20 0.30 0.00 rotate -90 -180 0

$surfaceprop "default"

$illumposition 7.649 -0.566 -5.186

$sequence idle "idle" ACT_VM_IDLE 1 fps 5 loop

$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 {
  { event 5004 10 "Weapon_Forsaken_DualM10.ClipOut" }
  { event 5004 36 "Weapon_Forsaken_DualM10.ClipIn" }
  { event 5004 75 "Weapon_Forsaken_DualM10.ClipIn" }
  { event 5004 98 "Weapon_Forsaken_DualM10.Draw" }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 {
  { snap }
  { event 5004 1 "Weapon_Forsaken_DualM10.Draw" }
}

$sequence shoot_left1 "fire_left1" ACT_VM_PRIMARYATTACK 1 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualM10 MuzzleL" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 4 40" }

}

$sequence shoot_left2 "fire_left2" ACT_VM_PRIMARYATTACK 2 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualM10 MuzzleL" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 4 30" }
}

$sequence shoot_left3 "fire_left3" ACT_VM_PRIMARYATTACK 3 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualM10 MuzzleL" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 4 45" }
}

$sequence shoot_right1 "fire_right1" ACT_VM_SECONDARYATTACK 1 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualM10 MuzzleR" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 3 40" }
}

$sequence shoot_right2 "fire_right2" ACT_VM_SECONDARYATTACK 2 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualM10 MuzzleR" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 3 30" }
}

$sequence shoot_right3 "fire_right3" ACT_VM_SECONDARYATTACK 3 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualM10 MuzzleR" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 3 45" }
}

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 snap loop

/*
$sequence idle_leftempty "idle_leftempty" ACT_VM_IDLE_EMPTY_LEFT 1 fps 16.00

$sequence shoot_leftlast "shoot_leftlast" ACT_VM_DRYFIRE_LEFT 1 fps 18.00 {
  { event 5001 1 "2" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 4 90" }
}

$sequence shoot_rightlast "shoot_rightlast" ACT_VM_DRYFIRE 1 fps 18.00 {
  { event 5001 1 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_MAC10 3 90" }
}

}*/