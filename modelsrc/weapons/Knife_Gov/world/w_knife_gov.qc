$modelname "weapons/w_knife_gov.mdl"
$cdmaterials "models\weapons\v_models\v_knife_gov\"
$cdmaterials "models\weapons\w_models\w_knife_gov\"

$model "w_knife_gov" "w_knife_gov_ref"

$lod 5
{
	replacemodel "w_knife_gov_ref" "w_knife_gov_lod_ref"
	replacematerial "v_knife_gov" "w_knife_gov"
}

$sequence idle "w_knife_gov_still" fps 1

$collisionmodel "w_knife_gov_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
