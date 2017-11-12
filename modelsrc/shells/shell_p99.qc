$modelname "shells\shell_p99.mdl"

$cdmaterials "models\weapons\shells\"

$scale 2

$body "shell" "shell_p99.smd"

$sequence idle "shell_p99_still.smd" fps 1

$collisionmodel "shell_p99_phys.smd" {

	$concave
	$mass 0.5
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}