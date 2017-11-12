$modelname props_forsaken\dynamic\both\food_resources\bottle.mdl
$cdmaterials models\props_forsaken\food_resources

$keyvalues { "prop_data" {"base" "Plastic.Small" } 
$body studio "bottle_ref.smd"

$sequence idle "bottle_still" fps 1

$collisionmodel "bottle_phys" {
   $concave
   $mass 2
}