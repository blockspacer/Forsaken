$modelname "weapons\v_shot_1201fp.mdl"

$body "studio" "1201fp_ref.smd"
$body "hands" "hands_reference.smd"

$cdmaterials "models\weapons\v_models\v_1201fp\"
$cdmaterials "models\weapons\v_models\hands\"

$attachment "1" "v_weapon.M3_PARENT" 0.00 3.00 18.00 rotate 0.00 0.00 0.00
$attachment "2" "v_weapon.M3_PARENT" -0.50 3.50 5.00 rotate -35.00 180.00 0.00

$surfaceprop "default"
$sequence idle "idle" ACT_VM_IDLE 1 rotate -90 
$sequence shoot1 "shoot1" ACT_VM_PRIMARYATTACK 1 rotate -90 { event 5001 1 7 }
$sequence shoot2 "shoot2" ACT_VM_PRIMARYATTACK 1 rotate -90 
$sequence insert "insert" ACT_VM_RELOAD 1 rotate -90 
$sequence after_reload "after_reload" ACT_SHOTGUN_RELOAD_FINISH 1 rotate -90 
$sequence start_reload "start_reload" ACT_SHOTGUN_RELOAD_START 1 rotate -90 
$sequence draw "draw" ACT_VM_DRAW 1 rotate -90 