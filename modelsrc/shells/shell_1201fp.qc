$modelname "shells\shell_1201fp.mdl"

$cdmaterials "models\weapons\shells\"

$scale 2

$body "shell" "shell_1201fp.smd"

$sequence idle "shell_1201fp_still.smd" fps 1

$collisionmodel "shell_1201fp_phys.smd" {

	$concave
	$mass 0.5
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}