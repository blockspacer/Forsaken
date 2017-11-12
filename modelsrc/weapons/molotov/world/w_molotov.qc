$modelname "weapons/w_molotov.mdl"
$cdmaterials "models\weapons\v_models\v_molotov\"
$cdmaterials "models\weapons\w_models\w_molotov\"

$model "w_molotov" "w_molotov_ref"

$lod 5
{
	replacemodel "w_molotov_ref" "w_molotov_lod_ref"
	replacematerial "v_molotov_nontrans" "w_molotov"
}

$sequence idle "w_molotov_still" fps 1

$collisionmodel "w_molotov_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}