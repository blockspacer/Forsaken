// Output .MDL
$modelname props_forsaken\dynamic\outdoor\booth_door.mdl

$cdmaterials models\props_forsaken\booth

$mostlyopaque
$scale 1.29

$body studio "booth_door_ref.smd"

$sequence idle "booth_door_still" loop fps 1

$surfaceprop "metal"
$keyvalues { "prop_data" { "base" "metal.medium" } }

$collisionmodel "booth_door_phys.smd" {
     // Mass in kilograms
     $Mass 7
   $concave
}