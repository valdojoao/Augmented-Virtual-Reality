/* Set global scene parameters */
SET SCENE_FOV          	= 60;
SET SCENE_NEAR         	= 0.1;
SET SCENE_FAR        	= 100;
SET AUDIO_OPENAL     	= 1;

#define START_FRAME 16
#define END_FRAME     48

/*************************** NETWORKING************************ */
#define BROADCAST_ADDRESS "192.168.116.255"

/* 1 up to 4 reserved for players ID */
#define MY_ID 3        

/*PDU INFO
5 - Manages players position; FR:playerUpdate(), sendPlayer(), updateEnemyPosition(), updateEnemies(), networkFrame();
6 - Manages bullet info when player shoots; FR:playerUpdate(),sendBullet(), shootNet(), networkFrame(); 
7 - checks wheter the enemy is in the game; FR:networkFrame();
8 - updates the score and life; FR: levelUpdate(), drawScore(), networkFrame();
9 - Manages Rocket gun position; FR:networkFrame();
10- info that player has new gun; FR: playerUpdate(), sendGun(), networkFrame();
11- info player has reloaded; FR:playerUpdate(), reloadBullet(), networkFrame();
12- update the bullet counter; FR:networkFrame();*/

#define POSITION_PDU     	5
#define MISSILE_PDU     	6
#define KEEPALIVE_PDU     	7
#define SCORE_PDU         	8
#define GUN_PDU         	9
#define TAKE_GUN_PDU     	10
#define RELOAD_PDU        	11
#define BULLET_PDU        	12

#define PACKET_LOSS_PROB        	20
#define POSITION_TIMER_MILISEC     	200
/**************************************************************** */

var CamPos = [30.0,30.0,30.0];      	// camera position
var Light0;                         	// Light globals 
var PosL = [0.0, 10.0, 10.0];        	// default light position  
var globalcam = 0;                    	//variable to manage different cameras

/* random_gun_mesh, gun_obj, obj_random_gun     - info of the rocket gun
*  bullets_array     - round of n  bullets
*  ps                - position of the rocket gun
*  id_gun             - holds the index of the player who took the rocket gun
*  array_x_z        - holds random values of x and z
*/

var bullets_array,    random_gun_mesh,    gun_obj;
var obj_random_gun,    ps;
var id_gun, array_x_z;

var isGunTaken     = false;        	//if any player takes rocket gun set to true, else false 
var change_gun     = false;        	//if a player changes gun set to true, else false

static var bullet_counter = 0;    	//pointer to the index of bullets array

var mylevel, getnormalFrequency; 	//getnormalFrequency - holds the frequency of the normal gun

/*************************** NETWORKING**************************/
var g_enemy_keepalive             = vector(5);
var g_enemy_id                    = vector(5);
var g_enemy_angle                 = vector(5);
var g_enemy_added                 = vector(2*5);	//0 - enemy id, 1 - enemy index on players array
var is_enemy_added                = vector(2*5);      	//0 - enemy id, 1 - true or false

var index_id     = vector(4*5);            		//0 - Id, 1 - Bullets, 2 - Score, 3 - Life
var gun          = array(3);                		//send and receive rocket gun position

var player_on_off 	= array(6);            		//holds 1 if player on index i is in the game, else 0
var take_new_gun    	= vector(2*5);        		//0 - Id, 1 - index in the players array
var enemy_pressed_9 	= false;            		//when enemy tries to take the rocket gun

var meshG ="";                            		//mesh for the new gun

var keep_alive_timer   		= 0.0;
var keep_alive_timer2		= 0.0;
var position_timer           	= 0.0;
var bullet_position_timer 	= 0;

function networkFrame(thislevel);       	//receive all PDUs and send the ones that need to be sent  periodically
function MyReceive();                    	//receives contents from a remote host
function new_gun(meshG, id_gun);        	//set a new gun to players
function return_x_z();                    	//generate random positions
/******************************************************************/



/* Function declarations */
function CameraMoveMouse();
function DrawGrid(col, size);

//return distance between pos1 and pos2
function distance(pos1,pos2);               



