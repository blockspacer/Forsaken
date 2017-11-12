$modelname "weapons\v_rpg.mdl"

$model "studio" "v_rpg_ref.smd"
$body "hands" "hands_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\V_models\v_rpg\"
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

$sequence Idle "v_rpg_idle" ACT_VM_IDLE 1 fps 30.00 { snap }
$sequence Fire "v_rpg_fire" ACT_VM_PRIMARYATTACK 1 fps 30.00 { snap }
$sequence Reload "v_rpg_reload" ACT_VM_RELOAD 1 fps 30.00 {
  { event 5004 0 "Weapon_Forsaken_RPG.Reload" }
  { snap }
}
$sequence Draw "v_rpg_draw" ACT_VM_DRAW 1 fps 30.00 {
  { snap }
  { event 5004 0 "Weapon_Forsaken_RPG.Draw" }
}

//$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 30.00 loop