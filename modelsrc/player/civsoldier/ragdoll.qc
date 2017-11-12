$surfaceprop "flesh"

$collisionjoints civSoldier_Collis {
				//CS Values
	$mass 84.0		//94
	$inertia 10.00		//10
	$damping 1.75		//1.00
	$rotdamping 5.50	//10
	$rootbone "pelvis"
	$jointmerge "Pelvis" "Spine1"
	$jointmassbias "pelvis" 2.60

	$jointmassbias "r_thigh" 1.30
	$jointconstrain "r_thigh" x limit -30.00 40.00 0.00
	$jointconstrain "r_thigh" y limit -35.00 15.00 0.00
	$jointconstrain "r_thigh" z limit -70.00 95.00 0.00

	$jointmassbias "r_calf" 0.80
	$jointconstrain "r_calf" x limit -15.00 15.00 0.00
	$jointconstrain "r_calf" y limit 0.00 0.00 0.00
	$jointconstrain "r_calf" z limit -108.00 0.00 0.00

	$jointmassbias "l_thigh" 1.30
	$jointconstrain "l_thigh" x limit -30.00 40.00 0.00
	$jointconstrain "l_thigh" y limit -15.00 35.00 0.00
	$jointconstrain "l_thigh" z limit -70.00 95.00 0.00

	$jointmassbias "l_calf" 0.80
	$jointconstrain "l_calf" x limit -15.00 15.00 0.00
	$jointconstrain "l_calf" y limit 0.00 0.00 0.00
	$jointconstrain "l_calf" z limit -108.00 0.00 0.00

	$jointmassbias "l_foot" 0.40
	$jointconstrain "l_foot" x limit 0.00 0.00 0.00
	$jointconstrain "l_foot" y limit -3.00 3.00 0.00
	$jointconstrain "l_foot" z limit -15.00 35.00 0.00

	$jointmassbias "spine3" 5.40
	$jointconstrain "spine3" x limit 0.00 0.00 0.00
	$jointconstrain "spine3" y limit 0.00 0.00 0.00
	$jointconstrain "spine3" z limit -45.00 45.00 0.00

	$jointmassbias "r_upperarm" 0.80
	$jointrotdamping "r_upperarm" 1.50
	$jointconstrain "r_upperarm" x limit -40.00 40.00 0.00
	$jointconstrain "r_upperarm" y limit -25.00 70.00 0.00
	$jointconstrain "r_upperarm" z limit -80.00 40.00 0.00

	$jointmassbias "r_forearm" 0.60
	$jointdamping "r_forearm" 1.50
	$jointconstrain "r_forearm" x limit 0.00 0.00 0.00
	$jointconstrain "r_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "r_forearm" z limit -151.00 0.00 0.00

	$jointmassbias "l_upperarm" 0.80
	$jointdamping "l_upperarm" 1.50
	$jointconstrain "l_upperarm" x limit -40.00 40.00 0.00
	$jointconstrain "l_upperarm" y limit -70.00 25.00 0.00
	$jointconstrain "l_upperarm" z limit -80.00 40.00 0.00

	$jointmassbias "l_forearm" 0.60
	$jointdamping "l_forearm" 1.50
	$jointconstrain "l_forearm" x limit 0.00 0.00 0.00
	$jointconstrain "l_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "l_forearm" z limit -151.00 0.00 0.00

	$jointmassbias "l_hand" 0.20
	$jointdamping "l_hand" 1.50
	$jointconstrain "l_hand" x limit -90.00 90.00 0.00
	$jointconstrain "l_hand" y limit -25.00 25.00 0.00
	$jointconstrain "l_hand" z limit -35.00 90.00 0.00

	$jointdamping "head" 1.75
	$jointconstrain "head" x limit -48.00 48.00 0.00
	$jointconstrain "head" y limit -50.00 50.00 0.00
	$jointconstrain "head" z limit -55.00 37.00 0.00

	$jointmassbias "r_hand" 0.20
	$jointdamping "r_hand" 1.50
	$jointconstrain "r_hand" x limit -90.00 90.00 0.00
	$jointconstrain "r_hand" y limit -25.00 25.00 0.00
	$jointconstrain "r_hand" z limit -35.00 90.00 0.00

	$jointmassbias "r_foot" 0.40
	$jointconstrain "r_foot" x limit 0.00 0.00 0.00
	$jointconstrain "r_foot" y limit -3.00 3.00 0.00
	$jointconstrain "r_foot" z limit -15.00 35.00 0.00
}
$sequence ragdoll 		"CivSoldier_Ragdoll" 		FPS 30 		activity ACT_DIERAGDOLL 1