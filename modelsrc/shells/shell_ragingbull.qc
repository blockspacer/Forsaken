$modelname "shells\shell_ragingbull.mdl"

$cdmaterials "models\weapons\shells\"

$scale 2

$body "shell" "shell_ragingbull.smd"

$sequence idle "shell_ragingbull_still.smd" fps 1

$collisionmodel "shell_ragingbull_phys.smd" {

	$concave
	$mass 0.5
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}