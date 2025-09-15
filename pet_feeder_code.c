//constants
const int FEED_TIME = 500;
const int PET_DIST = 20;
const int TSPEED = 5;
const int HSPEED = 10;
const int TIME_CONVERSION = 1800000;

//values for the sensor
const int CAT_NOISE = 35;
const int DOG_NOISE = 100;
const int QUIET_SOUND = 10;

//chars to represent cat and dog or no animal
const char CAT = 'd';
const char DOG = 'c';
const char NONE = 'n';

//start up function that configures all sensors (huzayfa)
void startFeeder()
{
	//configure all sensors

	//color sensors
	SensorType[S2] = sensorEV3_Color;  // Cat hopper color sensor
	wait1Msec(50);
	SensorMode[S2] = modeEV3Color_Color;

	SensorType[S3] = sensorEV3_Color;  // Dog hopper color sensora
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;

	/*
	//gyro
	SensorType[S4] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S4] = modeEV3Gyro_Calibration;
	wait1Msec(50);
	SensorMode[S4] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);
	SensorValue[S4] = 0;
	*/

	//ultrasonic
	SensorType[S1] = sensorEV3_Ultrasonic;
	wait1Msec (50);

	//sound
	SensorType[S4] = sensorSoundDB;

	//timer
	clearTimer(T1);

	//set motor encoder values
	nMotorEncoder(motorC) = 0;//table motor
	nMotorEncoder(motorB) = 0;//dog
	nMotorEncoder(motorA) = 0;//cat

	//turn 180
	turnTable(180);

	//set display
	displayTextLine(0, "%s" , "WELCOME");

	wait1Msec(2000);

	displayTextLine(0, "%s" , "CURRENTLY RUNNING");
}

//takes table to default position
void turnTable(int deg)
{
	//set turn table to default position
	if(nMotorEncoder(motorC) > deg)
	{
		motor(motorC) = (-1)*TSPEED;

		while(abs(nMotorEncoder(motorC)) > deg)
		{}

		motor(motorC) = 0;
	} if(nMotorEncoder(motorC) < deg)
	{
		motor(motorC) = TSPEED;

		while(abs(nMotorEncoder(motorC)) > deg)
		{}

		motor(motorC) = 0;
	}
}



//function to end all tasks (kelvin)
void shutDown()
{
	displayTextLine(0, "%s" , "COMMENCING SHUTDOWN");
	playTone(440, 20);
	wait1Msec(1000);
	//set turn table to default position
	turnTable(0)();

	//code to display total food fed and other metrics for 10 seconds
	displayTextLine(0, "%s" ,"TIME SINCE START");
	displayTextLine(0, "%d" ,time1[T1]);
	displayTextLine(0, "%s" , "GOODBYE");
	wait1Msec(10000);
	//end
}

//function to open either the cat or dog hopper (savban)
void dispenseFood(char animal)
{
	if(animal == CAT)
	{

		//open the motor on the cat hopper using encoder
		nMotorEncoder(motorA) = 0;
		motor(motorA) = HSPEED;

		while(abs(nMotorEncoder(motorA)) < 45)
		{}

		motor(motorA) = 0;

		//wait
		wait1Msec(FEED_TIME);

		//close the motor on the cat hopper using encoder
		nMotorEncoder(motorA) = 0;
		motor(motorA) = (-1)*HSPEED;

		while(abs(nMotorEncoder(motorA)) < 45)
		{}

		motor(motorA) = 0;

	}else if(animal == DOG)
	{

		//open the motor on the dog hopper using encoder
		nMotorEncoder(motorB) = 0;
		motor(motorB) = HSPEED;

		while(abs(nMotorEncoder(motorB)) < 45)
		{}

		motor(motorB) = 0;

		//wait
		wait1Msec(FEED_TIME);

		//close the motor on the dog hopper using encoder
		nMotorEncoder(motorB) = 0;
		motor(motorB) = (-1)*HSPEED;

		while(abs(nMotorEncoder(motorB)) < 45)
		{}

		motor(motorB) = 0;
	}
}

//function to turn table and get food depending on if given cat or dog (savban)
void getFood(char animal) //animal will be "c" or "d"
{

	//turn to 0 position
	turnTable(0);

	//dispense
	wait1Msec(500);
	dispenseFood(animal);
	wait1Msec(500);

	//turn back
	turnTable(180);
}


