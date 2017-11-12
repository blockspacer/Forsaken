$modelname "shells\shell_hk23.mdl"

$cdmaterials "models\weapons\shells\"

$scale 1.6

$body "shell" "shell_hk23.smd"

$sequence idle "shell_hk23_still.smd" fps 1

$collisionmodel "shell_hk23_phys.smd" {

	$concave
	$mass 0.5
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}