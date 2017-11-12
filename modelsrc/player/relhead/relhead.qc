// Output .MDL
$modelname player/relhead/relhead.mdl

// Directory of .BMP
$cdmaterials models/player/relhead
$staticprop

// Write textures into a ???T.mdl
//$externaltextures

$scale 1.0

// whole body
$body studio "relhead_ref.smd"

// sequences: all sequences are in $cd
$sequence idle "relhead_still" loop fps 15

$collisionmodel "relhead_ref" {
     // Mass in kilograms
     $Mass 100
   $concave
}