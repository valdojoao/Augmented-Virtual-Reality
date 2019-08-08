class Player
{
    var position,	angle,		current_frame;
    var upper_mesh,	lower_mesh,	weapon_mesh;
    var lower_obj,	id,		weapon_obj;
    var color,		score,		player_bullet;
    var hit,		fire_obj,	fire_billboard;
    var fx_shoot,	fx_hit,		fx_empty,	fx_reload;
    var life,		fx_coin,	gun_type;                
    var direction,	upper_obj,	root_obj;
    
    init(player_id, player_color,player_pos);
    draw();
    update(thislevel);    
    sendPlayer(thislevel);             		//send my player POSITION_PDU
    updateEnemyPosition(received_data);     	//receive enemy POSITION_PDU
    updateEnemies(thislevel);                	//update enemies according with the received POSITION_PDU
    ShootNet(received_data);                    //receive MISSILE_PDU
    sendGun(thislevel);                      	//send TAKE_GUN_PDU
};

function player::init(player_id, player_color,player_pos)
{
    id 		= player_id;
    color 	= player_color;
    position 	= player_pos;    
    direction 	= [0,0,1];
    angle 	= 0;
    
    current_frame = START_FRAME;    
    score 	= 0;    
    life 	= 100;
    gun_type	= 0;
    
    upper_mesh = CVmNewMesh("swat_upper.aam");
    lower_mesh = CVmNewMesh("swat_lower.aam");
    upper_mesh.scale(0.04);
    lower_mesh.scale(0.04);
    
    upper_obj = CVmObj(upper_mesh);
    lower_obj = CVmObj(lower_mesh);
    weapon_mesh = CVmNewMesh("railgun.aam");
    weapon_mesh.scale(0.04);
    weapon_obj = CVmObj(weapon_mesh);    
    
    root_obj = CVmObj();
    root_obj.addChild(upper_obj);
    root_obj.addChild(lower_obj);
    root_obj.addChild(weapon_obj);
    
    root_obj.setposition(position);    
    root_obj.ModulateMaterials(color);
    
    player_bullet = bullet();
    player_bullet.init(color);
    
    fire_billboard = CvmBillboard("flame43.avi",0.5,0.5);
    fire_obj = CvmObj();
    fire_obj.LinkToBillBoard(fire_billboard);
    fire_obj.setposition(position);
    
    hit = false;
    
    fx_shoot 	= CVmVRAwav("shoot.wav",true);
    fx_hit 	= CVmVRAwav("hit.wav",true);
    fx_empty 	= CVmVRAwav("jam.wav",true);
    fx_reload 	= CVmVRAwav("reload.wav",true);
    fx_coin 	= CVmVRAwav("coin.wav",true);    
    
    var soundpos = position + [0,1,0];
    fx_shoot.setposition(soundpos);
    fx_hit.setposition(soundpos);    
}

function player::draw()
{
    root_obj.Draw(VR_FrameNumber,current_frame);
    player_bullet.Draw();
    static var counter = 0;
    if(hit)
    {
        glDisable(GL_LIGHTING);                            	//Disables OpenGL capabilities
        glEnable(GL_BLEND);                             	//OpenGL blending must be enabled before drawing trasnsparent entity
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);    	//pixel arithmetic
        glcolor(1,1,1);
        glDisable(GL_DEPTH_TEST);
        fire_obj.draw();
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        counter++;
        if(counter==50)        //time that fire will be active
        {
            hit =false;
            counter=0;
        }    
    }
}

/***************************NETWORKING FUNCTIONS***************************/
//receive bullet info from net and release a bullet with according received data
function Player::ShootNet(received_data){    
    position  = [received_data[2], received_data[3], received_data[4]];
    direction = [received_data[5], received_data[6], received_data[7]];
    player_bullet.shoot(position, direction);    
    fx_shoot.Play();    
}

//receive enemy position 
function Player::updateEnemyPosition(received_data) {
    position = [received_data[2], received_data[3], received_data[4]];    
    angle = received_data[5];    
    current_frame = received_data[6];
}

//update enemies 
function Player::updateEnemies(thislevel) 
{
    var oldposition = position;    
    
    if(current_frame > END_FRAME)    	current_frame = START_FRAME;
    if(current_frame < START_FRAME)    	current_frame = END_FRAME;
    
    direction = [sin(angle*GRAD_TO_RAD),0, cos(angle*GRAD_TO_RAD)];
    
    root_obj.setrotation(angle,0,1,0);
    root_obj.setposition(position);    
    fire_obj.setposition(position + [0,1,0]);    
    
    //if the enemy is near the random gun and pressed 9, give new gun to him
    if(enemy_pressed_9 == true && distance(position, ps) <= 1)

    {
        id_gun 		= take_new_gun[id*2+1];        //get index of the enemy who took the gun
        change_gun 	= true;
        enemy_pressed_9 = false;
    }    
    
    if(enemy_pressed_9 == true && isGunTaken == true)//drop rocket gun
    {
        meshG = "railgun.aam";
        new_gun(meshG, id_gun);    
        change_gun = false;
        isGunTaken = false;    
    }        
    
    player_bullet.update(thislevel);
}
/**************************************************************************/

