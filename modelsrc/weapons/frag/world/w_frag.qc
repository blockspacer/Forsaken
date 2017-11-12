$modelname "weapons/w_fraggrenade.mdl"
$cdmaterials "models\weapons\v_models\v_frag\"
$cdmaterials "models\weapons\w_models\w_frag\"

$model "w_fraggrenade" "frag_reference"

$lod 5
{
	replacemodel "frag_reference" "frag_lod_ref"
	replacematerial "frag_grenade_map01" "w_frag"
}

$sequence idle "frag_draw" fps 1

$collisionmodel "frag_phys" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}