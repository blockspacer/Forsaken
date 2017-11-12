$modelname static\gov_ammocrate\gov_ammocrate.mdl

$cdmaterials models/static/gov_ammocrate
$staticprop
$surfaceprop "solidmetal"



$scale 1.0

$body studio "gov_ammocrate_ref.smd"

$sequence idle "gov_ammocrate_still" loop fps 1

$collisionmodel "gov_ammocrate_ref.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}