$modelname "weapons\v_pist_hk23.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "hk23_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_hk23\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "Muzzle" "HK23" 8 0 0 rotate 0 0 0
$attachment "2" "HK23" 3.4 0 0 rotate -90 -180 0

$surfaceprop "default"

$sequence idle "idle" ACT_VM_IDLE 1 fps 5.00 loop

//$sequence idle_empty "idle_empty" ACT_VM_IDLE_EMPTY 1

$sequence draw "draw" ACT_VM_DRAW 1 {
  { snap }
  { event 5004 0 "Weapon_Forsaken_HK23.Draw" }
}

$sequence draw_empty "draw_empty" ACT_VM_DRAW_EMPTY 1 { snap }

$sequence shoot1 "shoot1" ACT_VM_PRIMARYATTACK 1 {
  { snap }
  { event AE_MUZZLEFLASH 0 "HK23 Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 1 "EjectBrass_HK23 2 30" }
}

$sequence shoot2 "shoot2" ACT_VM_PRIMARYATTACK 2 {
  { snap }
  { event AE_MUZZLEFLASH 0 "HK23 Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 1 "EjectBrass_HK23 2 30" }
}

$sequence melee "melee" ACT_VM_SECONDARYATTACK 1 {
  { snap }
}

$sequence miss "melee" ACT_VM_MISS 1 {
  { snap }
}

$sequence shootlast "shoot_last" ACT_VM_DRYFIRE 1 { event 5001 1 7 snap }

$sequence reload "reload" ACT_VM_RELOAD 1 {
  { event 5004 0 "Weapon_Forsaken_HK23.SlideBack" }
  { event 5004 11 "Weapon_Forsaken_HK23.ClipOut" }
  { event 5004 36 "Weapon_Forsaken_HK23.ClipIn" }
  { event 5004 41 "Weapon_Forsaken_HK23.SlideRelease" }
}

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 loop