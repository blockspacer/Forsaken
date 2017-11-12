$modelname "weapons/w_chisel_civ.mdl"
$cdmaterials "models\weapons\v_models\v_chisel_civ\"
$cdmaterials "models\weapons\w_models\w_chisel_civ\"

$model "w_chisel_civ" "w_chisel_civ_ref"

$lod 8
{
	replacemodel "w_chisel_civ_ref" "w_chisel_civ_lod_ref"
	replacematerial "v_chisel_civ" "w_chisel_civ"
}

$sequence idle "w_chisel_civ_still" fps 1

$collisionmodel "w_chisel_civ_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}
