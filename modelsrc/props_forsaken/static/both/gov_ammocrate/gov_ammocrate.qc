$modelname props_forsaken\static\both\gov_ammocrate.mdl

$cdmaterials models\props_forsaken\gov_ammocrate
$staticprop

$surfaceprop "metal"

$scale 1.0

$body studio "gov_ammocrate_ref.smd"

$sequence idle "gov_ammocrate_still" loop fps 1

$collisionmodel "gov_ammocrate_ref.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}