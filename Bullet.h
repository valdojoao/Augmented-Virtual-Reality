class bullet
{
    var mesh, obj, position;
    var direction, speed;
    var color,    flying;
    
    init(col);
    shoot(pos, dir);
    update(thislevel);
    draw();        
    sendBullet(position, direction, thislevel);                    //send MISSILE_PDU
    reloadBullet();                                                //send RELOAD_PDU
};

function bullet::init(col)
{
    var size;
    
    if(change_gun == true) //Rocket gun bullet data
    {
        size  = 0.2;
        speed = 0.3;
        mesh=CVmNewMesh(VRP_SPHERE);
    }    
    else
    {   //Normal gun bullet data
        size  = 0.1;
        speed = 0.1;
        mesh  = CVmNewMesh(VRP_BOX);
    }    
    mesh.normalize(size);
    obj = CVmObj(mesh);
    flying = false;    
    color = col;
    mesh.ModulateMaterials(color);    
}

function bullet::shoot(pos, dir)
{
        position = pos + [0,1,0];
        direction = dir;
        flying = true;        
}

function bullet::update(thislevel)
{
    if(flying)
    {        
        position += direction * speed;        
        var i = int(position.z);
        var j = int(position.x);            
        if(thislevel.wall_matrix[i][j] == 1)    flying = false;            
        else obj.SetPosition(position);
    }
}

function bullet::draw()
{
    if(flying)
        obj.Draw();    
}

/*****************NETWORKING FUNCTION********************************/
function bullet::sendBullet(position, direction, thislevel) //send bullet info to net when my player shoots
{
        var PDU	= vector(9);
        
        PDU[0]	= MISSILE_PDU;
        PDU[1]	= MY_ID;
        PDU[2]	= position.x;
        PDU[3]	= position.y;
        PDU[4]	= position.z;
        PDU[5]	= direction.x;
        PDU[6]  = direction.y;
        PDU[7]  = direction.z;
        PDU[8]  = bullet_counter;
        
        //NetVarSendTo - Send the contents to a remote host using the UDP protocol.     
        //               - string address, int port, ANYTYPE varToSend        
        NetVarSendTo(BROADCAST_ADDRESS, 50023, PDU);
}

function bullet::reloadBullet() //send to the net the information that my player has reloaded
{
        var PDU    = vector(3);
        
        PDU[0]    = RELOAD_PDU;
        PDU[1]    = MY_ID;
        PDU[2]    = bullet_counter;                        
        NetVarSendTo(BROADCAST_ADDRESS, 50023, PDU);
}
/***************************************************************/

