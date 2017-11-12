$modelname "weapons\v_molotov.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "molotov_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_molotov\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$attachment "1" "Zippo" 1.9 0.0 0.0 rotate 0 0 0
$attachment "2" "Wick03" 0.6 0.0 0.0 rotate 0 0 0
$attachment "3" "Wick02" 0.6 0.0 0.0 rotate 0 0 0
$attachment "4" "Wick01" 0.6 0.0 0.0 rotate 0 0 0

$surfaceprop "default"
$illumposition 4.316 0.610 -7.384

$sequence Idle "idle" ACT_VM_IDLE 1 fps 5 loop

$sequence light "light" ACT_VM_PULLPIN 1 fps 30.00 {
  { snap }
  { event 5004 8 "Weapon_Forsaken_Molotov.Light" }
  { event 5004 45 "Weapon_Forsaken_Molotov.BurnIdle" }
  { event 5004 64 "Weapon_Forsaken_Molotov.Close" }
}

$sequence light2 "light" ACT_VM_PULLBACK_HIGH 1 fps 30.00 {
  { snap }
  { event 5004 8 "Weapon_Forsaken_Molotov.Light" }
  { event 5004 45 "Weapon_Forsaken_Molotov.BurnIdle" }
  { event 5004 64 "Weapon_Forsaken_Molotov.Close" }
}

$sequence light3 "light" ACT_VM_PULLBACK_LOW 1 fps 30.00 {
  { snap }
  { event 5004 8 "Weapon_Forsaken_Molotov.Light" }
  { event 5004 45 "Weapon_Forsaken_Molotov.BurnIdle" }
  { event 5004 64 "Weapon_Forsaken_Molotov.Close" }
}

$sequence light4 "light" ACT_VM_PULLBACK_HAULBACK 1 fps 30.00 {
  { snap }
  { event 5004 8 "Weapon_Forsaken_Molotov.Light" }
  { event 5004 45 "Weapon_Forsaken_Molotov.BurnIdle" }
  { event 5004 64 "Weapon_Forsaken_Molotov.Close" }
}

$sequence throw "throw" ACT_VM_THROW 1 fps 30.00 {
  { snap }
  { event 3005 4 }
}

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 {
  { snap }
  { event 5004 4 "Weapon_Forsaken_Molotov.Draw" }
}

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 loop