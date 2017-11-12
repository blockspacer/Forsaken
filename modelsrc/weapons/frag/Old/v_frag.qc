$modelname "weapons\v_fraggrenade.mdl"

$model "studio" "frag_reference.smd"
$model "hands" "hands_reference.smd"

$cdmaterials "models\weapons\V_models\v_frag\"
$cdmaterials "models\weapons\v_models\hands\"


$surfaceprop "default"
$illumposition 4.316 0.610 -7.384
$sequence idle "idle" ACT_VM_IDLE 1 fps 30.00 rotate -90 

$sequence pullpin "pullpin" ACT_VM_PULLPIN 1 fps 41.00 rotate -90 {
  { event 5004 27 "Default.PullPin_Grenade" }
}

$sequence pullpin2 "pullpin2" ACT_VM_PULLBACK_HIGH 1 fps 41.00 rotate -90 {
  { event 5004 27 "Default.PullPin_Grenade" }
}

$sequence pullpin3 "pullpin3" ACT_VM_PULLBACK_LOW 1 fps 41.00 rotate -90 {
  { event 5004 27 "Default.PullPin_Grenade" }
}

$sequence pullpin4 "pullpin4" ACT_VM_PULLBACK_HAULBACK 1 fps 41.00 rotate -90 {
  { event 5004 27 "Default.PullPin_Grenade" }
}

$sequence throw "throw" ACT_VM_THROW 1 fps 30.00 rotate -90 {
  { event 3005 10 }
}

$sequence deploy "deploy" ACT_VM_DRAW 1 fps 30.00 rotate -90 
