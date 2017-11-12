// Output .MDL
$modelname props_forsaken\static\outdoor\barrier.mdl

// Directory of .BMP
$cdmaterials models/static/barrier
$staticprop

// Write textures into a ???T.mdl
//$externaltextures

$scale 1.0

// whole body
$body studio "barrier_ref.smd"

// sequences: all sequences are in $cd
$sequence idle "barrier_still" loop fps 15

$collisionmodel "barrier_ref.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}