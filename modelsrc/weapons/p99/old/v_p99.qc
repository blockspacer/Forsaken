$modelname "weapons\v_pist_dualp99.mdl"

$body "studio" "p99_reference.smd"
$body "hands" "hands_reference.smd"

$cdmaterials "models\weapons\v_models\v_p99"
$cdmaterials "models\weapons\v_models\hands\"

$attachment "1" "v_weapon.muzzleflash_right" -0.00 0.00 -0.00 rotate 0.00 -0.00 -0.00
$attachment "2" "v_weapon.muzzleflash_left" 0.00 0.00 0.00 rotate 0.00 -0.00 -0.00
$attachment "3" "v_weapon.shelleject_right" -0.00 0.00 -0.00 rotate 0.00 0.00 0.00
$attachment "4" "v_weapon.shelleject_left" -0.00 0.00 0.00 rotate 0.00 0.00 0.00

$surfaceprop "default"

$illumposition 7.649 -0.566 -5.186

$sequence idle "idle" ACT_VM_IDLE 1 fps 16.00
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
  { event 10001 1 "1" }
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

//5001 Old
//10001 New

$sequence reload "reload" ACT_VM_RELOAD 1 fps 25.00 {
  { event 5004 0 "Weapon_ELITE.Reloadstart" }
  { event 5004 12 "Weapon_ELITE.Clipout" }
  { event 5004 36 "Weapon_ELITE.Rclipin" }
  { event 5004 59 "Weapon_ELITE.Lclipin" }
  { event 5004 81 "Weapon_ELITE.Sliderelease" }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 {
  { event 5004 1 "Weapon_ELITE.Deploy" }
}