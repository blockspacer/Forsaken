$modelname props_forsaken/static/both/food_resources.mdl
$cdmaterials models/props_forsaken/food_resources

$staticprop
$surfaceprop "metal"

$body studio "food_resources_ref"

$sequence idle "food_resources_still" fps 1

$collisionmodel "food_resources_phys" {
	$Mass 100
	$concave
}