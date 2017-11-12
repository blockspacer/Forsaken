// Output .MDL
$modelname props_forsaken\dynamic\outdoor\booth.mdl

// Directory of .BMP
$cdmaterials models\props_forsaken\booth
$staticprop
$mostlyopaque

$surfaceprop "solidmetal"



$scale 1.3

// whole body
$body studio "booth_ref.smd"

// sequences: all sequences are in $cd
$sequence idle "booth_still" loop fps 15

$collisionmodel "booth_phys.smd" {
     // Mass in kilograms
     $Mass 100
   $concave
}