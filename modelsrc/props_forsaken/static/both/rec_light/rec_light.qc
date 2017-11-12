// Output .MDL
$modelname props_forsaken\static\both\rec_light.mdl

// Directory of .BMP
$cdmaterials models/props_forsaken\rec_light
$staticprop

$surfaceprop "metal"

$scale 1.1

$body studio "rec_light_ref.smd"
$sequence idle "rec_light_still" loop fps 15

$collisionmodel "rec_light_ref.smd" {
     // Mass in kilograms
     $Mass 10
   $concave
}