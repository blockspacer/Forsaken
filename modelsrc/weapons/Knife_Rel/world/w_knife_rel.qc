$modelname "weapons/w_knife_rel.mdl"
$cdmaterials "models\weapons\v_models\v_knife_rel\"
$cdmaterials "models\weapons\w_models\w_knife_rel\"

$model "w_knife_rel" "w_knife_rel_ref"

$lod 5
{
	replacemodel "w_knife_rel_ref" "w_knife_rel_lod_ref"
	replacematerial "v_knife_rel" "w_knife_rel"
}

$sequence idle "w_knife_rel_still" fps 1

$collisionmodel "w_knife_rel_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
