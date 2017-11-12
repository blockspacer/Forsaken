$modelname "weapons/w_rifle_sa80.mdl"
$cdmaterials "models\weapons\v_models\v_sa80\"
$cdmaterials "models\weapons\w_models\w_sa80\"

$model "w_rifle_sa80" "w_sa80_ref"

$lod 10
{
	replacemodel "w_sa80_ref" "w_sa80_lod_ref"
	replacematerial "v_sa80" "w_sa80"
}

//$attachment "muzzle" "flash" 0.00 0.00 0.00 rotate 0 0 0
$attachment "muzzle" "R_Hand" 22.00 5.20 3.50 rotate 1 10 0
$attachment "1" "R_Hand" -2.00 -0.40 5.00 rotate 0 -90 90

$sequence idle "w_SA80_still" fps 1
$collisionmodel "w_sa80_phys.smd" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}