$modelname "weapons\v_knife_rel.mdl"

$body "hands" "hands_reference.smd"
$body "studio" "knife_rel_reference.smd"
$body "leg" "leg_reference.smd"

$texturegroup skinfamilies
{
   { "Hand_Glove.vmt" }
   { "Hand_No_Glove.vmt" }
   { "Hand_Rel.vmt" }
}

$cdmaterials "models\weapons\v_models\v_knife_rel\"
$cdmaterials "models\weapons\v_models\hands\"
$cdmaterials "models\player\GovSoldier"

$surfaceprop "default"
$illumposition 8.418 -2.675 -4.874

$sequence draw "draw" ACT_VM_DRAW 1 fps 30.00 { snap }
$sequence idle "idle" ACT_VM_IDLE 1 fps 12.00

$sequence midslash1 "slash" ACT_VM_PRIMARYATTACK 1 fps 50.00 { snap }
$sequence midslash2 "slash" ACT_VM_PRIMARYATTACK 1 fps 50.00 { snap }
$sequence stab1 "stab_hit" ACT_VM_SECONDARYATTACK 1 fps 45.00 { snap }
$sequence stab2 "stab2_hit" ACT_VM_SECONDARYATTACK 1 fps 45.00 { snap }
$sequence miss "slash" ACT_VM_MISS 1 fps 45.00 { snap }
$sequence sprint "sprint" ACT_VM_SPRINT 1 fps 26.00 loop