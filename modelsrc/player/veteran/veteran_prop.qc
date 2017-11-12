$modelname player/veteran/veteran.mdl
$cdmaterials models/player/veteran

$staticprop

$body studio "veteran_ref.smd"
$sequence idle "verteran_still.smd" fps 1

$collisionmodel "veteran_ref" {
   $Mass 100
   $concave
}