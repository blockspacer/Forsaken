$modelname props_forsaken\static\indoor\candle_stick_wallmount.mdl
$cdmaterials models/props_forsaken/candle_stick_wallmount

$staticprop
$surfaceprop "metal"

$body studio "candle_stick_wallmount_ref"

$sequence idle "candle_stick_wallmount_still" loop fps 1

$collisionmodel "candle_stick_wallmount_ref" {
     // Mass in kilograms
     $Mass 100
   $concave
}