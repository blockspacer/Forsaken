$modelname "weapons\v_molotov.mdl"

$model "studio" "v_molotov_ref.smd"
$model "hands" "hands_reference.smd"

$cdmaterials "models\weapons\V_models\v_molotov\"
$cdmaterials "models\weapons\v_models\hands\"

$surfaceprop "default"
$illumposition 4.316 0.610 -7.384

/*
$sequence Idle "v_molotov_idle" ACT_VM_IDLE 1 fps 30.00 rotate -90 

$sequence Light "v_molotov_light" ACT_VM_PULLPIN 1 fps 41.00 rotate -90 {
  { event 5004 15 "Default.Light_Molotov" }
}

$sequence Throw "v_molotov_throw" ACT_VM_THROW 1 fps 30.00 rotate -90 {
  { event 3005 10 }
}*/

$sequence Idle "v_molotov_idle" ACT_VM_IDLE 1 fps 30.00 rotate -90 

$sequence pullpin "v_molotov_light" ACT_VM_PULLPIN 1 fps 41.00 rotate -90 {
  { event 5004 15 "Default.PullPin_Grenade" }
}

$sequence pullpin2 "v_molotov_light" ACT_VM_PULLBACK_HIGH 1 fps 41.00 rotate -90 {
  { event 5004 15 "Default.PullPin_Grenade" }
}

$sequence pullpin3 "v_molotov_light" ACT_VM_PULLBACK_LOW 1 fps 41.00 rotate -90 {
  { event 5004 15 "Default.PullPin_Grenade" }
}

$sequence pullpin4 "v_molotov_light" ACT_VM_PULLBACK_HAULBACK 1 fps 41.00 rotate -90 {
  { event 5004 15 "Default.PullPin_Grenade" }
}

$sequence throw "v_molotov_throw" ACT_VM_THROW 1 fps 30.00 rotate -90 {
  { event 3005 10 }
}

//$sequence deploy "deploy" ACT_VM_DRAW 1 fps 30.00 rotate -90 