The file "demo.html" shows how to present your panorama from 360 degree.

1) Include two javascript files in this way:
   <script src="js/jquery-1.7.1.min.js"></script>
   <script src="js/jquery.cyclotron.js"></script>
   
2) Declare a class in this way:
   .cycle {
		background-image: url(MS_Lobby.jpg);
		height: 600px;
		cursor: move;
    }
   where "MS_Lobby.jpg" is your panorama. For properly displaying, you should adjust the height when loading different panorama.

3) Display the panorama by this script segment:
    <script>
	$(document).ready(function ($) {
		$('.cycle').cyclotron();
	});
	</script>
	where ".cycle" is the class you define above.
	
You are free to embed the scripts like the above ones to show your panorama in html report.