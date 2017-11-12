$modelname "weapons/w_molotov_broken_8.mdl"
$cdmaterials "models\weapons\v_models\v_molotov\"

$model "w_molotov_broken_8" "w_molotov_broken_8_ref"

$sequence idle "w_molotov_broken_still" fps 1

$surfaceprop "glass" 
$keyvalues { "prop_data" { "base" "glass.small" } }

$collisionmodel "w_molotov_broken_8_phys" {
	$concave
	$mass 2.0
}