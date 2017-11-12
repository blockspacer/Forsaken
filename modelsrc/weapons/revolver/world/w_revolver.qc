$modelname "weapons/w_pist_revolver.mdl"
$cdmaterials "models\weapons\v_models\v_revolver\"

$model "w_pist_revolver" "w_revolver_ref"

//$attachment "muzzle" "muzzle_flash" 0.00 0.00 0.00 rotate 0 0 0
$attachment "muzzle" "R_Hand" 12.80 0.90 4.80 rotate -9 0 0
$attachment "1" "R_Hand" 6.00 0.80 3.00 rotate 90 0 0

$sequence idle "w_revolver_still" fps 1

$collisionmodel "w_revolver_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
