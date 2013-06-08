var averageSpeed=0; var speedTicks=0; var totalSpeed=0; speedProperty.changed.connect(function(value) { totalSpeed += value; speedTicks++; averageSpeed = totalSpeed / speedTicks;});
var todo = new Array();
