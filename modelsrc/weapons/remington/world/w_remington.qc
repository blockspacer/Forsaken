$modelname "weapons/w_rifle_remington.mdl"
$cdmaterials "models\weapons\v_models\v_remington\"
$cdmaterials "models\weapons\w_models\w_remington\"

$model "w_rifle_remington" "w_remington_ref"

$lod 5
{
	replacemodel "w_remington_ref" "w_remington_lod_ref"
	replacematerial "v_remington" "w_remington"
}

//$attachment "muzzle" "muzzle_flash" 0.00 0.00 0.00 rotate 0 0 0
$attachment "muzzle" "R_Hand" 31.50 6.67 10.40 rotate -16 10 -3
$attachment "1" "R_Hand" 8.00 2.10 4.40 rotate 0 -81 15

$sequence idle "w_remington_still" fps 1

$collisionmodel "w_remington_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
