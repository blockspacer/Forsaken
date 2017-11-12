$modelname props_forsaken\dynamic\indoor\desk.mdl
$cdmaterials models\props_forsaken\desk
//$staticprop
$keyvalues { "prop_data" {"base" "metal.medium" } 
$body studio "desk_ref.smd"

$sequence idle "desk_still" loop fps 1

$collisionmodel "desk_collis" {
   $concave
   $mass 800
}