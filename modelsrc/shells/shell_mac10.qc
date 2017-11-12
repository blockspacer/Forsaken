$modelname "shells\shell_mac10.mdl"

$cdmaterials "models\weapons\shells\"

$scale 2

$body "shell" "shell_mac10.smd"

$sequence idle "shell_mac10_still.smd" fps 1

$collisionmodel "shell_mac10_phys.smd" {

	$concave
	$mass 0.5
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}