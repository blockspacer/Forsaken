$modelname "weapons/w_pist_hk23.mdl"
$cdmaterials "models\weapons\v_models\v_hk23\"
$cdmaterials "models\weapons\w_models\w_hk23\"

$model "w_pist_hk23" "w_hk23_ref"

$lod 8
{
	replacemodel "w_hk23_ref" "w_hk23_lod_ref"
	replacematerial "v_hk23" "w_hk23"
}

//$attachment "muzzle" "muzzle_flash" 0.00 0.00 0.00 rotate 0 0 0
$attachment "muzzle" "R_Hand" 11.60 1.04 3.00 rotate -4 2 0
$attachment "1" "R_Hand" 7.00 0.90 2.70 rotate -10 -90 0

$sequence idle "w_hk23_still" fps 1

$collisionmodel "w_hk23_phys.smd" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
