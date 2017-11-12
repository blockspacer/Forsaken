$modelname props_forsaken\static\outdoor\gov_jeep.mdl 

$cdmaterials models/props_forsaken/gov_jeep


$surfaceprop "solidmetal"


//$keyvalues { "prop_data" {"base" "metal.medium" } 
//These are only used for phys_props... Found out the hard way.

$staticprop
$scale 1.0

$body studio "gov_jeep_ref.smd"

$sequence idle "gov_jeep_still" loop fps 1

$collisionmodel "gov_jeep_phys" {
   $concave
   $mass 800
}