$modelname "weapons\v_rifle_sa80.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "sa80_ref.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_sa80\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "Muzzle" "SA80_Parent" 28.10 -0.00 0.00 rotate 0 0 0
$attachment "2" "SA80_Parent" 6 0.4 0 rotate -85 -15 180

$surfaceprop "default"
$illumposition 11.744 4.617 -7.913

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 {
  { snap }
  { event 5004 10 "Weapon_Forsaken_SA80.Draw" }
}

$sequence idle "idle" ACT_VM_IDLE 1 fps 5.00 loop

$sequence reload "reload_matt" ACT_VM_RELOAD 1 fps 37.00 {
  { event 5004 0 "Weapon_Forsaken_SA80.Slide" }
  { event 5004 30 "Weapon_Forsaken_SA80.ClipOut" }
  { event 5004 55 "Weapon_Forsaken_SA80.ClipIn" }
}

$sequence shoot1 "shoot1" ACT_VM_PRIMARYATTACK 1 fps 45.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "SA80 Muzzle" }
  { event AE_CLIENT_EFFECT_ATTACH 0 "EjectBrass_SA80 2 45" }
}

$sequence toScope "toScope" ACT_VM_TOZOOM 1 fps 30.00 { snap }
$sequence fromScope "fromScope" ACT_VM_FROMZOOM 1 fps 30.00 { snap }

$sequence kick "kick" ACT_VM_KICK 1 fps 40.00
$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 loop