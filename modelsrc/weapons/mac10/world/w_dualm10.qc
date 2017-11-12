$modelname "weapons/w_smg_dualm10.mdl"
$cdmaterials "models\weapons\v_models\v_mac10\"

$model "w_smg_dualm10" "v_dualm10_ref"

//$attachment "Muzzle_L" "muzzle_flash_l" 0.0 0.00 0.00 rotate 0 0 0
//$attachment "Muzzle_R" "muzzle_flash_r" 0.0 0.00 0.00 rotate 0 0 0
$attachment "muzzle_l" "L_Hand" 11.40 0.80 -3.24 rotate -2 0 0
$attachment "muzzle_r" "R_Hand" 11.40 0.80 3.24 rotate 3 0 0
$attachment "3" "eport_l" 0.0 0.00 0.00 rotate 0 0 0
$attachment "4" "eport_r" 0.0 0.00 0.00 rotate 0 0 0

$sequence idle "v_dualm10_still" fps 1

//$collisionmodel "V_dualm10_phys" {
$collisionmodel "V_dualm10_ref" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
