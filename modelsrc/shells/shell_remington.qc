$modelname "shells\shell_remington.mdl"

$cdmaterials "models\weapons\shells\"

$scale 2

$body "shell" "shell_remington.smd"

$sequence idle "shell_remington_still.smd" fps 1

$collisionmodel "shell_remington_phys.smd" {

	$concave
	$mass 0.5
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}