function player::update(thislevel)
{
    var step 		= 0.05;
    var run 		= 1;
    var anglestep 	= 1;
    var animstep 	= 0.5;
    
    var oldposition = position;    
    
    if(keypressed(VK_SHIFT))   	run=2;
    
    if(keypressed(VK_right))   	angle-=anglestep;            
            
    if(keypressed(VK_left))   	angle+=anglestep;
        
    if(keypressed(VK_UP))
    {
        position += direction*step*run;
        current_frame +=animstep*run;    
    }
    
    if(keypressed(VK_DOWN))
    {
        position -= direction*step*run;
        current_frame -=animstep*run;        
    }    
    
    if(current_frame > END_FRAME)  	current_frame = START_FRAME;
    if(current_frame < START_FRAME)    	current_frame = END_FRAME;
    
    direction = [sin(angle*GRAD_TO_RAD),0, cos(angle*GRAD_TO_RAD)];
    
    var i = int(position.z);
    var j = int(position.x);
    
    if(thislevel.wall_matrix[i][j] == 1)    position = oldposition;        
    
    root_obj.setrotation(angle,0,1,0);
    root_obj.setposition(position);    
    fire_obj.setposition(position + [0,1,0]);    
    
    static var temp=0;    //to decrease only one bullet when tab is pressed
    temp++;
    
    if(Keypressed(VK_SPACE)) 
    {
        if(bullet_counter > 0 && temp > 25)  //if my player has bullets
        {                    
            player_bullet.shoot(position, direction);
            bullet_counter = bullets_array[bullet_counter-1]; 		//point to the bullets array on index [bullet_counter-1]
            index_id[id*4+1] = bullet_counter;
            player_bullet.sendBullet(position, direction, thislevel); 	//send bullet info to net
            fx_shoot.Play();            
            
            temp = 0;                    
        }
        
        if(bullet_counter == 0) //play sound of empty gun
        {            
            fx_empty.Play();                            
        }                
    }
    
    //if my player has no bullet and presses + or 4, reload his gun
    if(bullet_counter == 0 &&  (Keypressed(VK_ADD) || Keypressed("4")))
    {
        bullet_counter = len(bullets_array);
        index_id[id*4+1] = bullet_counter;
        fx_reload.Play();
        player_bullet.reloadBullet();        //send to the net the information that my player has reloaded 
    }
    
    //if my player is near the random gun and pressed 9, give new gun to him

    if((Keypressed(VK_NUMPAD9) || Keypressed("9")) &&  distance(position, ps) <= 1)
    {
        id_gun = take_new_gun[id*2+1];  //get index of my player 
        change_gun = true;
        sendGun(thislevel);         	//send the information that my player took rocket gun
    }    
    
    if((Keypressed(VK_NUMPAD9) || Keypressed("9")) && isGunTaken == true)//drop rocket gun
    {
        meshG = "railgun.aam";
        new_gun(meshG, id_gun);    
        change_gun = false;
        isGunTaken = false;
        sendGun(thislevel);    
    }            
        
    player_bullet.update(thislevel);
    
    var soundpos = position + [0,1,0];
    
    fx_shoot.setposition(soundpos);
    fx_hit.setposition(soundpos);    
    fx_empty.setposition(soundpos);    
    fx_reload.setposition(soundpos);
    fx_coin.setposition(soundpos);    
    
    sendPlayer(thislevel);    //send player informations to the net
}

/*****************NETWORKING FUNCTION********************************/
function player::sendPlayer(thislevel)
{
        var PDU	= vector(7);
        PDU[0]	= POSITION_PDU;
        PDU[1]	= MY_ID;
        PDU[2]	= position.x;
        PDU[3]	= position.y;
        PDU[4]	= position.z;
        PDU[5]	= angle;
        PDU[6]	= current_frame;
        
        NetVarSendTo(BROADCAST_ADDRESS, 50023, PDU);
        if(gettime() - position_timer > POSITION_TIMER_MILISEC )  //send data every 200 miliseconds
        {
          	position_timer = gettime();
            	NetVarSendTo(BROADCAST_ADDRESS,50023,PDU);
        }        
}

function player::sendGun(thislevel) //send the information that my player took rocket gun
{
        var PDU	= vector(2);
        PDU[0]	= TAKE_GUN_PDU;
        PDU[1]	= MY_ID;
                
        NetVarSendTo(BROADCAST_ADDRESS, 50023, PDU);        
}
/**************************************************************************/
 
