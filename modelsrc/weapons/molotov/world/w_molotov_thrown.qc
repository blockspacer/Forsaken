$modelname "weapons/w_molotov_thrown.mdl"
$cdmaterials "models\weapons\v_models\v_molotov\"
$cdmaterials "models\weapons\w_models\w_molotov\"

$surfaceprop "glass" 
$keyvalues { "prop_data" { "base" "glass.small" } }

$model "w_molotov_thrown" "w_molotov_thrown_ref"

$lod 5
{
	replacemodel "w_molotov_thrown_ref" "w_molotov_thrown_lod_ref"
	replacematerial "v_molotov_nontrans" "w_molotov"
}

$sequence idle "w_molotov_thrown_still" fps 1

$collisiontext { 
	break { "model" "weapons\w_molotov_broken_1.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_2.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_3.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_4.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_5.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_6.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_7.mdl" "health" "1" "fadetime" "0" }
	break { "model" "weapons\w_molotov_broken_8.mdl" "health" "1" "fadetime" "0" }
}

$collisionmodel "w_molotov_thrown_phys" {
	$concave
	$mass 10.0
	$inertia 1.00
}