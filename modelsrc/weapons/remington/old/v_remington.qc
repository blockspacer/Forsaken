$modelname "weapons\v_rifle_remington.mdl"

$model "studio" "remington_reference.smd"
$model "hands" "hands_reference.smd"

$cdmaterials "models\weapons\V_models\v_remington\"
$cdmaterials "models\weapons\v_models\hands\"

$attachment "1" "v_weapon.scout_Parent" 0.00 2.45 21.00 rotate 0.00 0.00 0.00
$attachment "2" "v_weapon.scout_Parent" 0.00 2.85 4.00 rotate 20.00 180.00 0.00

$surfaceprop "default"
$illumposition 7.731 4.148 -6.307

$sequence idle1 "idle1" ACT_VM_IDLE 1 fps 16.00

$sequence shoot "shoot" ACT_VM_PRIMARYATTACK 1 fps 35.00 {
  { event 5001 0 "1" }
  { event 5004 13 "Weapon_Scout.Bolt" }
  { event AE_CLIENT_EFFECT_ATTACH 20 "EjectBrass_762Nato 2 40" }
}

$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 {
  { event 5004 13 "Weapon_Scout.Clipout" }
  { event 5004 37 "Weapon_Scout.Clipin" }
  { event 5004 52 "Weapon_Scout.Bolt" }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00
