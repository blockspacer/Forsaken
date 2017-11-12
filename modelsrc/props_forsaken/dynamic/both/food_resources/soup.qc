$modelname props_forsaken\dynamic\both\food_resources\soup.mdl
$cdmaterials models\props_forsaken\food_resources

$keyvalues { "prop_data" {"base" "Metal.Medium" } 
$body studio "soup_ref.smd"

$sequence idle "soup_still" fps 1

$collisionmodel "soup_phys" {
   $concave
   $mass 4
}