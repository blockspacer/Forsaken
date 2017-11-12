$modelname props_forsaken\dynamic\both\food_resources\pallet.mdl
$cdmaterials models\props_forsaken\food_resources

$keyvalues { "prop_data" {"base" "Wooden.Large" } 
$body studio "pallet_ref.smd"

$sequence idle "pallet_still" fps 1

$collisionmodel "pallet_phys" {
   $concave
   $mass 50
}