$modelname "weapons/w_pist_dualp99.mdl"
$cdmaterials "models\weapons\v_models\v_p99\"

$model "w_pist_dualp99" "w_dualp99_ref"

//$attachment "Muzzle_L" "muzzle_flash_l" 0.0 0.00 0.00 rotate 0 0 0
//$attachment "Muzzle_R" "muzzle_flash_r" 0.0 0.00 0.00 rotate 0 0 0
$attachment "muzzle_l" "L_Hand" 9.60 0.84 -2.70 rotate -2 0 0
$attachment "muzzle_r" "R_Hand" 9.60 0.88 2.70 rotate 2 0 0
$attachment "3" "eport_l" 0.0 0.00 0.00 rotate 0 0 0
$attachment "4" "eport_r" 0.0 0.00 0.00 rotate 0 0 0

$sequence idle "w_dualp99_still" fps 1

$collisionmodel "w_dualp99_ref" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
