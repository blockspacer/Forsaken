$modelname props_forsaken\dynamic\both\food_resources\tuna.mdl
$cdmaterials models\props_forsaken\food_resources

$keyvalues { "prop_data" {"base" "Metal.Medium" } 
$body studio "tuna_ref.smd"

$sequence idle "tuna_still" fps 1

$collisionmodel "tuna_phys" {
   $concave
   $mass 3
}