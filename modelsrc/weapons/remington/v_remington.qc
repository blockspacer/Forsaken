$modelname "weapons\v_rifle_remington.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "remington_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_remington\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "Muzzle" "Remington" 30.00 0.00 0.00 rotate 0 -3 0
$attachment "2" "Bullets" 1.30 0.50 0.00 rotate -90 -180 20

$surfaceprop "default"
$illumposition 7.731 4.148 -6.307

$sequence idle1 "idle" ACT_VM_IDLE 1 fps 5.00 loop

$sequence shoot "shoot1" ACT_VM_PRIMARYATTACK 1 fps 30.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "710 Muzzle" }
  { event 5004 21 "Weapon_Forsaken_remington.BoltBack" }
  { event 5004 27 "Weapon_Forsaken_remington.BoltForward" }
  { event AE_CLIENT_EFFECT_ATTACH 30 "EjectBrass_Remington 2 20" }
}
$sequence shoot2 "shoot1" ACT_VM_PRIMARYATTACK 1 fps 30.00 {
  { snap }
  { event AE_MUZZLEFLASH 0 "710 Muzzle" }
  { event 5004 21 "Weapon_Forsaken_remington.BoltBack" }
  { event 5004 27 "Weapon_Forsaken_remington.BoltForward" }
  { event AE_CLIENT_EFFECT_ATTACH 30 "EjectBrass_Remington 2 20" }
}

$sequence reload "reload" ACT_VM_RELOAD 1 fps 30.00 {
  { event 5004 16 "Weapon_Forsaken_remington.ClipOut" }
  { event 5004 49 "Weapon_Forsaken_remington.ClipIn" }
  { event 5004 76 "Weapon_Forsaken_remington.BoltBack" }
  { event 5004 84 "Weapon_Forsaken_remington.BoltForward" }
//  { event 5004 52 "Weapon_Scout.Bolt" }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 { 
  { snap }
  { event 5004 8 "Weapon_Forsaken_remington.Draw" }
}

$sequence toScope "toScope" ACT_VM_TOZOOM 1 fps 30.00 { snap }
$sequence fromScope "fromScope" ACT_VM_FROMZOOM 1 fps 30.00 { snap }

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 loop