$modelname "weapons\v_fraggrenade.mdl"

$body "studio" "frag_reference.smd"
$body "hands" "hands_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_frag\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$surfaceprop "default"

$illumposition 4.316 0.610 -7.384

$sequence idle "idle" ACT_VM_IDLE 1 fps 5 loop

$sequence draw "draw" ACT_VM_DRAW 1 {
  { snap }
  { event 5004 6 "Weapon_Forsaken_Frag.Draw" }  
}

$sequence pullpin "pullpin" ACT_VM_PULLPIN 1 fps 30.00 {
  { event 5004 14 "Weapon_Forsaken_Frag.PullPin" }
}

$sequence pullpin2 "pullpin" ACT_VM_PULLBACK_HIGH 1 fps 30.00 {
  { event 5004 14 "Weapon_Forsaken_Frag.PullPin" }
}

$sequence pullpin3 "pullpinSpoon" ACT_VM_PULLBACK_LOW 1 fps 30.00 {
  { event 5004 14 "Weapon_Forsaken_Frag.PullPin" }
}

$sequence pullpin4 "pullpinSpoon" ACT_VM_PULLBACK_HAULBACK 1 fps 30.00 {
  { event 5004 14 "Weapon_Forsaken_Frag.PullPin" }
}

$sequence throw "throw" ACT_VM_THROW 1 fps 30.00 {
  { event 3005 2 }
}

$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 26.00 loop
