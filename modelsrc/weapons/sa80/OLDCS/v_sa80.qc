//This file was built off of the SG552 weapon from CS:S using the model decompiler version 0.4.1

$modelname "weapons\v_rifle_sa80.mdl"
$model "studio" "sa80_reference.smd"
$model "hands" "hands_reference.smd"
$cdmaterials "models\weapons\V_models\v_sa80\"
$cdmaterials "models\weapons\v_models\hands\"

//$attachment "1" "v_weapon.sg552_Parent" 0.20 4.00 13.00 rotate 0 0 0
$attachment "1" "v_weapon.sg552_Parent" 0.0 0.0 0.0 rotate 0 0 0
$attachment "2" "v_weapon.sg552_Parent" 0.00 4.80 4.60 rotate -5.00 165.00 -0.00
//$attachment "muzzle_flash" "v_weapon.sg552_Parent" 0.20 4.00 13.00 rotate 0 0 0

$surfaceprop "default"
$illumposition 11.744 4.617 -7.913
$sequence idle "idle" ACT_VM_IDLE 1 fps 30.00
$sequence reload "reload" ACT_VM_RELOAD 1 fps 37.00 {
  { event 5004 16 "Weapon_SG552.Clipout" }
  { event 5004 61 "Weapon_SG552.Clipin" }
  { event 5004 90 "Weapon_SG552.Boltpull" }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 37.00 {
  { event 5004 12 "Weapon_SG552.Boltpull" }
}

$sequence shoot1 "shoot1" ACT_VM_PRIMARYATTACK 1 fps 35.00 {
  { event 5001 0 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_556 2 90" }
}

//old event 5001

$sequence shoot2 "shoot2" ACT_VM_PRIMARYATTACK 1 fps 35.00 {
  { event 5001 0 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_556 2 90" }
}

$sequence shoot3 "shoot3" ACT_VM_PRIMARYATTACK 1 fps 35.00 {
  { event 5001 0 "1" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_556 2 90" }
}

