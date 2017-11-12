//==============================================//
//	Default Forsaken ragdoll		//
//	Writen by Sean McBride 3/1/05		//
//	For Forsaken. www.forsaken-game.com	//
//==============================================//

$surfaceprop "flesh"

$collisionjoints govSoldier_phys {

	$mass 81.0
	$inertia 10.00
	$damping 1.00
	$rotdamping 2.50
	$rootbone "pelvis"

	$jointmerge "pelvis" "spine1"

	$jointconstrain "r_thigh" x limit -10.00 10.00 0.00
	$jointconstrain "r_thigh" y limit -35.00 15.00 0.00
	$jointconstrain "r_thigh" z limit -60.00 85.00 0.00

	$jointconstrain "r_calf" x limit -15.00 15.00 0.00
	$jointconstrain "r_calf" y limit 0.00 0.00 0.00
	$jointconstrain "r_calf" z limit -102.00 0.00 0.00

	$jointconstrain "l_thigh" x limit 0.00 0.00 0.00
	$jointconstrain "l_thigh" y limit -17.00 35.00 0.00
	$jointconstrain "l_thigh" z limit -60.00 85.00 0.00

	$jointconstrain "l_calf" x limit -15.00 15.00 0.00
	$jointconstrain "l_calf" y limit 0.00 0.00 0.00
	$jointconstrain "l_calf" z limit -102.00 0.00 0.00

	$jointconstrain "l_foot" x limit 0.00 0.00 0.00
	$jointconstrain "l_foot" y limit -3.00 3.00 0.00
	$jointconstrain "l_foot" z limit -35.00 17.00 0.00

	$jointconstrain "spine1" x limit 0.00 0.00 0.00
	$jointconstrain "spine1" y limit 0.00 0.00 0.00
	$jointconstrain "spine1" z limit -20.00 30.00 0.00

	$jointconstrain "spine3" x limit 0.00 0.00 0.00
	$jointconstrain "spine3" y limit 0.00 0.00 0.00
	$jointconstrain "spine3" z limit -20.00 30.00 0.00

	$jointconstrain "r_upperarm" x limit -80.00 80.00 0.00
	$jointconstrain "r_upperarm" y limit -25.00 135.00 0.00
	$jointconstrain "r_upperarm" z limit -80.00 60.00 0.00

	$jointconstrain "r_forearm" x limit 0.00 0.00 0.00
	$jointconstrain "r_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "r_forearm" z limit -120.00 0.00 0.00

	$jointconstrain "l_upperarm" x limit -80.00 80.00 0.00
	$jointconstrain "l_upperarm" y limit -135.00 24.00 0.00
	$jointconstrain "l_upperarm" z limit 0.00 0.00 0.00

	$jointconstrain "l_forearm" x limit 0.00 0.00 0.00
	$jointconstrain "l_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "l_forearm" z limit -120.00 0.00 0.00

	$jointconstrain "l_hand" x limit -90.00 90.00 0.00
	$jointconstrain "l_hand" y limit -25.00 25.00 0.00
	$jointconstrain "l_hand" z limit -90.00 90.00 0.00

	$jointconstrain "head" x limit -48.00 48.00 0.00
	$jointconstrain "head" y limit -41.00 41.00 0.00
	$jointconstrain "head" z limit -30.00 30.00 0.00

	$jointconstrain "r_hand" x limit -90.00 90.00 0.00
	$jointconstrain "r_hand" y limit -25.00 25.00 0.00
	$jointconstrain "r_hand" z limit -90.00 90.00 0.00

	$jointconstrain "r_foot" x limit 0.00 0.00 0.00
	$jointconstrain "r_foot" y limit -3.00 3.00 0.00
	$jointconstrain "r_foot" z limit -35.00 17.00 0.00
}
$sequence ragdoll 		"govSOldier_Ragdoll" 		FPS 30 		activity ACT_DIERAGDOLL 1