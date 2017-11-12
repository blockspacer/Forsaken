$modelname props_forsaken\static\indoor\dropinvent.mdl
$cdmaterials models/props_forsaken/dropinvent

$staticprop
$surfaceprop "metal"

$body studio "dropinvent_reference"

$sequence idle "dropinvent_still" loop fps 1

$collisionmodel "dropinvent_reference" {
     // Mass in kilograms
     $Mass 100
   $concave
}