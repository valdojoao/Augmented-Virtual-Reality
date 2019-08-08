class level
{
    var floor_mesh,         floor_obj;    
    var wall_obj,           wall_obj_counter;
    var floor_material,     floor_texture;
    var wall_material,      wall_texture;
    var wall_matrix,        fx_start;    
    var players_array,      players_number;
    var sky_obj,            sun_obj;
    var wall_mesh,          sky_mesh;        
    
    addplayer(id, color, pos);
    update();    
    init(filename);
    draw();    
};

function level::update()
{
    players_array[0].update(this);
    
    for(var i = 1; i < mylevel.players_number; i++) {        
            mylevel.players_array[i].updateEnemies(this);
        }    
    
    var threashold = 1.0;    
    
    for(var i = 0; i < players_number; i++)
    {        
        if(players_array[i].player_bullet.flying)
        {        
            for(var j = 0; j < players_number; j++)
            {            
                if(i!=j)
                {                    //player i hits j
                    if(distance(players_array[j].position, players_array[i].player_bullet.position) < threashold)
                    {                    
                        players_array[i].player_bullet.flying = false;
                        players_array[i].score++;        
                        index_id[players_array[i].id*4+2] = players_array[i].score;    
                        
                        bullet_position_timer = players_array[i].player_bullet.position;  //keep bullet position of the hitting player

                        players_array[j].hit=true;
                        players_array[j].fx_hit.Play();
                        
                        if(players_array[i].gun_type == 0) //if normal gun decrease 50
                        {
                            players_array[j].life -= 50;
                            index_id[players_array[j].id*4+3] = players_array[j].life;    
                        }else
                        {
                            players_array[j].life -= 100; //rocket gun deacrease 100
                            index_id[players_array[j].id*4+3] = players_array[j].life;
                        }                        
                        
                        if(players_array[j].life <= 0) //if player is dead
                        {                                                
                            static var temp2=0;
                            while(temp2 < 1000)
                            {
                                consolefont("Arial", 26);
                                ConsoleText(0.4, 0.4, "PLAYER " + str(players_array[j].id) +" WERE KILLED, REBORNING...");
                                temp2++;                                    
                            }
            
                            players_array[j].life = 100;
                            index_id[players_array[j].id*4+3] = players_array[j].life;

                            var player_x_z = return_x_z();
                            var playerj_x = player_x_z[0];
                            var playerj_z = player_x_z[1];
                            
                            players_array[j].position = [playerj_x, 0, playerj_z];  //reborn player on random position

                            players_array[j].sendPlayer(this);                                    
                        }                                        
                        outputln("player ", players_array[i].id, " score : ", players_array[i].score);                        
                    }                        
                }                    
            }            
        }        
    }    
}

function level::addplayer(id, color,pos)
{
    var newplayer = player();
    newplayer.init(id, color,pos);
    aadd(players_array, newplayer);
    players_number = len(players_array);        
}

function level::init(filename)
{    
    players_array = array(0);
    players_number = len(players_array);
    
    wall_obj = array(0);
    var myobj = CVmObj();
    
    aadd(wall_obj, myobj);
        
    var data = loadfile(filename);
    var lendata = len(data) - 1;    
    var lenstring = len(data[0]);    
    wall_matrix = array(lendata);
    
    for(var i = 0; i < lenstring; i++)
    {
        wall_matrix[i] = array(lenstring);
    }
        
    wall_mesh = CVMNewMesh(VRP_BOX);
    wall_mesh.scale(1,3,1);
    
    for(var i = 0; i < lenstring; i++)
    {
        for(var j = 0; j < lendata; j++)
        {
            if(data[i][j] == "*")
            {
                myobj = CVmObj();
                myobj.LinkToMesh(wall_mesh);
                myobj.setposition(j,0,i);
                aadd(wall_obj, myobj);                
                wall_matrix[i][j] = 1;
            }            
            else
                wall_matrix[i][j] = 0;
        } 
    } 
    
    wall_obj[0].linktoMesh(wall_mesh);
    
    /* FLOOR */
    floor_mesh = CVMNewMesh(VRP_BOX);
    floor_mesh.translate(0,-1,0);
    floor_mesh.scale(lenstring,1,lendata);
    floor_obj = CVMObj(floor_mesh);    
    
    floor_material = CVmMaterial();
    floor_texture = CVmTexture("MARMO.png");
    floor_material.SetTexture(floor_texture);
    floor_mesh.ForceMaterial(floor_material);
    floor_mesh.ModulateMaterials(0.0,1.0,0.0,1.0);
    
    /* WALL */
    wall_material = CVmMaterial();
    wall_texture = CVmTexture("mattoni04.png");
    wall_material.SetTexture(wall_texture);
    wall_mesh.FOrceMaterial(wall_material);
    
    /* SKY */
    sky_mesh = CVmNewMesh("Sky.AAM");
    sky_obj = CVmObj(sky_mesh);
    sky_obj.SetPosition([15, 0, 15]);    
}

function level::draw()
{
    floor_obj.Draw();        	//draw floor
    sky_obj.Draw();        	//draw sky
    
    var length = len(wall_obj);
    for(var i=0; i<length; i++)    wall_obj[i].Draw(); //draw wall
    
    for(var i=0; i<players_number; i++)
    {
        if(i == 0) //my player
        {
            players_array[i].Draw();
            player_on_off[i] = 1;
        }
        else
        {
            //GetTime() Retrieves the number of milliseconds since program was started
            if(GetTime() - g_enemy_keepalive[players_array[i].id] < 1000) //draw enemy if is active
            {
                GetTime();
                players_array[i].Draw();
                player_on_off[players_array[i].id] = 1;
            }else             
            {
                //enemy is out, dont delete him but also dont draw him, reset all his data (bullets, score, life, gun)
                player_on_off[players_array[i].id] = 0;                
                 
                index_id[players_array[i].id*4+0] = players_array[i].id;        //id
                index_id[players_array[i].id*4+1] = 6;                          //bullets
                index_id[players_array[i].id*4+2] = 0;                          //score
                players_array[i].score = 0;
                index_id[players_array[i].id*4+3] = 100;                        //life
                
                if(isGunTaken == true)
                {
                    meshG = "railgun.aam";
                    new_gun(meshG, i);
                    change_gun = false;
                    isGunTaken = false;
                }                            
            }
        }        
    }    
    
    var j=0;
    for(var i=0; i<players_number; i++)
    {
        if(players_array[i].gun_type == 1) j++;
    }
    
    if(j == 0) //if rocket gun is not taken, draw it
    {
        obj_random_gun.Draw();    
        isGunTaken = false;
    }else isGunTaken = true;
}
