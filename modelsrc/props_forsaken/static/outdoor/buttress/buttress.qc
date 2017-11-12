$modelname props_forsaken\static\outdoor\buttress.mdl

$cdmaterials models\props_forsaken\buttress
$staticprop

$body studio "buttress_ref.smd"

$sequence idle "buttress_still" fps 1

$collisionmodel "buttress_phys.smd" {
     $Mass 10000
     $concave
}