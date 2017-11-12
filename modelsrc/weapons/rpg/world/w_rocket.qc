$modelname "weapons/w_rocket.mdl"
$cdmaterials "models\weapons\v_models\v_rpg\"
$cdmaterials "models\weapons\w_models\w_rpg\"

$model "w_rocket" "w_rocket_ref"

$lod 5
{
	replacemodel "w_rocket_ref" "w_rocket_lod_ref"
	replacematerial "v_rpg_rocket" "w_rpg_rocket"
}

$attachment "1" "Smoke" 0 0 0

$sequence idle "w_rocket_still" fps 1

$collisionmodel "w_rocket_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}