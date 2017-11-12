$modelname props_forsaken\static\indoor\hanginglight01.mdl
$cdmaterials models/props_forsaken/hanginglight

$staticprop
$surfaceprop "metal"

$body studio "hanginglight01_reference"

$sequence idle "hanginglight01_still" loop fps 1

$collisionmodel "hanginglight01_reference" {
     // Mass in kilograms
     $Mass 100
   $concave
}