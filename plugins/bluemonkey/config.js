var speedProperty = bluemonkey.subscribeTo("VehicleSpeed");

bluemonkey.log("Javascript engine works!!!!");

bluemonkey.createCustomProperty("BluemonkeyTest", false);

var bluemonkeyTest = bluemonkey.subscribeTo("BluemonkeyTest");

bluemonkeyTest.value = true;

var timer = bluemonkey.createTimer();

timer.interval = 1000;
timer.singleShot = false;
timer.timeout.connect(function() {
	bluemonkeyTest.value = !bluemonkeyTest.value;
});

timer.start();
