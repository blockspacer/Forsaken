$modelname props_forsaken\static\outdoor\subtechsign.mdl

$cdmaterials models/props_forsaken/subtechsign

$staticprop
$surfaceprop "metal"

$body studio "subtechsign_reference"

$sequence idle "subtechsign_still" loop fps 1

$collisionmodel "subtechsign_reference" {
     // Mass in kilograms
     $Mass 100
   $concave
}