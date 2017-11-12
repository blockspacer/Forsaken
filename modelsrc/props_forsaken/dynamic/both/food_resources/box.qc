$modelname props_forsaken\dynamic\both\food_resources\box.mdl
$cdmaterials models\props_forsaken\food_resources

$keyvalues { "prop_data" {"base" "Cardboard.Medium" } 
$body studio "box_ref.smd"

$sequence idle "box_still" fps 1

$collisionmodel "box_phys" {
   $concave
   $mass 20
}