$modelname "weapons\v_knife_gov.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "knife_gov_reference.smd"
$body "leg" "leg_reference.smd"

$cdmaterials "models\weapons\v_models\v_knife_gov\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$surfaceprop "default"
$illumposition 8.418 -2.675 -4.874

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 { snap }
$sequence idle "idle" ACT_VM_IDLE 1 fps 10.00
$sequence slash1 "slash1" ACT_VM_PRIMARYATTACK 1 fps 75.00 { snap }
//$sequence slash2 "slash2" ACT_VM_PRIMARYATTACK 1 fps 75.00 { snap }
$sequence stab_hit "stab_hit" ACT_VM_SECONDARYATTACK 1 fps 65.00 { snap }
$sequence miss "slash1" ACT_VM_MISS 1 fps 75.00 { snap }
$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 26.00 loop