$modelname "weapons\v_pist_dualp99.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "p99s_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_p99\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

//Left Muzzle flash = 1
//Right Muzzle Flash = 2
//Right Ejector Port = 3
//Left Ejector Port = 4

$attachment "MuzzleR" "R_P99" 6.5 0.0 0.0 rotate 0 0 0
$attachment "MuzzleL" "L_P99" 6.5 0.0 0.0 rotate 0 0 0
$attachment "3" "R_P99" 3.4 0.0 0.0 rotate -90 -180 0
$attachment "4" "L_P99" 3.4 0.0 0.0 rotate -90 -180 0

$surfaceprop "default"

$illumposition 7.649 -0.566 -5.186

$sequence idle "idle" ACT_VM_IDLE 1 fps 5 loop

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 {
  { snap }
  { event 5004 0 "Weapon_Forsaken_dualp99.Draw" }
}

$sequence shoot_left1 "shoot_left" ACT_VM_PRIMARYATTACK 1 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualP99 MuzzleL" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_P99 4 30" }
}

$sequence shoot_left2 "shoot_left" ACT_VM_PRIMARYATTACK 2 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualP99 MuzzleL" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_P99 4 30" }
}

$sequence shoot_right1 "shoot_right" ACT_VM_SECONDARYATTACK 1 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualP99 MuzzleR" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_P99 3 30" }
}

$sequence shoot_right2 "shoot_right" ACT_VM_SECONDARYATTACK 2 fps 18.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "DualP99 MuzzleR" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_P99 3 30" }
}

$sequence reload "reload" ACT_VM_RELOAD 1 fps 37.00 {
  { event 5004 0 "Weapon_Forsaken_dualp99.SlideBack" }
  { event 5004 13 "Weapon_Forsaken_dualp99.Clipout" }
  { event 5004 16 "Weapon_Forsaken_dualp99.Clipout" }
  { event 5004 44 "Weapon_Forsaken_dualp99.ClipIn" }
  { event 5004 51 "Weapon_Forsaken_dualp99.SlideRelease" }
  { event 5004 74 "Weapon_Forsaken_dualp99.ClipIn" }
  { event 5004 80 "Weapon_Forsaken_dualp99.Sliderelease" }
}

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 snap loop

/*
$sequence idle_leftempty "idle_leftempty" ACT_VM_IDLE_EMPTY_LEFT 1 fps 16.00
$sequence shoot_left1 "shoot_left1" ACT_VM_PRIMARYATTACK 1 fps 18.00 {
  { event 5001 1 "2" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_9mm 4 90" }
}

$sequence shoot_left2 "shoot_left2" ACT_VM_PRIMARYATTACK 2 fps 18.00 {
  { event 5001 1 "2" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_9mm 4 90" }
}

$sequence shoot_leftlast "shoot_leftlast" ACT_VM_DRYFIRE_LEFT 1 fps 18.00 {
  { event 5001 1 "2" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_9mm 4 90" }
}

$sequence shoot_right1 "shoot_right1" ACT_VM_SECONDARYATTACK 1 fps 18.00 {
  { event 5001 1 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_9mm 3 90" }
}

$sequence shoot_right2 "shoot_right2" ACT_VM_SECONDARYATTACK 2 fps 18.00 {
  { event 5001 1 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_9mm 3 90" }
}

$sequence shoot_rightlast "shoot_rightlast" ACT_VM_DRYFIRE 1 fps 18.00 {
  { event 5001 1 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_9mm 3 90" }
}
*/