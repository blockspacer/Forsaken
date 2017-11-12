$modelname "weapons/w_special_rpg.mdl"
$cdmaterials "models\weapons\v_models\v_rpg\"
$cdmaterials "models\weapons\w_models\w_rpg\"

$model "w_special_rpg" "w_rpg_ref"

$lod 16
{
	replacemodel "w_rpg_ref" "w_rpg_lod_ref"
	replacematerial "v_rpg" "w_rpg"
}

//$attachment "muzzle" "flash" -0.00 0.00 -0.00 rotate 0 0 90
$attachment "muzzle" "r_hand" 5.70 3.00 17.00 rotate -87 0 0
$attachment "1" "flash" -37.00 0.00 0.00 rotate -0 -180 -90

$sequence idle "w_rpg_still" fps 1

$collisionmodel "w_rpg_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
