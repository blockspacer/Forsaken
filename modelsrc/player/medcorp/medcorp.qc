$cd "C:\Documents and Settings\n30g3n\Desktop\css"
$modelname "player/urban.mdl"
$model "Urban" "test_model_default.smd"
$lod 10
 {
  replacemodel "test_model_default" "lod1_test_model_default"
}
$lod 25
 {
  replacemodel "test_model_default" "lod2_test_model_default"
}
$lod 40
 {
  replacemodel "test_model_default" "lod3_test_model_default"
}
$lod 60
 {
  replacemodel "test_model_default" "lod4_test_model_default"
}
$shadowlod
{
  replacemodel "test_model_default" "lod5_test_model_default"
}
$poseparameter move_yaw -180.00 180.00 360.00
$poseparameter body_pitch -90.00 90.00 360.00
$poseparameter body_yaw -90.00 90.00 360.00
$cdmaterials "models\player\med_corp"
$hboxset "cstrike"
$hbox 3 "ValveBiped.Bip01_Pelvis" -8.299  -7.727  -5.429  8.299  2.007  6.583
$hbox 6 "ValveBiped.Bip01_L_Thigh" 4.576  -5.377  -3.432  22.880  3.775  4.576
$hbox 6 "ValveBiped.Bip01_L_Calf" -0.291  -4.295  -3.432  20.883  3.151  3.432
$hbox 6 "ValveBiped.Bip01_L_Foot" -2.288  -1.144  -2.579  5.720  4.576  2.007
//$hbox 6 "ValveBiped.Bip01_L_Toe0" -2.860  -2.808  -2.860  4.004  1.196  1.716
$hbox 7 "ValveBiped.Bip01_R_Thigh" 4.576  -5.377  -3.432  22.880  3.775  4.576
$hbox 7 "ValveBiped.Bip01_R_Calf" -0.291  -4.295  -3.432  20.883  3.151  3.432
$hbox 7 "ValveBiped.Bip01_R_Foot" -2.288  -1.144  -2.007  5.720  4.576  2.579
//$hbox 7 "ValveBiped.Bip01_R_Toe0" -2.860  -2.808  -1.716  4.004  1.196  2.860
$hbox 3 "ValveBiped.Bip01_Spine1" -9.152  -1.258  -8.008  4.576  11.326  8.008
$hbox 2 "ValveBiped.Bip01_Spine2" -3.068  -3.432  -9.693  13.572  10.317  9.693
$hbox 1 "ValveBiped.Bip01_Neck1" -0.104  -4.576  -2.860  5.512  2.288  2.860
$hbox 1 "ValveBiped.Bip01_Head1" -0.416  -6.240  -3.640  9.360  4.441  3.016
$hbox 4 "ValveBiped.Bip01_L_UpperArm" -1.144  -2.579  -2.288  14.872  2.579  2.288
$hbox 4 "ValveBiped.Bip01_L_Forearm" -1.716  -2.402  -2.402  13.156  2.402  2.402
$hbox 4 "ValveBiped.Bip01_L_Hand" 0.291  -2.465  -1.716  6.583  1.550  2.860
$hbox 5 "ValveBiped.Bip01_R_UpperArm" -1.144  -2.579  -2.288  14.872  2.579  2.288
$hbox 5 "ValveBiped.Bip01_R_Forearm" -1.716  -2.402  -2.402  13.156  2.402  2.402
$hbox 5 "ValveBiped.Bip01_R_Hand" 0.291  -2.465  -2.860  6.583  1.550  1.716

