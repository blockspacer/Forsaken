// Output .MDL
//$modelname "weapons/w_rifle_sa80.mdl"
$modelname "weapons/w_rif_m4a1.mdl"

// Directory of .TGA
$cdmaterials "models\weapons\v_models\v_sa80\"

$model "w_rifle_sa80" "w_sa80_ref"

//$attachment "muzzle_flash" "R_Hand" 29.00 0.00 2.00 rotate 0 0 0

$attachment "muzzle_flash" "R_Hand" 29.00 0.00 2.00 rotate 0 0 0
$attachment "muzzle_flash" "flash" 0.00 0.00 0.00 rotate 0 0 0

$sequence idle "w_SA80_still" fps 1

$collisionmodel "w_sa80_ref.smd" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
