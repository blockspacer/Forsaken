$modelname "weapons/w_molotov_broken_6.mdl"
$cdmaterials "models\weapons\v_models\v_molotov\"

$model "w_molotov_broken_6" "w_molotov_broken_6_ref"

$sequence idle "w_molotov_broken_still" fps 1

$surfaceprop "glass" 
$keyvalues { "prop_data" { "base" "glass.small" } }

$collisionmodel "w_molotov_broken_6_phys" {
	$concave
	$mass 2.0
}