$attachment "grenade0" "ValveBiped.Bip01_Pelvis" 7.28 1.56 1.04 rotate 0.00 0.00 -80.00
$attachment "grenade1" "ValveBiped.Bip01_Pelvis" 7.80 1.56 -3.64 rotate 0.00 0.00 -80.00
$attachment "grenade2" "ValveBiped.Bip01_Pelvis" -7.28 1.56 1.04 rotate 0.00 0.00 -80.00
$attachment "grenade3" "ValveBiped.Bip01_Pelvis" -7.80 1.56 -3.64 rotate 0.00 0.00 -80.00
$attachment "pistol" "ValveBiped.Bip01_R_Thigh" 8.32 1.04 -3.64 rotate 0.00 -0.00 -0.00
$attachment "primary" "ValveBiped.Bip01_Spine2" 0.00 -4.16 -2.08 rotate 0.00 -0.00 -0.00
$attachment "defusekit" "ValveBiped.Bip01_Pelvis" 0.00 1.56 -8.32 rotate -90.00 -90.00 0.00
$attachment "lfoot" "ValveBiped.Bip01_L_Foot" 0.00 5.20 0.00 rotate -0.00 0.00 -0.00
$attachment "rfoot" "ValveBiped.Bip01_R_Foot" -0.00 5.20 0.00 rotate 0.00 -0.00 0.00
$attachment "muzzle_flash" "ValveBiped.Bip01_L_Hand" 0.00 0.00 0.00 rotate -0.00 0.00 -0.00
$includemodel "player/cs_player_shared.mdl"
$includemodel "player/ak_anims_ct.mdl"
$surfaceprop "flesh"
$eyeposition 0.000 0.000 73.000
$illumposition 0.699 0.019 37.726
$sequence ragdoll "ragdoll" ACT_DIERAGDOLL 1 fps 30.00
$ikchain rhand ValveBiped.Bip01_R_Hand knee  0.707 0.707 0.000
$ikchain lhand ValveBiped.Bip01_L_Hand knee  0.707 0.707 0.000
$ikchain rfoot ValveBiped.Bip01_R_Foot knee  0.707 -0.707 0.000
$ikchain lfoot ValveBiped.Bip01_L_Foot knee  0.707 -0.707 0.000
$ikchain ikclip ValveBiped.weapon_bone_Clip knee  0.707 -0.707 0.000
$collisionjoints "phymodel.smd" {

	$mass 94.0
	$inertia 10.00
	$damping 1.75
	$rotdamping 10.00
	$rootbone "valvebiped.bip01_pelvis"
	$jointmassbias "valvebiped.bip01_pelvis" 2.60
	$jointdamping "valvebiped.bip01_pelvis" 1.00

	$jointmassbias "valvebiped.bip01_spine2" 3.40
	$jointconstrain "valvebiped.bip01_spine2" x limit -15.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_spine2" y limit -12.00 12.00 0.00
	$jointconstrain "valvebiped.bip01_spine2" z limit -20.00 15.00 0.00

	$jointmassbias "valvebiped.bip01_r_upperarm" 0.80
	$jointconstrain "valvebiped.bip01_r_upperarm" x limit -40.00 40.00 0.00
	$jointconstrain "valvebiped.bip01_r_upperarm" y limit -20.00 91.00 0.00
	$jointconstrain "valvebiped.bip01_r_upperarm" z limit -80.00 24.00 0.00

	$jointmassbias "valvebiped.bip01_l_upperarm" 0.80
	$jointconstrain "valvebiped.bip01_l_upperarm" x limit -40.00 40.00 0.00
	$jointconstrain "valvebiped.bip01_l_upperarm" y limit -91.00 20.00 0.00
	$jointconstrain "valvebiped.bip01_l_upperarm" z limit -80.00 23.00 0.00

	$jointmassbias "valvebiped.bip01_l_forearm" 0.60
	$jointconstrain "valvebiped.bip01_l_forearm" x limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_l_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_l_forearm" z limit -123.00 -3.00 0.00

	$jointmassbias "valvebiped.bip01_l_hand" 0.20
	$jointconstrain "valvebiped.bip01_l_hand" x limit -52.00 37.00 0.00
	$jointconstrain "valvebiped.bip01_l_hand" y limit -12.00 8.00 0.00
	$jointconstrain "valvebiped.bip01_l_hand" z limit -52.00 -1.00 0.00

	$jointmassbias "valvebiped.bip01_r_forearm" 0.60
	$jointconstrain "valvebiped.bip01_r_forearm" x limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_r_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_r_forearm" z limit -120.00 -3.00 0.00

	$jointmassbias "valvebiped.bip01_r_hand" 0.20
	$jointconstrain "valvebiped.bip01_r_hand" x limit -60.00 60.00 0.00
	$jointconstrain "valvebiped.bip01_r_hand" y limit -8.00 12.00 0.00
	$jointconstrain "valvebiped.bip01_r_hand" z limit -52.00 -1.00 0.00

	$jointmassbias "valvebiped.bip01_r_thigh" 1.30
	$jointconstrain "valvebiped.bip01_r_thigh" x limit -12.00 12.00 0.00
	$jointconstrain "valvebiped.bip01_r_thigh" y limit -8.00 33.00 0.00
	$jointconstrain "valvebiped.bip01_r_thigh" z limit -79.00 24.00 0.00

	$jointmassbias "valvebiped.bip01_r_calf" 0.80
	$jointconstrain "valvebiped.bip01_r_calf" x limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_r_calf" y limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_r_calf" z limit -6.00 116.00 0.00

	$jointconstrain "valvebiped.bip01_head1" x limit -72.00 60.00 0.00
	$jointconstrain "valvebiped.bip01_head1" y limit -37.00 33.00 0.00
	$jointconstrain "valvebiped.bip01_head1" z limit -19.00 45.00 0.00

	$jointmassbias "valvebiped.bip01_l_thigh" 1.30
	$jointconstrain "valvebiped.bip01_l_thigh" x limit -12.00 12.00 0.00
	$jointconstrain "valvebiped.bip01_l_thigh" y limit -8.00 33.00 0.00
	$jointconstrain "valvebiped.bip01_l_thigh" z limit -79.00 24.00 0.00

	$jointmassbias "valvebiped.bip01_l_calf" 0.80
	$jointconstrain "valvebiped.bip01_l_calf" x limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_l_calf" y limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_l_calf" z limit -6.00 116.00 0.00

	$jointmassbias "valvebiped.bip01_l_foot" 0.40
	$jointconstrain "valvebiped.bip01_l_foot" x limit -17.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_l_foot" y limit -25.00 19.00 0.00
	$jointconstrain "valvebiped.bip01_l_foot" z limit -15.00 35.00 0.00

	$jointmassbias "valvebiped.bip01_r_foot" 0.40
	$jointconstrain "valvebiped.bip01_r_foot" x limit -17.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_r_foot" y limit -25.00 19.00 0.00
	$jointconstrain "valvebiped.bip01_r_foot" z limit -15.00 35.00 0.00
}
