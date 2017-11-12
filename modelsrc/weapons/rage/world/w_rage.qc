$modelname "weapons/w_pist_ragingbull.mdl"
$cdmaterials "models\weapons\v_models\v_rage\"
$cdmaterials "models\weapons\w_models\w_rage\"

$model "w_pist_ragingbull" "w_rage_ref"

$lod 5
{
	replacemodel "w_rage_ref" "w_rage_lod_ref"
	replacematerial "v_rage" "w_rage"
}

//$attachment "muzzle" "muzzle_flash" 0.00 0.00 0.00 rotate 0 0 0
$attachment "muzzle" "R_Hand" 14.00 0.90 4.80 rotate -10 0 0
$attachment "1" "R_Hand" 6.00 0.80 3.00 rotate 90 0 0

$sequence idle "w_rage_still" fps 1

$collisionmodel "w_rage_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