//function to check if cat or dog (kelvin)
char checkAnimal()
{
	if(SensorValue[S1] <= PET_DIST) // this condition should be in the main, when a pet is in range itll then call this function which waits for a sound.
	{
		//wait for sound
		while(SensorValue[S4] < QUIET_SOUND)//quiet sound is what the sound sensor hears in empty room
		{}

		int noiseDB = SensorValue[S4];

		if(noiseDB < CAT_NOISE && noiseDB > QUIET_SOUND)
		{
			//displayTextLine(1, "%s" , "cat");

			return CAT;


		} else if(noiseDB < DOG_NOISE && noiseDB > CAT_NOISE)
		{
			//displayTextLine(1, "%s" , "dog");
			return DOG;
		}
	}

	return NONE;//if no pet found
}

//function to make noise and alert user if food empty (huzayfa)
void alert(char animal)
{


	if(animal == CAT)
	{
		//display message on ev3 screen
		displayClearTextLine(1);
		displayTextLine(1, "cat food supply empty");

	} else if(animal == DOG)
	{
		//display message on ev3 screen
		displayClearTextLine(1);
		displayTextLine(1, "dog food supply empty");

	}
	for (int i=0;i<3;i++)
	{
		//use speaker on ev3
		playTone(440, 20);
		wait1Msec(1000);
	}
}

//function to check if food ran out (huzayfa)
void checkHopperEmpty(char animal)
{
	if(animal == CAT)
	{
		//check if color sensor can see other side
		if(SensorValue[S2] == (int)colorRed)
		{

			alert(CAT);

		}
	} else if(animal == DOG)
	{
		//check if color sensor can see other side
		if(SensorValue[S3] == (int)colorBlue)
		{

			alert(DOG);

		}
	}
}


int setFeedTime()
{
	int feedTime = 0;
	displayTextLine(1, "How long do you want the dispensers to be opened for?");
	displayTextLine(2,"PRESS ENTER WHEN COMPLETE");

	displayTextLine(3, "%d" , feedTime);

	while(!getButtonPress(buttonEnter)){
		if(getButtonPress(buttonUp) && feedTime < 10)
		{
			while(getButtonPress(buttonUp))
			{}
			feedTime++;
			displayTextLine(3, "%d" , feedTime);
		}
		else if (getButtonPress(buttonDown) && feedTime > 0)
		{
			while(getButtonPress(buttonDown))
			{}
			feedTime--;
			displayTextLine(3, "%d" , feedTime);
		}
	}

	while(getButtonPress(buttonEnter))
	{}


	displayTextLine(1, " ");
	displayTextLine(2, " ");
	displayTextLine(3, " ");

	return (feedTime*TIME_CONVERSION);//convert the feedtime to seconds
}

//function to handle all the logic
void runChecks()
{
	bool running = true;
	char animal = 'a';

	//feedtime
	int feedLimit = setFeedTime();
	int catLastFed = 0;
	int dogLastFed = 0;

	//check if food is in hopper
	checkHopperEmpty(CAT);
	checkHopperEmpty(DOG);

	//while loop to keep checking for something to trigger an action
	while(running == true)
	{

		//check if there is an animal
		animal = checkAnimal();

		displayTextLine(7, "%d", time1[T1]/60000);
		displayTextLine(4, "%d", catLastFed/60000);
		displayTextLine(5, "%d", dogLastFed/60000);

		if(animal == CAT)
		{
			if((time1[T1] - catLastFed) > feedLimit || catLastFed == 0)
			{
				//reset cat last fed, get food then check if the hopper is empty
				catLastFed = time1[T1];
				getFood(animal);
				checkHopperEmpty(CAT);
			}

		} else if(animal == DOG)
		{
			if((time1[T1] - dogLastFed) > feedLimit || dogLastFed == 0)
			{
				//reset dog last fed, get food then check if the hopper is empty
				dogLastFed = time1[T1];
				getFood(animal);
				checkHopperEmpty(DOG);
			}

		}

		//if statement to get out of loop
		if(getButtonPress(buttonEnter) == true)
		{
			//wait for release
			while(getButtonPress(buttonEnter) == true)
			{}

			//to break out of loop
			running = false;
		}
	}
}



task main() //savban
{
	//configure startup
	startFeeder();

	//function to handle logic
	runChecks();

	//shutdown when everything is done
	shutDown();

}
