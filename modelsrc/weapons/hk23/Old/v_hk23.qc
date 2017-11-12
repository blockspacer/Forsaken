$modelname "weapons\v_pist_hk23.mdl"

$body "studio" "hk23_ref.smd"
$body "hands" "old_hands_reference.smd"

$cdmaterials "models\weapons\V_models\v_hk23\"
$cdmaterials "models\weapons\v_models\hands\"

$attachment "1" "v_weapon.p228_Parent" 0.00 3.00 5.60 rotate 0 0 0
$attachment "2" "v_weapon.p228_Parent" 0.00 3.10 1.00 rotate -10.00 -180.00 0.00

$surfaceprop "default"
$sequence idle "idle" ACT_VM_IDLE 1 rotate -90 
$sequence shoot1 "shoot1" ACT_VM_PRIMARYATTACK 1 rotate -90 { event 5001 1 7 }
$sequence shoot2 "shoot2" ACT_VM_PRIMARYATTACK 1 rotate -90 
$sequence shoot3 "shoot3" ACT_VM_PRIMARYATTACK 1 rotate -90 
$sequence shoot_empty "shoot_empty" ACT_VM_DRYFIRE 1 rotate -90 
$sequence reload "reload" ACT_VM_RELOAD 1 rotate -90 
$sequence draw "draw" ACT_VM_DRAW 1 rotate -90 