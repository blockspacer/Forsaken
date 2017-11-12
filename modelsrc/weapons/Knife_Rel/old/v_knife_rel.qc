$modelname "weapons\v_knife_rel.mdl"

$model "studio" "knife_rel_reference.smd"
$model "hands" "hands_reference.smd"

$cdmaterials "models\weapons\v_models\v_knife_rel\"
$cdmaterials "models\weapons\v_models\hands\"

$surfaceprop "default"
$illumposition 8.418 -2.675 -4.874

$sequence idle "idle" ACT_VM_IDLE 1 fps 12.00 rotate -90
$sequence draw "draw" ACT_VM_DRAW 1 fps 45.00 rotate -90
$sequence stab "stab" ACT_VM_SECONDARYATTACK 1 fps 35.00 rotate -90
$sequence stab_miss "stab_miss" ACT_VM_MISS 1 fps 40.00 rotate -90
$sequence midslash1 "midslash1" ACT_VM_PRIMARYATTACK 1 fps 55.00 rotate -90
$sequence midslash2 "midslash2" ACT_VM_PRIMARYATTACK 1 fps 55.00 rotate -90