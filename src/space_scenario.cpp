#include "space_scenario.hpp"
#include "ship.hpp"
#include "../Server/server.hpp"
#include "bullet.hpp"

SpaceScenario::SpaceScenario() : GameObject() {
	using namespace std;
    player_ship = new Ship();
    addChild(player_ship);
    //addChild(new Ship(true));
    name = "SpaceScenario";


	network = Network();
	network.setSocketPort(8010);
	string answer = "";
	cout << "Enter server ip" << endl;
	cin >> answer;
    network.connectWith( answer.c_str() , 8011);
    ENetEvent event;
	if ( network.pollEvents(&event , 5000) > 0){
        if (event.type == ENET_EVENT_TYPE_CONNECT) {
            cout << "Joined server: " << event.peer->address.host << ":" << event.peer->address.port << endl;
		}
	}else{
        cout << "Couldn't connect with server..." << endl;
    }
}

void    SpaceScenario::_frameUpdate(){
    handlePlayerInput();
    handleNetwork();
}


void    SpaceScenario::handlePlayerInput(){
    float rot = player_ship->getShipSpriteObject()->getRotation();
    if( Input::isKeyPressed( SDL_SCANCODE_LEFT ) ){
        player_ship->getShipSpriteObject()->rotation -= 0.05;
    }
    if( Input::isKeyPressed( SDL_SCANCODE_RIGHT) ){
        player_ship->getShipSpriteObject()->rotation += 0.05;
    }
    if( Input::isKeyPressed(SDL_SCANCODE_UP ) ){
        player_ship->velocity += Vector2(0,-0.3).rotatedByRad( rot );
    }else 
    if( Input::isKeyPressed(SDL_SCANCODE_DOWN ) ){
        player_ship->velocity += Vector2(0,0.15).rotatedByRad( rot );
    }
    
    if( Input::wasKeyPressedDown(SDL_SCANCODE_Z ) ){
        new Bullet( player_ship );
		next_packet_to_send.has_shot = true;
	} else {
		next_packet_to_send.has_shot = false;
	}
    
    player_ship->velocity = player_ship->velocity * 0.97;
}

void SpaceScenario::handleNetwork( uint16_t miliseconds_timeout){
    using namespace std;
    ENetEvent event;
    while( network.pollEvents(&event , miliseconds_timeout ) ){
        if( event.type == ENET_EVENT_TYPE_CONNECT ){
            // someone joined!
            addOtherShip( event.peer );
            cout << "Someone joined!" << endl;
        }else
        if( event.type == ENET_EVENT_TYPE_DISCONNECT ){
            // someone disconnected
			if (other_ships.find(event.peer) != other_ships.end()) {
				removeOtherShip(event.peer);
				cout << "Someone disconnected!" << endl;
			}else {
                cout << "Server went nuts" << endl;
            }
        }else
        if( event.type == ENET_EVENT_TYPE_RECEIVE ){
            //received a packet
			if (event.channelID == 0) {
				//Received from server
				ENetAddress* address = (ENetAddress*)event.packet->data;
                cout << "Trying to connect with a new peer...." << endl;
				network.connectWith( address );
			}else if (event.channelID == 1) {
				//Received from peer(player)
				updateFromPack(event.peer, event.packet);
			}
			enet_packet_destroy(event.packet);
        }else
        if( event.type == ENET_EVENT_TYPE_NONE ){
            //nothing to do here!
        }
    }

	next_packet_to_send.position = player_ship->getPosition();
	next_packet_to_send.rot= player_ship->getShipSpriteObject()->getRotation();
	next_packet_to_send.current_life= player_ship->getCurrentLife();
    network.sendPacket( static_cast<void*>(&next_packet_to_send) , sizeof(next_packet_to_send) , 1 , true );
     
}

void    SpaceScenario::addOtherShip( ENetPeer* peer ){
    Ship* new_ship = new Ship(true);
    other_ships[peer] = new_ship;
    addChild( new_ship );
}
void    SpaceScenario::removeOtherShip( ENetPeer* peer ){
    Ship* ship = other_ships[peer];
    other_ships.erase( peer );
	ship->queueDelete();
}
void    SpaceScenario::updateFromPack( ENetPeer* peer , ENetPacket* packet ){
    Ship* ship = other_ships[ peer ];
    ship_packet* p = (ship_packet*)(packet->data);
    ship->setPosition( p->position );
	ship->getShipSpriteObject()->setRotation(p->rot);
	if (p->has_shot) {
		new Bullet(ship);
	}
	while (ship->getCurrentLife() > p->current_life)
		ship->takeHit();
	
}




