$modelname props_forsaken\static\outdoor\hardcastle.mdl

$cdmaterials models/props_forsaken/hardcastle

$staticprop
$surfaceprop "solidmetal"



$scale 1.0

$body studio "hardcastle_ref.smd"
$sequence idle "hardcastle_still" loop fps 1

$collisionmodel "hardcastle_ref.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}