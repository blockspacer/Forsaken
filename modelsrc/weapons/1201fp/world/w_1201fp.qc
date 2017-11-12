$modelname "weapons/w_shot_1201fp.mdl"
$cdmaterials "models\weapons\v_models\v_1201fp\"
$cdmaterials "models\weapons\w_models\w_1201fp\"

$model "w_1201fp" "w_1201fp_ref"

$lod 4
{
	replacemodel "w_1201fp_ref" "w_1201fp_lod_ref"
	replacematerial "v_1201fp" "w_1201fp"
}

//$attachment "muzzle" "muzzle_flash" 0.00 0.00 -0.00 rotate 90 0 0
$attachment "muzzle" "R_Hand" 29.00 2.10 4.60 rotate 0 3 0
$attachment "1" "R_Hand" 11.00 0.50 3.40 rotate 0 -90 0

$sequence idle "w_1201fp_still" fps 1

$collisionmodel "w_1201fp_phys.smd" {

	$concave
	$mass 10.0
	$inertia 1.00
	$damping 0.00
	$rotdamping 0.00
}


//$sequence attack1 "w_1201fp_still" ACT_VM_PRIMARYATTACK 1 fps 1.00 {
//$sequence attack1 "w_1201fp_still" ACT_FSKN_FIRE 1 fps 1.00 {
//  { event AE_NPC_MUZZLEFLASH 0 "1201FP MUZZLE" }
//  { event 6001 0 "Muzzle" }
//}